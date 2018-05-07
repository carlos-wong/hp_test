#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <linux/input.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>
#include <stdbool.h>
#include <sys/ioctl.h>
#include "../ArkCar.h"
#include "../ArkNaviProtocol.h"
#include "YAOXI.h"



#define YAOXI_DBG printf
//#define YAOXI_DBG

typedef struct __si4730_setting_info
{
    unsigned int	fm_min_freq;		/* 最小阈值 */
    unsigned int	fm_max_freq;		/* 最大阈值 */
    unsigned int	am_min_freq;
    unsigned int	am_max_freq;
    unsigned int	min_freq;			/* 当前模式下最小频率 */
    unsigned int	max_freq;			/* 当前模式下最大频率 */
    unsigned int	granularity;		/* 搜台步进: 单位10k */
    unsigned int	signal_intensity;	/* 信号质量 */
    unsigned int	loc_dx;				/* 远程/近程搜台 */
    unsigned int	cur_freq;			/* 当前频率 */
    unsigned char	area_select;		/* 地区选择 */
    unsigned char	stereo_select;		/* 立体声或单声道选择*/
    unsigned char	band_mode;			/* FM 或AM 模式选择*/
}si4730_setting_info_t;

typedef struct __yaoxi_private_protocol {
    unsigned int ark_type;
    unsigned int type;
    unsigned char data[4];
    int len;
}yaoxi_private_protocol_t;


#define SI4730_IOCTL									0x58
#define SI4730_IOCTL_RESET								_IO(SI4730_IOCTL, 0x01)
#define SI4730_IOCTL_BAND_MODE_SELECT					_IO(SI4730_IOCTL, 0x02)
#define SI4730_IOCTL_SET_FREQ							_IO(SI4730_IOCTL, 0x03)
#define SI4730_IOCTL_SET_SEEK_BAND_BOTTOM				_IO(SI4730_IOCTL, 0x04)	//seek band bottom configure
#define SI4730_IOCTL_SET_SEEK_BAND_TOP					_IO(SI4730_IOCTL, 0x05)	//seek band top configure
#define SI4730_IOCTL_SET_SEEK_FREQ_SPACING				_IO(SI4730_IOCTL, 0x06)	//seek space step configure(搜台间隔)
#define SI4730_IOCTL_SET_SEEK_TUNE_SNR_THRESHOLD		_IO(SI4730_IOCTL, 0x07)
#define SI4730_IOCTL_SET_SEEK_TUNE_RSSI_THRESHOLD		_IO(SI4730_IOCTL, 0x08)
#define SI4730_IOCTL_SEEK_START							_IO(SI4730_IOCTL, 0x09)
#define SI4730_IOCTL_GET_RSQ_STATUS						_IO(SI4730_IOCTL, 0x0A)
#define SI4730_IOCTL_GET_TUNE_STATUS					_IO(SI4730_IOCTL, 0x0B)
#define SI4730_IOCTL_SET_BLEND_STEREO_THRESHOLD			_IO(SI4730_IOCTL, 0x0C)
#define SI4730_IOCTL_SET_BLEND_MONO_THRESHOLD			_IO(SI4730_IOCTL, 0x0D)
#define SI4730_IOCTL_MANUAL_SEEK						_IO(SI4730_IOCTL, 0x0E)
#define SI4730_IOCTL_AUTO_SEEK							_IO(SI4730_IOCTL, 0x0F)
#define SI4730_IOCTL_SET_FM_STEREO						_IO(SI4730_IOCTL, 0x10)
#define SI4730_IOCTL_SET_FM_LOC							_IO(SI4730_IOCTL, 0x11)
#define SI4730_IOCTL_PLAY								_IO(SI4730_IOCTL, 0x12)
#define SI4730_IOCTL_RX_VOLUME							_IO(SI4730_IOCTL, 0x13)
#define SI4730_IOCTL_RX_HARD_MUTE						_IO(SI4730_IOCTL, 0x14)
#define SI4730_IOCTL_SET_AREA							_IO(SI4730_IOCTL, 0x15)
#define SI4730_IOCTL_GET_INFO							_IO(SI4730_IOCTL, 0x16)
#define SI4730_IOCTL_SET_FM_SEEK_BAND_BOTTOM			_IO(SI4730_IOCTL, 0x17)
#define SI4730_IOCTL_SET_FM_SEEK_BAND_TOP				_IO(SI4730_IOCTL, 0x18)
#define SI4730_IOCTL_POWER_DOWN							_IO(SI4730_IOCTL, 0x19)


//stereo
#define SI4730_FM_STEREO				0x00
#define SI4730_FM_MONO					0x01
//loc status
#define SI4730_FM_LOC					0x01	/* 近程 */
#define SI4730_FM_DX					0x00	/* 远程 */

//mute status
#define SI4730_MUTE_OFF					0x00
#define SI4730_MUTE_ON					0x01
//seek direction
#define SI4730_SEEK_UP					0x01
#define SI4730_SEEK_DOWN				0x00
//band mode
#define SI4730_MODE_FM					0x00
#define SI4730_MODE_AM					0x01
//area select
#define AREA_IN_ASIA					0x00
#define AREA_IN_US						0x01

//return value
#define SI4730_OK						0x00
#define SI4730_I2C_ERROR				0x01
#define SI4730_ACK_TIMEOUT				0x02
#define SI4730_ACK_ERROR				0x03
#define SI4730_CMD_FAILED				0x04
#define SI4730_INVALID_PARAMETER		0x05
#define SI4730_INVALID_CHANNEL			0x06


#define SI4730_FIFO_NAME        "/tmp/si4730_fifo"
#define BAND_NUM    5
static pthread_t yaoxi_pthread = 0;
static pthread_mutex_t yaoxi_mutex;
static int auto_search_status_valid = 1;
static int is_searching = 0;
static int si4730_fd;
static unsigned int yaoxi_store_station[BAND_NUM][RADIO_PRESET_NUM] = {
    /* 默认值，没有存台时使用此默认值 */
    {8750, 9010, 9810, 10600, 10800, 8750},  /* FM1 */
    {8750, 9010, 9810, 10600, 10800, 8750},  /* FM2 */
    {8750, 9010, 9810, 10600, 10800, 8750},  /* FM3 */
    {522, 603, 999, 1404, 1620, 522},    /* AM1 */
    {522, 603, 999, 1404, 1620, 522}    /* AM2 */
};

static struct RadioInfo radio_info = {
    .min_freq = -1,
    .max_freq = -1,
    .granularity = -1,
    .signal_intensity = -1,
    .status = ARS_Undefine,
    .curfreq_stereo = -1,
    .use_stereo = -1,
    .scanmode = -1,
    .loc_dx = -1,
    .current_band = ARB_Undefine,
    .current_preset = -1,
    .current_freq = -1,
    .preset_list = {-1, -1, -1, -1, -1, -1},
};


static int report_radio_event(enum ArkRadioType type, int value);
static bool is_freq_valid(int freq);


static int si4730_ioctl(unsigned int cmd, unsigned int *arg)
{
    return ioctl(si4730_fd, cmd, (unsigned long)arg);
}

static int si4730_get_info(si4730_setting_info_t *info)
{
    return si4730_ioctl(SI4730_IOCTL_GET_INFO, (unsigned int *)info);
}

/********************************************************
 **
 *	description	: receive singal quality
 *	argument:
 *				channel_freq: current channel frequency
                seek_fail	: seek error status
                valid_channel: if current channel valid
 * 	return value:
 *				success	: SI4730_OK
 *				fail		: others
 **
 ********************************************************/

static int si4730_get_tune_status(unsigned int *channel_freq, unsigned int *seek_fail, unsigned int *valid_channel)
{
    unsigned int arg[3];
    int ret;

    ret = si4730_ioctl(SI4730_IOCTL_GET_TUNE_STATUS, arg);
    if(ret == SI4730_OK)
    {
        *channel_freq = arg[0];
        *seek_fail = arg[1];
        *valid_channel = arg[2];
    }

    return ret;
}

static unsigned int si4730_get_cur_freq(void)
{
    unsigned int channel_freq, seek_fail, valid_channel;
    int ret;

/* after change am/fm mode, the frequency is 0, so should process this prosition */
    ret = si4730_get_tune_status(&channel_freq, &seek_fail, &valid_channel);

    return ((ret == SI4730_OK) ? channel_freq : -1);
}

/********************************************************
 **
 *	description	: receive singal quality
 *	argument:
 *				none
 * 	return value:
 *				success	: singal quality
 *				fail		: 0
 **
 ********************************************************/
static int si4730_get_rsq(void)
{
    unsigned int rsq,ret;

    ret = si4730_ioctl(SI4730_IOCTL_GET_RSQ_STATUS, &rsq);
    printf("app rsq:%d,ret:%d\n",rsq,ret);
    if(ret != SI4730_OK)
        return 0;

    return rsq;
}

static int si4730_set_snr_threshold(unsigned int snr)
{
    return si4730_ioctl(SI4730_IOCTL_SET_SEEK_TUNE_SNR_THRESHOLD, &snr);
}

static int si4730_set_rssi_threshold(unsigned int rssi)
{
    return si4730_ioctl(SI4730_IOCTL_SET_SEEK_TUNE_SNR_THRESHOLD, &rssi);
}

#if 0
int si4730_set_fm_loc(unsigned int on_off)
{
    int ret;

    if(on_off == SI4730_FM_LOC_ON)
    {
        ret = SI4730_SET_FM_SEEK_TUNE_RSSI_THRESHOLD(0x23);
    }
    else if(on_off == SI4730_FM_LOC_OFF)
    {
        ret = SI4730_SET_FM_SEEK_TUNE_RSSI_THRESHOLD(0x14);
    }
    else
    {
        return SI4730_INVALID_PARAMETER;
    }

    return ret;
}
#else
/********************************************************
 **
 *	description	: set loc
 *	argument:
 *				on_off	: loc on/off
 * 	return value:
 *				success	: SI4730_OK;
 *				fail		: other value
 **
 ********************************************************/
static int si4730_fm_set_loc(unsigned int on_off)
{
    return si4730_ioctl(SI4730_IOCTL_SET_FM_LOC, &on_off);
}
#endif
#if 0
static int si4730_fm_stereo_choose(unsigned int stereo)
{
    int ret;

    if(stereo == SI4730_FM_STEREO)
    {
        ret = SI4730_SET_FM_BLEND_STEREO_THRESHOLD(10);
        if(ret != SI4730_OK)
        {
            return ret;
        }
        ret = SI4730_SET_FM_BLEND_MONO_THRESHOLD(8);
        if(ret != SI4730_OK)
        {
            return ret;
        }
    }
    else if(stereo == SI4730_FM_MONO)
    {
        ret = SI4730_SET_FM_BLEND_STEREO_THRESHOLD(127);
        if(ret != SI4730_OK)
        {
            return ret;
        }
        ret = SI4730_SET_FM_BLEND_MONO_THRESHOLD(127);
        if(ret != SI4730_OK)
        {
            return ret;
        }
    }
    else
    {
        return SI4730_INVALID_PARAMETER;
    }

    return ret;
}
#else
/********************************************************
 **
 *	description	: choose stereo/mono mode
 *	argument:
 *				audio_method	: mode
 * 	return value:
 *				success	: SI4730_OK;
 *				fail		: other value
 **
 ********************************************************/
static int si4730_fm_stereo_choose(unsigned int audio_method)
{
    return si4730_ioctl(SI4730_IOCTL_SET_FM_STEREO, &audio_method);
}
#endif

/********************************************************
 **
 *	description	: set a frequency
 *	argument:
 *				freq	: frequency value
 * 	return value:
 *				success	: SI4730_OK;
 *				fail		: other value
 **
 ********************************************************/
static int si4730_set_freq(unsigned int freq)
{
    return si4730_ioctl(SI4730_IOCTL_SET_FREQ, &freq);
}

/********************************************************
 **
 *	description	: manual search if the frequency is in a valid channel
 *	argument:
 *				freq	: start frequency
 *				dir	: search direction
 * 	return value:
 *				success	: SI4730_OK;
 *				fail		: SI4730_INVALID_CHANNEL
 **
 ********************************************************/
static int si4730_manual_search(unsigned int freq)
{
    return si4730_ioctl(SI4730_IOCTL_MANUAL_SEEK, &freq);

}
/********************************************************
 **
 *	description	: auto search a station
 *	argument:
 *				dir	: search direction
 *				search_step	: search frequeney from last frequency (unit:10K)
 * 	return value:
 *				success	: frequency;
 *				fail		: -1
 **
 ********************************************************/
static int si4730_auto_search(unsigned int dir, unsigned int search_step)
{
    si4730_setting_info_t info;
    unsigned int curr_freq,seek_fail,valid_channel;
    unsigned int max_freq,min_freq;
    unsigned int freq;
    unsigned char buf[2];
    int timeout;
    int ret;

    ret = si4730_get_info(&info);
    if(ret != SI4730_OK)
    {
        return -1;
    }

    if(info.band_mode == SI4730_MODE_AM)
    {
        max_freq = info.am_max_freq;
        min_freq = info.am_min_freq;
    }
    else
    {
        max_freq = info.fm_max_freq;
        min_freq = info.fm_min_freq;
    }

    ret = si4730_get_tune_status(&curr_freq, &seek_fail, &valid_channel);
//	if(ret != SI4730_OK)
//	{
//		return -1;
//	}
    if((curr_freq > max_freq) || (curr_freq < min_freq))
    {
        if(dir == SI4730_SEEK_UP)
            curr_freq = min_freq;
        else
            curr_freq = max_freq;
    }

    freq = curr_freq;

    if(search_step == 0)
        search_step = 10;	//default: 100K

    while(1)
    {
        if(dir == SI4730_SEEK_UP)
            freq += search_step;
        else
            freq -= search_step;

        if(freq > max_freq)
        {
            freq = min_freq;
        }
        else if(freq < min_freq)
        {
            freq = max_freq;
        }

        ret = si4730_manual_search(freq);
        if(ret == SI4730_OK)
        {
            break;
        }

        if(is_freq_valid(freq))
        {
            report_radio_event(ART_GetFreq, freq);
        }

        //return original frequency
        if(freq == curr_freq)
        {
            break;
            //return -1;
        }

        timeout = 5;
        while(timeout--)
        {
            if(auto_search_status_valid == 0)
            {
                return 0;
            }
            usleep(1000);
        }

    }

    return freq;
}


/********************************************************
 **
 *	description	: auto search a station
 *	argument:
 *				dir	: search direction
 * 	return value:
 *				success	: frequency;
 *				fail		: -1
 **
 ********************************************************/
static int si4730_auto_seek(unsigned int dir)
{
    int freq,ret;

    ret = si4730_ioctl(SI4730_IOCTL_AUTO_SEEK, &dir);
    freq = dir;

    if(ret == SI4730_OK)
        return freq;

    return -1;
}

/********************************************************
 **
 *	description	: set a frequency and mute off
 *	argument:
 *				freq	: frequency value
 * 	return value:
 *				success	: SI4730_OK;
 *				fail		: other value
 **
 ********************************************************/
static int si4730_paly(unsigned int freq)
{
    return si4730_ioctl(SI4730_IOCTL_PLAY, &freq);
}

static int si4730_band_mode_select(unsigned int mode)
{
    return si4730_ioctl(SI4730_IOCTL_BAND_MODE_SELECT, &mode);
}

static int si4730_set_volume(unsigned int volume)
{
    return si4730_ioctl(SI4730_IOCTL_RX_VOLUME, &volume);
}

static int si4730_mute(unsigned int mute)
{
    return si4730_ioctl(SI4730_IOCTL_RX_HARD_MUTE, &mute);
}

static int si4730_set_area(unsigned int area)
{
    return si4730_ioctl(SI4730_IOCTL_SET_AREA, &area);
}

static int si4730_power_down(void)
{
    return si4730_ioctl(SI4730_IOCTL_POWER_DOWN, NULL);
}


/************************************************************************************************************
 *
 ************************************************************************************************************/
#if 1
static int report_radio_event(enum ArkRadioType type, int value)
{
    unsigned char buf[2];

    switch(type)
    {
        case ART_GetMinFreq:
            if(value >> 16)
                break;
            radio_info.min_freq = value;
            buf[0] = radio_info.min_freq >> 8;
            buf[1] = radio_info.min_freq;
            notify_radio_event(ART_GetMinFreq, buf, 2);
            break;
        case ART_GetMaxFreq:
            radio_info.max_freq = value;
            buf[0] = radio_info.max_freq >> 8;
            buf[1] = radio_info.max_freq;
            notify_radio_event(ART_GetMaxFreq, buf, 2);
            break;
        case ART_GetGranularity:
            radio_info.granularity = value;
            buf[0] = radio_info.granularity;
            notify_radio_event(ART_GetGranularity, buf, 1);
            break;
        case ART_GetSignalIntensity:
            radio_info.signal_intensity = value;  //use default
            buf[0] = radio_info.signal_intensity;
            notify_radio_event(ART_GetSignalIntensity, buf, 1);
            break;
        case ART_GetFreq:
            if(value >> 16)
                break;
            radio_info.current_freq = value;
            buf[0] = radio_info.current_freq >> 8;
            buf[1] = radio_info.current_freq;
            notify_radio_event(ART_GetFreq, buf, 2);
            break;
        case ART_GetBand:
            radio_info.current_band = value;
            buf[0] = radio_info.current_band;
            notify_radio_event(ART_GetBand, buf, 1);
            break;
        case ART_GetStatus:
            radio_info.status = value;
            buf[0] = radio_info.status;
            notify_radio_event(ART_GetStatus, buf, 1);
            break;
        case ART_GetAutoScan:
            radio_info.scanmode = ART_GetAutoScan;
            notify_radio_event(radio_info.scanmode, NULL, 0);
            break;
        case ART_GetManualScan:
            radio_info.scanmode = ART_GetManualScan;
            notify_radio_event(radio_info.scanmode, NULL, 0);
            break;
        case ART_GetCurfreqStereo:
            radio_info.curfreq_stereo = ART_GetCurfreqStereo;
            notify_radio_event(radio_info.curfreq_stereo, NULL, 0);
            break;
        case ART_GetCurfreqMono:
            radio_info.curfreq_stereo = ART_GetCurfreqMono;
            notify_radio_event(radio_info.curfreq_stereo, NULL, 0);
            break;
        case ART_GetUseStereo:
            radio_info.use_stereo = ART_GetUseStereo;
            notify_radio_event(radio_info.use_stereo, NULL, 0);
            break;
        case ART_GetUseMono:
            radio_info.use_stereo = ART_GetUseMono;
            notify_radio_event(radio_info.use_stereo, NULL, 0);
            break;
        case ART_GetLOC:
            radio_info.loc_dx = ART_GetLOC;
            notify_radio_event(radio_info.loc_dx, NULL, 0);
            break;
        case ART_GetDX:
            radio_info.loc_dx = ART_GetDX;
            notify_radio_event(radio_info.loc_dx, NULL, 0);
            break;
        case ART_GetPreset:
            radio_info.current_preset = value;
            buf[0] = radio_info.current_preset;
            notify_radio_event(ART_GetPreset, buf, 1);
            break;
        default:

            break;
    }
}
#endif

static bool is_freq_valid(int freq)
{
    if((freq >= radio_info.min_freq) && (freq <= radio_info.max_freq))
        return true;

    return false;
}

/* 保存搜索到的台到flash */
static int save_station_info_to_flash(void)
{
    FILE *fp = fopen("/data/audio_station","w+");

    if(fp == NULL)
    {
        printf("## err: fopen /data/audio_station failed\n");
        return -1;

    }

    fwrite(yaoxi_store_station,1,sizeof(yaoxi_store_station),fp);

    fclose(fp);
}

/* 从flash读取保存的台 */
static int read_station_info_from_flash(void)
{
    FILE *fp = fopen("/data/audio_station","r");

    if(fp == NULL)
    {
        printf("## err: fopen /data/audio_station failed\n");
        return -1;
    }

    fread(yaoxi_store_station,1,sizeof(yaoxi_store_station),fp);

#if 0   //print log
    int i,j;
    for(i=0; i<5; i++)
    {
        printf("station[%d]:",i );
        for(j=0; j<6; j++)
            printf(" %d",yaoxi_store_station[i][j]);
        printf("\n");
    }
#endif

    fclose(fp);

    return 0;
}

static int set_radio_info(int band)
{
    si4730_setting_info_t info;
    unsigned char buf[12];
    unsigned int freq;
    int i;

    si4730_get_info(&info);
    report_radio_event(ART_GetStatus, ARS_INIT);
    report_radio_event(ART_GetMinFreq, info.min_freq);
    report_radio_event(ART_GetMaxFreq, info.max_freq);
    report_radio_event(ART_GetGranularity, info.granularity);
    report_radio_event(ART_GetSignalIntensity, 100);    //default 100
    report_radio_event(ART_GetManualScan, 0);

    if(band < ARB_AM1)
    {
        if(info.stereo_select == SI4730_FM_STEREO)
            report_radio_event(ART_GetCurfreqStereo, 0);
        else
            report_radio_event(ART_GetCurfreqMono, 0);

        report_radio_event(ART_GetUseMono, 0);  //default

        if(info.loc_dx == SI4730_FM_LOC)
            report_radio_event(ART_GetLOC, 0);
        else
            report_radio_event(ART_GetDX, 0);
    }
    else if(band == ARB_AM1)
    {
        /* band切换到AM，上报UI屏蔽立体声显示图标 */
        report_radio_event(ART_GetCurfreqMono, 0);
        /* band切换到AM，上报UI屏蔽远近程显示图标 */
        report_radio_event(ART_GetDX, 0);
    }

    if(band < ARB_AM1)
    {
        if(info.band_mode == SI4730_MODE_AM)
            report_radio_event(ART_GetBand, ARB_AM1);
        else
            report_radio_event(ART_GetBand, band);
    }
    else
    {
        if(info.band_mode == SI4730_MODE_FM)
            report_radio_event(ART_GetBand, ARB_FM1);
        else
            report_radio_event(ART_GetBand, band);
    }

    freq = yaoxi_store_station[radio_info.current_band][0];
    if(is_freq_valid(freq))
    {
        report_radio_event(ART_GetFreq, freq);
        report_radio_event(ART_GetPreset, 1);
    }
    else
    {
        freq = info.min_freq;
        report_radio_event(ART_GetFreq, freq);
        report_radio_event(ART_GetPreset, 0);
    }

    for(i=0; i<6; i++)
    {
        radio_info.preset_list[i] = yaoxi_store_station[radio_info.current_band][i];
        buf[2*i] = radio_info.preset_list[i] >> 8;
        buf[2*i+1] = radio_info.preset_list[i];
    }
    notify_radio_event(ART_GetPresetList, buf, 12);

    report_radio_event(ART_GetStatus, ARS_IDLE);

    return 0;
}

static int get_radio_info(void)
{
    unsigned char buf[12];
    int i;

    buf[0] = radio_info.min_freq >> 8;
    buf[1] = radio_info.min_freq;
    notify_radio_event(ART_GetMinFreq, buf, 2);

    buf[0] = radio_info.max_freq >> 8;
    buf[1] = radio_info.max_freq;
    notify_radio_event(ART_GetMaxFreq, buf, 2);

    buf[0] = radio_info.granularity;
    notify_radio_event(ART_GetGranularity, buf, 1);

    buf[0] = radio_info.signal_intensity;
    notify_radio_event(ART_GetSignalIntensity, buf, 1);

    buf[0] = radio_info.status;
    notify_radio_event(ART_GetStatus, buf, 1);

    if(radio_info.current_band < ARB_AM1)
    {
        notify_radio_event(radio_info.curfreq_stereo, NULL, 0);
        notify_radio_event(radio_info.use_stereo, NULL, 0);
        notify_radio_event(radio_info.loc_dx, NULL, 0);
    }

    notify_radio_event(radio_info.scanmode, NULL, 0);

    buf[0] = radio_info.current_band;
    notify_radio_event(ART_GetBand, buf, 1);

    buf[0] = radio_info.current_freq >> 8;
    buf[1] = radio_info.current_freq;
    notify_radio_event(ART_GetFreq, buf, 2);

    buf[0] = radio_info.current_preset;
    notify_radio_event(ART_GetPreset, buf, 1);

    for(i=0; i<6; i++)
    {
        buf[2*i] = radio_info.preset_list[i] >> 8;
        buf[2*i+1] = radio_info.preset_list[i];
    }
    notify_radio_event(ART_GetPresetList, buf, 12);

    return 0;
}

/* 协议解析 */
static int protocol_parse(yaoxi_private_protocol_t pri_data)
{
    unsigned char buf[12];
    unsigned int freq;
    unsigned int count;
    int i,j,k;
    int timeout;
    int ret = -1;

   switch (pri_data.ark_type)
   {
       case AT_Radio:
       {
           switch (pri_data.type)
           {
               case ART_KEY_Smeter:
                   YAOXI_DBG("### ART_KEY_Smeter\n");

                   break;

               case ART_QueryFreqInfo:
                   YAOXI_DBG("### ART_QueryFreqInfo\n");
                   get_radio_info();
                   break;

               case ART_QueryPresetListInfo:
                   YAOXI_DBG("### ART_QueryPresetListInfo\n");

                   break;

               case ART_Key_1:
               case ART_Key_2:
               case ART_Key_3:
               case ART_Key_4:
               case ART_Key_5:
               case ART_Key_6:
                   /* 六个预存台按键按下，播放对应频率的台 */
                   freq = yaoxi_store_station[radio_info.current_band][pri_data.type - ART_Key_1];
                   ret = si4730_paly(freq);
                   if(ret == SI4730_OK)
                   {
                       report_radio_event(ART_GetFreq, freq);
                       report_radio_event(ART_GetPreset, pri_data.type - ART_KEY);
                   }
                   break;

               case ART_KEY_Band:
                   /* band模式切换(FM1,FM2,FM3,AM1,AM2) */
                   if(radio_info.current_band <= ARB_Undefine)
                       radio_info.current_band = ARB_FM1;

                   if(radio_info.current_band == ARB_FM3)
                   {
                      ret = si4730_band_mode_select(SI4730_MODE_AM);
                      if(ret == SI4730_OK)
                      {
                          /* 设置radio信息 */
                          set_radio_info(ARB_AM1);
                      }
                      freq = yaoxi_store_station[radio_info.current_band][0];
                      if(!is_freq_valid(freq))
                      {
                          freq = radio_info.min_freq;
                      }
                   }
                   else if(radio_info.current_band == ARB_AM2)
                   {
                       ret = si4730_band_mode_select(SI4730_MODE_FM);
                       if(ret == SI4730_OK)
                       {
                           /* 设置radio信息 */
                           set_radio_info(ARB_FM1);

                           if(si4730_fm_stereo_choose(SI4730_FM_MONO) == SI4730_OK)
                           {
                               /* band切换到FM，上报UI屏蔽立体声显示图标(默认单声道) */
                               report_radio_event(ART_GetCurfreqMono, 0);
                           }
                       }

                       freq = yaoxi_store_station[radio_info.current_band][0];
                       if(!is_freq_valid(freq))
                       {
                           freq = radio_info.min_freq;
                       }
                   }
                   else
                   {
                       report_radio_event(ART_GetBand, radio_info.current_band+1);

                       /* 上报存台列表 */
                       for(i=0; i<6; i++)
                       {
                           radio_info.preset_list[i] = yaoxi_store_station[radio_info.current_band][i];
                           buf[2*i] = radio_info.preset_list[i] >> 8;
                           buf[2*i+1] = radio_info.preset_list[i];
                       }
                       notify_radio_event(ART_GetPresetList, buf, 12);

                       freq = yaoxi_store_station[radio_info.current_band][0];
                       if(is_freq_valid(freq))
                       {
                           report_radio_event(ART_GetFreq, freq);
                           report_radio_event(ART_GetPreset, 1);
                       }
                       else
                       {
                           freq = radio_info.min_freq;
                           report_radio_event(ART_GetFreq, freq);
                           report_radio_event(ART_GetPreset, 0);
                       }
                   }

                   ret = si4730_paly(freq);
                   if(ret != SI4730_OK)
                   {
                       si4730_mute(SI4730_MUTE_OFF);
                   }
                   break;

               case ART_KEY_TuningUp:
                   /* 向上步进搜台 */
                   freq = si4730_get_cur_freq();
                   if(is_freq_valid(freq))
                   {
                       ret = si4730_paly(freq - radio_info.granularity);
                       if(ret == SI4730_OK)
                       {
                           report_radio_event(ART_GetFreq, freq - radio_info.granularity);
                       }
                   }
                   break;

               case ART_KEY_TuningDown:
                   /* 向下步进搜台 */
                   freq = si4730_get_cur_freq();
                   if(is_freq_valid(freq))
                   {
                       ret = si4730_paly(freq + radio_info.granularity);
                       if(ret == SI4730_OK)
                       {
                           report_radio_event(ART_GetFreq, freq + radio_info.granularity);
                       }
                   }
                   break;

               case ART_KEY_SeekUp:
                   /* 向上（低频段）搜台,直到搜到一个合法通道的台 */
                   report_radio_event(ART_GetAutoScan, 0);
                   report_radio_event(ART_GetStatus, ARS_SEEK_UP);
                   is_searching = 1;

                   freq = si4730_auto_search(SI4730_SEEK_DOWN, radio_info.granularity);
                   if(is_freq_valid(freq))
                   {
                       report_radio_event(ART_GetFreq, freq);
                       ret = si4730_paly(freq);
                       if(ret != SI4730_OK)
                       {
                           si4730_mute(SI4730_MUTE_OFF);
                       }
                   }
                   else
                   {
                       si4730_mute(SI4730_MUTE_OFF);
                   }
                   report_radio_event(ART_GetStatus, ARS_IDLE);
                   break;

               case ART_KEY_SeekDown:
                   /* 向下（高频段）搜台,直到搜到一个合法通道的台 */
                   report_radio_event(ART_GetAutoScan, 0);
                   report_radio_event(ART_GetStatus, ARS_SEEK_DOWN);
                   is_searching = 1;

                   freq = si4730_auto_search(SI4730_SEEK_UP, radio_info.granularity);
                   if(is_freq_valid(freq))
                   {
                       report_radio_event(ART_GetFreq, freq);

                       ret = si4730_paly(freq);
                       if(ret != SI4730_OK)
                       {
                           si4730_mute(SI4730_MUTE_OFF);
                       }
                   }
                   else
                   {
                       si4730_mute(SI4730_MUTE_OFF);
                   }
                   report_radio_event(ART_GetStatus, ARS_IDLE);
                   break;

               case ART_KEY_AutoScan:
                   /* 自动搜台并存储 */
                   freq = radio_info.min_freq;
                   count = 0;
                   is_searching = 1;

                   /* 上报搜台模式为自动搜台 */
                   report_radio_event(ART_GetAutoScan, 0);
                   /* 上报当前状态为自动搜台（UI此时静音） */
                   report_radio_event(ART_GetStatus, ARS_AUTOSTORE);
                   /* 预存台为0 */
                   report_radio_event(ART_GetPreset, 0);

                   while(1)
                   {
                       if(freq > radio_info.max_freq)
                       {
                           break;
                       }

                       /* 带宽变化 */
                       if(count/6 == 0)
                       {
                           /* 上报带宽 */
                           report_radio_event(ART_GetBand, ARB_FM1 + count/6);

                           /* 上报存台列表 */
                           for(i=0; i<6; i++)
                           {
                               radio_info.preset_list[i] = yaoxi_store_station[radio_info.current_band][i];
                               buf[2*i] = radio_info.preset_list[i] >> 8;
                               buf[2*i+1] = radio_info.preset_list[i];
                           }
                           notify_radio_event(ART_GetPresetList, buf, 12);
                       }

                       ret = si4730_manual_search(freq);
                       if(ret == SI4730_OK)
                       {
                           /* 存台 */
                           if(radio_info.current_band < ARB_AM1)   //store fm station
                           {
                               if(count < (ARB_AM1 - ARB_FM1)*RADIO_PRESET_NUM)
                               {
                                   if(count%6 == 0) /* 切换带宽 */
                                   {
                                       /* 上报带宽 */
                                       report_radio_event(ART_GetBand, ARB_FM1 + count/6);
                                   }
                                   /* 缓存台 */
                                   yaoxi_store_station[ARB_FM1+count/6][count%6] = freq;
                                   /* 上报存台列表 */
                                   for(i=0; i<6; i++)
                                   {
                                       radio_info.preset_list[i] = yaoxi_store_station[radio_info.current_band][i];
                                       buf[2*i] = radio_info.preset_list[i] >> 8;
                                       buf[2*i+1] = radio_info.preset_list[i];
                                   }
                                   notify_radio_event(ART_GetPresetList, buf, 12);
                                   /* 上报预设值 */
                                   report_radio_event(ART_GetPreset, count%6+1);
                               }
                           }
                           else     //store am station
                           {
                               if(count < (ARB_LAST - ARB_AM1)*RADIO_PRESET_NUM)
                               {
                                   if(count%6 == 0)  /* 切换带宽 */
                                   {
                                       /* 上报带宽 */
                                       report_radio_event(ART_GetBand, ARB_AM1 + count/6);
                                   }
                                   /* 缓存台 */
                                   yaoxi_store_station[ARB_AM1+count/6][count%6] = freq;
                                   /* 上报存台列表 */
                                   for(i=0; i<6; i++)
                                   {
                                       radio_info.preset_list[i] = yaoxi_store_station[radio_info.current_band][i];
                                       buf[2*i] = radio_info.preset_list[i] >> 8;
                                       buf[2*i+1] = radio_info.preset_list[i];
                                   }
                                   notify_radio_event(ART_GetPresetList, buf, 12);
                                   /* 上报预设值 */
                                   report_radio_event(ART_GetPreset, count%6+1);
                               }
                           }

                           count++;
                       }
                       /* 上报频率 */
                       report_radio_event(ART_GetFreq, freq);
                       /*  */
                       freq += radio_info.granularity;

                       /* 延时判断搜台是否被打断（不添加延时，按键不能及时终止搜台） */
                       timeout = 5;
                       while(timeout--)
                       {
                           /* 搜台被打断 */
                           if(auto_search_status_valid == 0)
                           {
                               /* 存台 */
                               save_station_info_to_flash();

                               /* 设置并播放频点 */
                               ret = si4730_paly(freq);
                               if(ret != SI4730_OK)
                               {
                                   si4730_mute(SI4730_MUTE_OFF);
                               }

                               /* 上报当前状态（UI此时取消静音） */
                               report_radio_event(ART_GetStatus, ARS_IDLE);

                               goto search_end;
                           }
                           usleep(1000);
                       }
                   }

                   /* 存台 */
                   save_station_info_to_flash();

                   /* band切换到FM1或AM1 */
                   if(radio_info.current_band < ARB_AM1)
                       report_radio_event(ART_GetBand, ARB_FM1);
                   else
                       report_radio_event(ART_GetBand, ARB_AM1);

                   /* 上报存台列表给UI */
                   for(i=0; i<6; i++)
                   {
                       radio_info.preset_list[i] = yaoxi_store_station[radio_info.current_band][i];
                       buf[2*i] = radio_info.preset_list[i] >> 8;
                       buf[2*i+1] = radio_info.preset_list[i];
                   }
                   notify_radio_event(ART_GetPresetList, buf, 12);

                   /* 预存台显示第一个 */
                   report_radio_event(ART_GetPreset, 1);
                   /* 自动搜台结束后，播放第一个台 */
                   freq = radio_info.preset_list[0];
                   /* 上报当前频率 */
                   report_radio_event(ART_GetFreq, freq);
                   /* 设置并播放频点 */
                   ret = si4730_paly(freq);
                   if(ret != SI4730_OK)
                   {
                       si4730_mute(SI4730_MUTE_OFF);
                   }
                   /* 上报当前状态（UI此时取消静音） */
                   report_radio_event(ART_GetStatus, ARS_IDLE);
                   break;

               case ART_KEY_PS:
                   /* 存台预览 */
                   is_searching = 1;
                   report_radio_event(ART_GetStatus, ARS_BROWSE_PRESTORE);

                   if(radio_info.current_band < ARB_AM1)    //fm
                     {
                          for(i=0; i<3; i++)
                          {
                              report_radio_event(ART_GetBand, ARB_FM1+i);

                              for(k=0; k<6; k++)
                              {
                                  radio_info.preset_list[k] = yaoxi_store_station[radio_info.current_band][k];
                                  buf[2*k] = radio_info.preset_list[k] >> 8;
                                  buf[2*k+1] = radio_info.preset_list[k];
                              }
                              notify_radio_event(ART_GetPresetList, buf, 12);
                              for(j=0; j<RADIO_PRESET_NUM; j++)
                              {
                                  freq = yaoxi_store_station[i][j];
                                  if(is_freq_valid(freq))
                                  {
                                      /* 台频率合法则进行预览 */
                                      ret = si4730_paly(freq);
                                      if(ret != SI4730_OK)
                                      {
                                          si4730_mute(SI4730_MUTE_OFF);
                                      }
                                      report_radio_event(ART_GetFreq, freq);
                                      report_radio_event(ART_GetPreset, j+1);

                                      /* 设定预览时间 */
                                      timeout = 5000;
                                      while(--timeout)
                                      {
                                          /* 预览时被触摸按键打断则终止预览 */
                                          if(auto_search_status_valid == 0)
                                          {
                                              /* 上报当前频率 */
                                              report_radio_event(ART_GetFreq, freq);
                                              goto search_end;
                                          }
                                          usleep(1000);
                                      }
                                  }
                                  else /*  台频率不合法则预览下一个 */
                                  {
                                      /* 延时判断有没被按键打断 */
                                      timeout = 5;
                                      while(--timeout)
                                      {
                                          /* 预览时被触摸按键打断则终止预览 */
                                          if(auto_search_status_valid == 0)
                                          {
                                              goto fm_scan_over;
                                          }
                                          usleep(1000);
                                      }

                                      continue;
                                      //goto fm_scan_over;
                                  }
                              }
                          }
fm_scan_over:
                          /* 预览结束后通知UI默认显示 */
                          report_radio_event(ART_GetBand, ARB_FM1);

                          for(k=0; k<6; k++)
                          {
                              radio_info.preset_list[k] = yaoxi_store_station[radio_info.current_band][k];
                              buf[2*k] = radio_info.preset_list[k] >> 8;
                              buf[2*k+1] = radio_info.preset_list[k];
                          }
                          notify_radio_event(ART_GetPresetList, buf, 12);

                          freq = yaoxi_store_station[ARB_FM1][0];
                          if(!is_freq_valid(freq))
                              freq = radio_info.min_freq;
//                          ret = si4730_paly(freq);
//                          if(ret != SI4730_OK)
//                          {
//                              si4730_mute(SI4730_MUTE_OFF);
//                          }

                          report_radio_event(ART_GetFreq, freq);
                          report_radio_event(ART_GetPreset, 1);
                     }
                     else //am
                     {
                         for(i=0; i<2; i++)
                         {
                             report_radio_event(ART_GetBand, ARB_AM1+i);

                             for(k=0; k<6; k++)
                             {
                                 radio_info.preset_list[k] = yaoxi_store_station[radio_info.current_band][k];
                                 buf[2*k] = radio_info.preset_list[k] >> 8;
                                 buf[2*k+1] = radio_info.preset_list[k];
                             }
                             for(j=0; j<RADIO_PRESET_NUM; j++)
                             {
                                 freq = yaoxi_store_station[i+3][j];

                                 if(is_freq_valid(freq))
                                 {
                                     /* 台频率合法则进行预览 */
                                     si4730_paly(freq);
                                     report_radio_event(ART_GetFreq, freq);
                                     report_radio_event(ART_GetPreset, j+1);

                                     timeout = 5000;
                                     while(timeout--)
                                     {
                                         /* 预览时被触摸按键打断则终止预览 */
                                         if(auto_search_status_valid == 0)
                                         {
                                             /* 上报当前频率 */
                                             report_radio_event(ART_GetFreq, freq);
                                             goto search_end;
                                         }
                                         usleep(1000);
                                     }
                                 }
                                 else /*  台频率不合法则预览结束 */
                                 {
                                     /* 延时判断有没被按键打断 */
                                     timeout = 5;
                                     while(--timeout)
                                     {
                                         /* 预览时被触摸按键打断则终止预览 */
                                         if(auto_search_status_valid == 0)
                                         {
                                             goto am_scan_over;
                                         }
                                         usleep(1000);
                                     }

                                     continue;
                                     //goto am_scan_over;
                                 }
                             }
                         }
am_scan_over:
                         report_radio_event(ART_GetBand, ARB_AM1);

                         for(k=0; k<6; k++)
                         {
                             radio_info.preset_list[k] = yaoxi_store_station[radio_info.current_band][k];
                             buf[2*k] = radio_info.preset_list[k] >> 8;
                             buf[2*k+1] = radio_info.preset_list[k];
                         }
                         notify_radio_event(ART_GetPresetList, buf, 12);

                         freq = yaoxi_store_station[ARB_AM1][0];
                         if(!is_freq_valid(freq))
                             freq = radio_info.min_freq;
//                         ret = si4730_paly(freq);
//                         if(ret != SI4730_OK)
//                         {
//                             si4730_mute(SI4730_MUTE_OFF);
//                         }

                         report_radio_event(ART_GetFreq, freq);
                         report_radio_event(ART_GetPreset, 1);
                    }
                   report_radio_event(ART_GetStatus, ARS_IDLE);
                   break;

               case ART_KEY_LOC:
                   /* 远/近程选择 */
                   if(radio_info.current_band < ARB_AM1)
                   {
                       if(radio_info.loc_dx == ART_GetLOC)
                       {
                           ret = si4730_fm_set_loc(SI4730_FM_DX);
                           if(ret == SI4730_OK)
                           {
                               report_radio_event(ART_GetDX, 0);
                           }
                       }
                       else
                       {
                           ret = si4730_fm_set_loc(SI4730_FM_LOC);
                           if(ret == SI4730_OK)
                           {
                               report_radio_event(ART_GetLOC, 0);
                           }
                       }
                   }
                   break;
               case ART_KEY_TA:
               case ART_KEY_PTY:
               case ART_KEY_AF:
               case ART_KEY_REG:
               case ART_KEY_CT:
               case ART_KEY_EON:
               case ART_KEY_RDS:
               case ART_KEY_RDSType:
               case ART_KEY_PtySeek:
                   break;

               case ART_KEY_StereoStateChange:
                   /* 立体声/单声道选择 */
                   if(radio_info.current_band < ARB_AM1)
                   {
                       if(radio_info.curfreq_stereo == ART_GetCurfreqStereo)
                       {
                           ret = si4730_fm_stereo_choose(SI4730_FM_MONO);
                           if(ret == SI4730_OK)
                           {
                               report_radio_event(ART_GetCurfreqMono, 0);
                           }
                       }
                       else
                       {
                           ret = si4730_fm_stereo_choose(SI4730_FM_STEREO);
                           if(ret == SI4730_OK)
                           {
                               report_radio_event(ART_GetCurfreqStereo, 0);
                           }

                       }
                   }
                   break;

               case ART_KEY_FMFreq:
               case ART_KEY_AMFreq:
                   /* 频点设置 */
                   freq = (pri_data.data[0] << 8) | pri_data.data[1];
                   ret = si4730_paly(freq);
                   if(ret == SI4730_OK)
                   {
                       report_radio_event(ART_GetFreq, freq);
                   }
                   break;

               case ART_KEY_PresetSave:
                   /* 长按六个预选台按钮中任一个，则会把当前频率存到当前位置 */
                   freq = si4730_get_cur_freq();
                   if(is_freq_valid(freq))
                   {
                       report_radio_event(ART_GetFreq, freq);
                       report_radio_event(ART_GetPreset, pri_data.data[0]);

                       radio_info.preset_list[radio_info.current_preset-1] = yaoxi_store_station[radio_info.current_band][radio_info.current_preset-1] = freq;
                       save_station_info_to_flash();
                       for(i=0; i<6; i++)
                       {
                           buf[2*i] = radio_info.preset_list[i] >> 8;
                           buf[2*i+1] = radio_info.preset_list[i];
                       }
                       notify_radio_event(ART_GetPresetList, buf, 12);
                   }
                   break;
               case ART_KEY_PresetLoad:
                   YAOXI_DBG("### ART_KEY_PresetLoad\n");

                   break;
              default:
                   break;
           }
           break;
       }
       default:
           break;
   }

search_end:
    /* 状态位复位 */
    is_searching = 0;
    if(auto_search_status_valid == 0)
    {
        pthread_mutex_lock(&yaoxi_mutex);
        auto_search_status_valid = 1;
        pthread_mutex_unlock(&yaoxi_mutex);
    }

    return 0;
}

static int fifo_write(unsigned char *buf, int len)
{
    int fd,ret;

    fd = open(SI4730_FIFO_NAME, O_WRONLY);
    if(fd < 0)
    {
        printf("### yaoxi fifo_write open fifo failed\n");
        return -1;
    }

    /* 写管道(读端一直在读不会阻塞，如果读端停止读取，则写端会阻塞) */
    ret = write(fd, buf, len);
    if(ret < 0)
    {
        printf("### yaoxi write fifo failed\n");
    }

    close(fd);

    return ret;
}

/* 解析并处理UI协议 */
static void* yaoxi_pthread_handler(void* arg)
{
    yaoxi_private_protocol_t pri_data;
    int ret,fd;

    /* 创建管道进行通信(收音搜台等处理比较耗时，需要单独线程来处理，所以需要借助管道来实现通信) */
    ret = mkfifo(SI4730_FIFO_NAME, 0666);
    if(ret < 0)
    {
        printf("### ERR: yaoxi mkfifo failed\n");
    }

    fd = open(SI4730_FIFO_NAME, O_RDWR);
    if(fd < 0)
    {
        perror("### yaoxi fifo_read open fifo failed");
        return NULL;
    }

    while(1)
    {
        /* 读管道(阻塞读，直到有数据写进来) */
        ret = read(fd, (unsigned char *)(&pri_data), sizeof(pri_data));
        if(ret > 0)
        {
            /* 解析协议数据包 */
            protocol_parse(pri_data);
        }
    }

    close(fd);

    return NULL;
}


static int yaoxi_read(const int fd)
{

    return 0;
}

/* UI端调用写函数与底层驱动层通信 */
static int yaoxi_write(const struct ArkProtocol* protocol)
{
    yaoxi_private_protocol_t pri_data;

    switch (protocol->type)
    {
        case AT_Radio:
            pri_data.ark_type = AT_Radio;
            if (NULL != protocol->data)
            {
                struct ArkRadio *radio = (struct ArkRadio*)protocol->data;
                pri_data.type = radio->type;
                pri_data.len = radio->length;
                if(pri_data.len > 0)
                {
                    if(radio->length > 4)
                        radio->length = 4;
                    memcpy(pri_data.data, radio->data, radio->length);
                    //memcpy(pri_data.data, radio->data, radio->length);
                }

                switch (radio->type)
                {
                    case ART_QueryFreqInfo:
                        fifo_write((unsigned char *)(&pri_data), sizeof(pri_data));
                        break;

                    case ART_Key_1:
                    case ART_Key_2:
                    case ART_Key_3:
                    case ART_Key_4:
                    case ART_Key_5:
                    case ART_Key_6:
                    case ART_KEY_Band:
                    case ART_KEY_TuningUp:
                    case ART_KEY_TuningDown:
                    case ART_KEY_SeekUp:
                    case ART_KEY_SeekDown:
                    case ART_KEY_AutoScan:
                    case ART_KEY_PS:
                    case ART_KEY_LOC:
                    case ART_KEY_StereoStateChange:
                    case ART_KEY_FMFreq:
                    case ART_KEY_AMFreq:
                    case ART_KEY_PresetSave:
                        if(is_searching == 1)
                        {
                            /* 搜台或预览时被触摸按键打断，标志位清零以停止搜台或预览 */
                            pthread_mutex_lock(&yaoxi_mutex);
                            auto_search_status_valid = 0;
                            pthread_mutex_unlock(&yaoxi_mutex);
                        }
                        else
                        {
                            /* 写管道通信(搜台或预览时，按键按下搜台或预览被打断，但按键不响应) */
                            fifo_write((unsigned char *)(&pri_data), sizeof(pri_data));
                        }
                        break;
                    default:
                        break;
                }

            }
            break;

        default:
            break;
    }

    return 0;
}


enum ArkCustomer yaoxi_init(void)
{
    unsigned int freq;
    int ret;

    LOG_DEBUG("### yaoxi_init\n");

    char* customer_id = (char*)getenv("PROTOCOL_ID");
    if (NULL != customer_id) {
        if (0 == strcmp(customer_id, "yaoxi")){
            si4730_fd = open("/dev/fm_dev", O_RDWR);
            if(si4730_fd < 0)
            {
                printf("yaoxi init failed\n");
                return AC_Undefine;
            }
        }
        else {
            return AC_Undefine;
        }
    }
    else{
        return AC_Undefine;
    }

    pthread_mutex_init(&yaoxi_mutex,NULL);

    //
    if (0 != pthread_create(&yaoxi_pthread, NULL, (void*(*)(void*))yaoxi_pthread_handler, NULL)) {
        LOG_DEBUG("pthread_create yaoxi_pthread error\n");
        goto err_yaoxi_pthread_handler;
    }

    //default select FM
    si4730_band_mode_select(SI4730_MODE_FM);

    //read station saved in the flash
    ret = read_station_info_from_flash();
    if(ret < 0)
    {
        //no station saved in flash, using default value
    }

    //set radio default information
    set_radio_info(ARB_FM1);
    freq = yaoxi_store_station[radio_info.current_band][0];
    if(!is_freq_valid(freq))
    {
        freq = radio_info.min_freq;
    }
    ret = si4730_paly(freq);
    if(ret != SI4730_OK)
    {
     si4730_mute(SI4730_MUTE_OFF);
    }


err_yaoxi_pthread_handler:

    return AC_YAOXI;
}

int yaoxi_uninit(void)
{
    LOG_DEBUG("yaoxi_uninit\n");

    close(si4730_fd);
    pthread_join(yaoxi_pthread, NULL);

    pthread_mutex_destroy(&yaoxi_mutex);

    return 0;
}

struct CustomerProtocol yaoxi = {
    .customer = AC_YAOXI,
    .fd = {-1, -1, -1},
    .init = yaoxi_init,
    .read = yaoxi_read,
    .write = yaoxi_write,
    .uninit = yaoxi_uninit,
};


