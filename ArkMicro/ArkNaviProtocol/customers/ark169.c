#include "ark169.h"
#include <linux/input.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#if 0
typedef enum _MCU_SEND_UPDATE_INFO
{
    UPDATE_START,
    UPDATE_FILE_LEN,
    //UPDATE_TRANSFER_TRY,
    UPDATE_TRANSFER,
    UPDATE_PROGRESS,
    //CHECK_START,
    //CHECK_TRANSFER,
    UPDATE_SUCCESS,
    UPDATE_FAILED,
} MCU_SEND_UPDATE_INFO;

typedef enum _ARM_SEND_UPDATE_INFO
{
    CMD_UPDATE_START,
    CMD_TRANSFER_END,

} ARM_SEND_UPDATE_INFO;
#endif

static int mcu_serial_port_fd = -1;
static int input_fd = -1;
static int reversing_fd = -1;
static int ill_fd = -1;
static int brake_fd =-1;
static struct timeval old_tv = {0,0};
static int uart_rx_frame = 0;
static int check_rx_frame = 0;
static int uart_rx_frame_len = 0;
static int uart_rx_step = 0;
static int uart_rx_index = 0;
static unsigned char uart_rx_buffer[RX_FRAME_NUMBER][RX_BUFFER_SIZE];
static unsigned char uart_rx_checksum = 0;

static sem_t frame_sem;
static pthread_t frame_handler_pthread = 0;
static int frame_thread_exit_flag = 1;

#define KEY_INPUT_DEVICE_PATH   "/dev/input/event1"

#define TOUCH_KEY_RELEASE		0
#define TOUCH_KEY_PRESS			1
#define TOUCH_KEY_LEAVE			(2 | TOUCH_KEY_PRESS)
#define TOUCH_KEY_HOVER			(4 | TOUCH_KEY_PRESS)

static inline int set_heartbeat_tick(int sec)
{
    unsigned char tick = sec;
    return notify_control_event(ACLT_Heartbeat, &tick, 1);
}

//static int source_switch(enum ArkType type)
//{
//    static enum ArkType lastType = AT_Undefine;
//    if (lastType != type) {
//        switch (type) {
//        case AT_Radio: {
//            lastType = type;
//            struct ArkControl control;
//            memset((void*)&control, 0, sizeof(struct ArkControl));
//            struct ArkProtocol protocol;
//            memset((void*)&protocol, 0, sizeof(struct ArkProtocol));
//            protocol.type = AT_Control;
//            protocol.direction = AD_Setter;
//            protocol.data = &control;
//            control.type = ACLT_RequestSourceSwitch;
//            unsigned char data[1] = {0x04};
//            control.data = data;
//            control.length = 1;
//            ark169_mcu_write(&protocol);
//            break;
//        }
//        case AT_Bluetooth: {
//            lastType = type;
//            struct ArkControl control;
//            memset((void*)&control, 0, sizeof(struct ArkControl));
//            struct ArkProtocol protocol;
//            memset((void*)&protocol, 0, sizeof(struct ArkProtocol));
//            protocol.type = AT_Control;
//            protocol.direction = AD_Setter;
//            protocol.data = &control;
//            control.type = ACLT_RequestSourceSwitch;
//            unsigned char data[1] = {0x0B};
//            control.data = data;
//            control.length = 1;
//            ark169_mcu_write(&protocol);
//            break;
//        }
//        default: {
//            break;
//        }
//        }
//    }
//}

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

static struct CarDeviceState device_state = {
    .reversing = -1,
    .illlight = -1,
    .brake = -1,
};
//02 81 b4 07 05 b7 5a 0a 05 a4 10 61
static int get_radio_info(unsigned char *data, unsigned char len)
{
    //        LOG_FFLDEBUG("\n");
    if (data[0] == 0) {
        if (len != 10) return -1;
        int min_freq = (data[1] << 8) | data[2];
        if (min_freq != radio_info.min_freq) {
            radio_info.min_freq = min_freq;
            notify_radio_event(ART_GetMinFreq, data + 1, 2);
        }
        int max_freq = (data[3] << 8) | data[4];
        if (max_freq != radio_info.max_freq) {
            radio_info.max_freq = max_freq;
            notify_radio_event(ART_GetMaxFreq, data + 3, 2);
        }
        int granularity = data[5];
        if (granularity != radio_info.granularity) {
            radio_info.granularity = granularity;
            notify_radio_event(ART_GetGranularity, data + 5, 1);
        }
        int siganl_intensity = data[6];
        if (siganl_intensity != radio_info.signal_intensity) {
            radio_info.signal_intensity = siganl_intensity;
            notify_radio_event(ART_GetSignalIntensity, data + 6, 1);
        }
        union Bb8 radio_status;
        radio_status.Byte = data[7];
        enum ArkRadioStatus status;
        if (radio_status.Bit.b0) status = ARS_SEEK_UP;
        else if (radio_status.Bit.b1) status = ARS_SEEK_DOWN;
        else if (radio_status.Bit.b2) status = ARS_BROWSE_PRESTORE;
        else if (radio_status.Bit.b3) status = ARS_AUTOSTORE;
        else status = ARS_IDLE;
        if (status != radio_info.status) {
            radio_info.status = status;
            unsigned char st = (unsigned char)status;
            notify_radio_event(ART_GetStatus, &st, 1);
        }
        int curfreq_stereo = radio_status.Bit.b4;
        LOG_DEBUG("radio_status.Bit.b4=%d\n", radio_status.Bit.b4);
        if (curfreq_stereo != radio_info.curfreq_stereo) {
            radio_info.curfreq_stereo = curfreq_stereo;
            notify_radio_event(radio_info.curfreq_stereo ? ART_GetCurfreqStereo : ART_GetCurfreqMono, NULL, 0);
        }
        int use_stereo = radio_status.Bit.b5;
        LOG_DEBUG("radio_status.Bit.b5=%d\n", radio_status.Bit.b5);
        if (use_stereo != radio_info.use_stereo) {
            radio_info.use_stereo = use_stereo;
            notify_radio_event(radio_info.use_stereo ? ART_GetUseStereo : ART_GetUseMono, NULL, 0);
        }
        int scanmode = radio_status.Bit.b6;
        if (scanmode != radio_info.scanmode) {
            radio_info.scanmode = scanmode;
            notify_radio_event(scanmode ? ART_GetAutoScan : ART_GetManualScan, NULL, 0);
        }
        int loc_dx = radio_status.Bit.b7;
        LOG_DEBUG("radio_status.Bit.b7=%d\n", radio_status.Bit.b7);
        if (loc_dx != radio_info.loc_dx) {
            radio_info.loc_dx = loc_dx;
            notify_radio_event(loc_dx ?ART_GetLOC: ART_GetDX, NULL, 0);
        }
    } else if (data[0] == 1) {
        LOG_FFLDEBUG("\n");
        if (len != 17) return -1;
        LOG_FFLDEBUG("\n");
        int i, preset_list[6], diff = 0;
        for (i = 0; i < RADIO_PRESET_NUM; i++) {
            preset_list[i] = (data[5 + 2*i] << 8) | data[5 + 2*i + 1];
            if (preset_list[i] != radio_info.preset_list[i]) {
                radio_info.preset_list[i] = preset_list[i];
                diff++;
                LOG_FFLDEBUG("\n");
            }
        }
        int current_band = data[1];
        if (current_band > ARB_Undefine && current_band < ARB_LAST && current_band != radio_info.current_band) {
            radio_info.current_band = current_band;
            notify_radio_event(ART_GetBand, data + 1, 1);
            //            notify_radio_event(radio_info.curfreq_stereo ? ART_GetCurfreqStereo : ART_GetCurfreqMono, NULL, 0);
            //            notify_radio_event(radio_info.use_stereo ? ART_GetUseStereo : ART_GetUseMono, NULL, 0);
        }
        if (diff > 0) notify_radio_event(ART_GetPresetList, data + 5, 12);
        int current_preset = data[2];
        int current_freq = (data[3] << 8) | data[4];
        if (current_preset >=0 && current_preset <= RADIO_PRESET_NUM && current_preset != radio_info.current_preset) {
            radio_info.current_preset = current_preset;
            notify_radio_event(ART_GetPreset, data + 2, 1);
        }
        //        if (current_freq != radio_info.current_freq) {
        radio_info.current_freq = current_freq;
        notify_radio_event(ART_GetFreq, data + 3, 2);
        //        }
    } else if (0x05 == data[0]) {
        if (07 == len) {
            LOG_DEBUG("ART_KEY_Smeter\n");
            notify_radio_event(ART_KEY_Smeter, data + 1, len - 1);
        }
    }
    return 0;
}

static int get_device_state(unsigned char *data, unsigned char len)
{
    if (len != 9) return -1;

    //    int reversing = data[1] & 1 ? 1 : 0;
    //    if (reversing != device_state.reversing) {
    //        device_state.reversing = reversing;
    ////        enum ArkCarStatus status = reversing ? ACS_ReversingOn : ACS_ReversingOff;
    ////        LOG_DEBUG("reversing = status = %d\n", status);
    //        //        notify_car_event(ACT_Reversing, status, NULL, 0);
    //    }

    int illlight = data[1] & 2 ? 1 : 0;
    if (illlight != device_state.illlight) {
        device_state.illlight = illlight;
        enum ArkCarStatus status = illlight ? ACS_ILLLightOn : ACS_ILLLightOff;
        if (status == ACS_ILLLightOn)
        {
            export_gpio(34);
            direction_gpio(34, "out");
            set_value_gpio(34, 1);
        }else{
            export_gpio(34);
            direction_gpio(34, "out");
            set_value_gpio(34, 0);
        }

        notify_car_event(ACT_ILLLight, status, NULL, 0);
    }

    int brake = data[1] & 4 ? 0 : 1;
    if (brake != device_state.brake) {
        device_state.brake = brake;
        enum ArkCarStatus status = brake ? ACS_BrakeOn : ACS_BrakeOff;
        notify_car_event(ACT_Brake, status, NULL, 0);
    }

    return 0;
}

struct BtInfo bt_info = {
    .status = ABS_Undefine,
};

static enum ArkBtSyncType bt_sync_type = ABST_Undefine;
static enum ArkBtSyncType bt_sync_cmd = ABST_Undefine;

static int bt_cmd_handler(unsigned char *data, unsigned char len)
{
    if (len < 4 || (data[len - 2] != 0xd && data[len - 1] != 0xa)) return -1;

    enum ArkBtStatus status = bt_info.status;
    //    int iter = 0;
    //    for (iter = 0; iter < len; ++iter) {
    //        LOG_DEBUG("iter = %d, %x\n", iter, data[iter]);
    //    }
    //    LOG_DEBUG("--------\n");
    switch (data[0]) {
    case 'C': {
        if ('P' == data[1]) {
            enum ArkBtType  type = ABT_VoiceBluetooth;
            if (data[2] == 0x01) {
                type = ABT_VoicePhone;
            }
            notify_bt_event(type, NULL, 0);
        }
    }
    case 'I':
        switch (data[1]) {
        case 'F': {
            LOG_FFLDEBUG("bt_info.status = %d\n", bt_info.status);
            switch (bt_info.status) {
            case ABS_TALKING:
            case ABS_INCOMING:
            case ABS_OUTGOING: {
                notify_bt_event(ABT_Hangup, NULL, 0);
                status = ABS_CONNECTED;
                break;
            }
            default: {
                break;
            }
            }
            break;
        }
        case 'S':
        case 'A':
            LOG_FFLDEBUG("\n");
            status = ABS_IDLE;
            break;
            //        case 'V':
            //            status = ABS_CONNECTING;
            //            break;
        case 'B':
            status = ABS_CONNECTED;
            break;
        case 'C':
            status = ABS_OUTGOING;
            if (len > 4) {
                notify_bt_event(ABT_GetOutgoingNumber, data + 2, len - 4);
            }
            break;
        case 'D':
            status = ABS_INCOMING;
            if (len > 4) {
                notify_bt_event(ABT_GetIncomingNumber, data + 2, len - 4);
            }
            break;
        case 'R':
            if (len > 4) {
                notify_bt_event(ABT_GetPhoneNumberNow, data + 2, len - 4);
            }
            break;
        case 'G':
            status = ABS_TALKING;
            break;
        case 'I':
            LOG_FFLDEBUG("\n");
            notify_bt_event(ABT_PairedMode, NULL, 0);
            break;
        case 'J':
            LOG_FFLDEBUG("\n");
            notify_bt_event(ABT_CancelPairedMode, NULL, 0);
            break;
        }
        LOG_FFLDEBUG("bt_info.status = %d\n", bt_info.status);
        if (status != bt_info.status) {
            bt_info.status = status;
            LOG_FFLDEBUG("bt_info.status = %d\n", bt_info.status);
            unsigned char st = (unsigned char)status;
            notify_bt_event(ABT_GetStatus, &st, 1);
        }
        break;
    case 'M':
        switch (data[1]) {
        case 'P': {
            LOG_DEBUG("---len=%d\n", len);
            notify_bt_event(ABT_MusicName, data + 2, len - 4);
            break;
        }
        case 'M':
            if (len > 4) {
                notify_bt_event(ABT_GetLocalName, data + 2, len - 4);
            }
            break;
        case 'N':
            if (len > 4) {
                notify_bt_event(ABT_GetPinCode, data + 2, len - 4);
            }
            break;
        case 'Z':
            if (len > 4) {
                notify_bt_event(ABT_GetLocalAddr, data + 2, len - 4);
            }
            break;
        case 'A' :
            if (ABS_CONNECTED != bt_info.status) {
                bt_info.status = ABS_CONNECTED;
                unsigned char st = (unsigned char)bt_info.status;
                LOG_FFLDEBUG("bt_info.status = %d\n", bt_info.status);
                notify_bt_event(ABT_GetStatus, &st, 1);
            }
            notify_bt_event(ABT_GetA2DPConnect, NULL, 0);
            break;
        case 'B' :
            notify_bt_event(ABT_GetMusicPlaying, NULL, 0);
            break;
        case 'C' :
            notify_bt_event(ABT_GetVoiceConnect, NULL, 0);
            break;
        case 'D':
            notify_bt_event(ABT_GetVoiceDisconnect, NULL, 0);
            break;
        case 'X': {
            notify_bt_event(ABT_PairedListName, data + 2, len - 4);
            break;
        }
        case 'Y' :
            notify_bt_event(ABT_GetA2DPDisconnect, NULL, 0);
            break;
        case 'F':
            if ((len == 5)
                    && (0x0D == data[3])
                    && (0x0A == data[4])) {
                if(data[2] & 0x01) {
                    notify_bt_event(ABT_GetAutoAnswer, NULL, 0);
                } else {
                    notify_bt_event(ABT_GetNotAutoAnswer, NULL, 0);
                }
                if(data[2] & 0x02) {
                    notify_bt_event(ABT_GetAutoConnect, NULL, 0);
                } else {
                    notify_bt_event(ABT_GetNotAutoConnect, NULL, 0);
                }
            }
            break;
        case 'G': {
            if ((len == 5)
                    && (0x0D == data[3])
                    && (0x0A == data[4])) {
                if (0x01 == data[2]) {
#if 0
                    if (ABS_CONNECTING == bt_info.status) {
                        bt_info.status = ABS_IDLE;
                        unsigned char st = (unsigned char)bt_info.status;
                        LOG_FFLDEBUG("bt_info.status = %d\n", bt_info.status);
                        notify_bt_event(ABT_GetStatus, &st, 1);
                    }
#else
                    switch (bt_info.status) {
                    case ABS_OUTGOING:
                    case ABS_INCOMING:
                    case ABS_TALKING: {
                        notify_bt_event(ABT_Hangup, NULL, 0);
                    }
                    case ABS_CONNECTING:
                    case ABS_CONNECTED: {
                        LOG_FFLDEBUG("bt_info.status = %d\n", bt_info.status);
                        bt_info.status = ABS_IDLE;
                        unsigned char st = (unsigned char)bt_info.status;
                        LOG_FFLDEBUG("bt_info.status = %d\n", bt_info.status);
                        notify_bt_event(ABT_GetStatus, &st, 1);
                        break;
                    }
                    default: {
                        break;
                    }
                    }
#endif
                } else if (0x05 == data[2]) {
                    LOG_FFLDEBUG("bt_sync_type=%d\n", bt_sync_type);
                    if (ABS_CONNECTED != bt_info.status) {
                        switch (bt_info.status) {
                        case ABS_OUTGOING:
                        case ABS_INCOMING:
                        case ABS_TALKING: {
                            notify_bt_event(ABT_Hangup, NULL, 0);
                        }
                        default: {
                            break;
                        }
                        }
                        bt_info.status = ABS_CONNECTED;
                        unsigned char st = (unsigned char)bt_info.status;
                        LOG_FFLDEBUG("bt_info.status = %d\n", bt_info.status);
                        notify_bt_event(ABT_GetStatus, &st, 1);
                    }
                }
            }
            //            //蓝牙状态，用bt_get_status函数来获取
            //            enum {
            //                BT_STA_OFF = 0x00,                                 //蓝牙模块已关闭
            //                BT_STA_IDLE = 0x01,                                //未连接
            //                BT_STA_SLEEP = 0x02,                               //低功耗

            //                BT_STA_DISCONNECTING = 0x03,                       //断开中
            //                BT_STA_CONNECTING = 0x04,                          //连接中

            //                BT_STA_CONNECTED = 0x05,                           //已连接
            //                BT_STA_PLAYING = 0x06,                             //播放
            //                BT_STA_INCOMING = 0x07,                            //来电响铃
            //                BT_STA_OUTGOING = 0x08,                            //正在呼出
            //                BT_STA_PHONE = 0x09,                               //通话中
            //            };
            break;
        }
        }
        break;
    case 'A':
        switch (data[1]) {
        case 'D':
            if (len > 4) {
                notify_bt_event(ABT_GetRemoteAddr, data + 2, len - 4);
            }
            break;
        }
        break;
    case 'S':
        switch (data[1]) {
        case 'A':
            if (len > 4) {
                notify_bt_event(ABT_GetRemoteName, data + 2, len - 4);
                LOG_FFLDEBUG("bt_info.status = %d, len = %d\n", bt_info.status, len - 4);
                if (ABS_CONNECTED != bt_info.status) {
                    bt_info.status = ABS_CONNECTING;
                    LOG_FFLDEBUG("bt_info.status = %d\n", bt_info.status);
                    unsigned char st = bt_info.status;
                    notify_bt_event(ABT_GetStatus, &st, 1);
                }
            }
            break;
        }
        break;
    case 'P':
        switch (data[1]) {
        case 'B':
            if (len > 4) {
                notify_bt_event(ABT_GetPhoneBookEntry, data + 2, len - 4);
            }
            break;
        case 'C':
            if (5 == len) {
                int i = 0;
                for (i = 0; i < len; ++i) {
                    LOG_DEBUG("i = %i, %x\n", i, data[i]);
                }
                LOG_FFLDEBUG("bt_sync_type=%d\n", bt_sync_type);
                if (0x01 == data[2]) {//cancel
                    bt_sync_type = ABST_Undefine;
                    notify_bt_event(ABT_SyncPhoneBookEnd, NULL, 0);
                } else if (0x02 == data[2]) {
                    bt_sync_type = ABST_Undefine;
                    notify_bt_event(ABT_SyncReceiveRecordEnd, NULL, 0);
                } else if (0x04 == data[2]) {
                    bt_sync_type = ABST_Undefine;
                    notify_bt_event(ABT_SyncDialedRecordEnd, NULL, 0);
                } else if (0x08 == data[2]) {
                    bt_sync_type = ABST_Undefine;
                    notify_bt_event(ABT_SyncMissRecordEnd, NULL, 0);
                } else if ((0xff == data[2])
                           || (0xfe == data[2])) {
                    switch (bt_sync_type) {
                    case ABST_PhoneBook: {
                        notify_bt_event(ABT_SyncPhoneBookCancelling, NULL, 0);
                        break;
                    }
                    case ABST_ReceiveRecord: {
                        notify_bt_event(ABT_SyncReceiveRecordCancelling, NULL, 0);
                        break;
                    }
                    case ABST_DialedRecord: {
                        notify_bt_event(ABT_SyncDialedRecordCancelling, NULL, 0);
                        break;
                    }
                    case ABST_MissRecord: {
                        notify_bt_event(ABT_SyncMissRecordCancelling, NULL, 0);
                        break;
                    }
                    default: {
                        break;
                    }
                    }
                    bt_sync_type = ABST_Undefine;
                    //第一次收到0xff,先发送stop,再次收到,把蓝牙关掉,再打开
                    //notify_bt_event(ABT_SyncMissRecordEnd, NULL, 0);
                } else if (0xef == data[2]) {//request
                    switch (bt_sync_type) {
                    case ABST_PhoneBook:
                    case ABST_ReceiveRecord:
                    case ABST_DialedRecord:
                    case ABST_MissRecord: {
                        notify_bt_event(bt_sync_type, NULL, 0);
                        break;
                    }
                    default: {
                        break;
                    }
                    }
                }
            }
            break;
        case 'D':
            if (len > 4) {
                if (bt_sync_type == ABST_ReceiveRecord)
                    notify_bt_event(ABT_GetReceiveRecordEntry, data + 2, len - 4);
                else if (bt_sync_type == ABST_DialedRecord)
                    notify_bt_event(ABT_GetDialedRecordEntry, data + 2, len - 4);
                else if (bt_sync_type == ABST_MissRecord)
                    notify_bt_event(ABT_GetMissRecordEntry, data + 2, len - 4);
            }
            break;
        }
        break;
    default:
        break;
    }


    return 0;
}

#define SWC_KEY_NUM         9
unsigned char swc_table[SWC_KEY_NUM + 1] = {0};

static int get_swc_table(unsigned char *data, unsigned char len)
{
    if (len != 18) return -1;

    unsigned char keyidx;
    for (keyidx = 1; keyidx <= SWC_KEY_NUM; keyidx++) {
        if (data[2*(keyidx-1)] != swc_table[keyidx]) {
            swc_table[keyidx] = data[2*(keyidx-1)];
            if (swc_table[keyidx]) {
                notify_control_event(ACLT_SWCStudyOK, &keyidx, 1);
            } else {
                notify_control_event(ACLT_SWCStudyCancel, &keyidx, 1);
            }
        }
    }

    return 0;
}

#if 0
1、发现并读取升级文件相关信息后,向MCU发送开始升级命令						CMD_UPDATE_START。
2、接收到UPDATE_START命令后，回复SUCCESS。
3、接收到UPDATE_FILE_LEN命令后，回复升级文件大小。
4、接收到UPDATE_TRANSFER命令后，根据偏移位置读取并依次发送16个数据包，发送CMD_TRANSFER_END命令。如此循环直到所有数据升级完毕。
5、接收到MCU升级结果命令后，根据相关具体情况进行相应的处理。
注：在升级各阶段会接收到当前升级进度通知，此时需要更新升级进度条及其他升级信息。
0x00：开始升级
0x01：文件信息
0x02：数据传输
0x03：升级进度
0x04：升级成功
0x05：升级失败
#endif

static int mcu_update_handler(const unsigned char* data, const unsigned char len)
{
    LOG_FFLDEBUG("len = %d\n", len);
    int j = 0;
    for (j = 0; j < len; ++j) {
        LOG_FFLDEBUG("%d=%d\n", j, data[j]);
    }
    unsigned char buffer[256 + 32];
    unsigned char checksum = 0;
    unsigned short int length = 0;
    j = 0;
    buffer[j++] = 0x02;
    switch (data[0]) {
    case 0x00: {//开始升级
        if (1 == len) {
            LOG_FFLDEBUG("\n");
            buffer[j++] = ARM_ACKDATA_CMD;
            buffer[j++] = Mcu_Send_Update_Status;
            buffer[j++] = 0x00;
            buffer[j++] = data[0];
            buffer[j++] = ERR_SUCCESS;
            buffer[0x03] = j - 4;
            length = j + 1;
        }
        break;
    }
    case 0x01: {//文件信息  文件数据大小/128       examples:172032 byte / 32 = 5376
        if (1 == len) {
            LOG_FFLDEBUG("\n");
#if 0
            buffer[j++] = ARM_ACKDATA_CMD;
            buffer[j++] = Mcu_Send_Update_Status;
            buffer[j++] = 0x00;
            buffer[j++] = ERR_SUCCESS;
            buffer[j++] = 0x01;
            unsigned short int file_length = 172032 / 128;
            unsigned char high = (file_length & 0xff00) >> 8;
            unsigned char low = (file_length & 0x00ff);
            buffer[j++] = high;
            buffer[j++] = low;
            buffer[0x03] = j - 4;
            length = j + 1;
#endif
            notify_car_event(ACT_MCUUpdate, ACS_MCUUpdateFileLen, NULL, 0);
        }
        break;
    }
    case 0x02: {//数据传输
        if (4 == len) {
#if 0
            //            int count;
            //            for (count = 0; count < data[3]; ++count) {
            //                usleep(50 * 1000);
            //            LOG_FFLDEBUG("!!%d\n", count);
            j = 1;
            buffer[j++] = ARM_ACKDATA_CMD;
            buffer[j++] = Mcu_Send_Update_Status;
            buffer[j++] = 0x00;
            buffer[j++] = ERR_SUCCESS;
            buffer[j++] = 0x02;
            buffer[j++] = 0x01;
            //            buffer[j++] = /*count + 1*/data[1];
            //            buffer[j++] = /*count + 1*/data[2];
            int l = 0;
            for (l = 0; l < 128/*256*/; ++l) {
                buffer[j++] = l;
            }
            buffer[0x03] = (j - 4);
            length = j + 1;

            if (length > 0) {
                int i;
                for (i = 1; i < length - 1; i++) {
                    checksum ^= buffer[i];
                }
                buffer[length - 1] = checksum;
                LOG_DEBUG("ARM to MCU : ");
                for (i = 0; i < length; i++)
                    LOG_DEBUG("%.2x ", buffer[i]);
                LOG_DEBUG("\n");
                //                for (i = 0; i < length; i++) {
                //                    if ((buffer[i] >= ' ')
                //                            && (buffer[i] < 'z')) {
                //                        LOG_DEBUG("%c ", buffer[i]);
                //                    }
                //                }
                LOG_DEBUG("\n");
                int ret = write(mcu_serial_port_fd, buffer, length);
                fsync(mcu_serial_port_fd);
                LOG_DEBUG("ret = length = %d, ret%d\n", length, ret);
                return 0;
            }
            //            }
#else
            unsigned char callback_data[3] = {data[1], data[2], data[3]};
            notify_car_event(ACT_MCUUpdate, ACS_MCUUpdateTransfer, callback_data, 3);
#endif
        }
        //        j = 1;
        //        LOG_FFLDEBUG("\n");
        //        buffer[j++] = ARM_SENDDATA_CMD;
        //        buffer[j++] = Arm_Send_McuUpdateCmd;
        //        buffer[j++] = 0x01;
        //        buffer[j++] = 0x01;
        //        buffer[0x03] = j - 4;
        //        length = j + 1;f
        break;
    }
    case 0x03: {//升级进度
        if (2 == len) {
            LOG_FFLDEBUG("Upgrade Status:%d%\n", data[1]);
            char percent[1] = {data[1]};
            notify_car_event(ACT_MCUUpdate, ACS_MCUUpdatePercent, percent, 1);
        }
        break;
    }
    case 0x04: {//升级成功
        break;
    }
    case 0x05: {//升级失败
        //        LOG_FFLDEBUG("\n");
        //        buffer[j++] = ARM_ACKDATA_CMD;
        //        buffer[j++] = Arm_Send_McuUpdateCmd;
        //        buffer[j++] = 0x02;
        //        buffer[j++] = ERR_SUCCESS;
        //        length = j + 1;
        break;
    }
    default: {
        break;
    }
    }
    if (length > 0) {
        int i;
        for (i = 1; i < length - 1; i++) {
            checksum ^= buffer[i];
        }
        buffer[length - 1] = checksum;
        LOG_DEBUG("ARM to MCU : ");
        for (i = 0; i < length; i++)
            LOG_DEBUG("%.2x ", buffer[i]);
        LOG_DEBUG("\n");
        for (i = 0; i < length; i++) {
            if ((buffer[i] >= ' ')
                    && (buffer[i] < 'z')) {
                LOG_DEBUG("%c ", buffer[i]);
            }
        }
        LOG_DEBUG("\n");
        int ret = write(mcu_serial_port_fd, buffer, length);
        fsync(mcu_serial_port_fd);
        LOG_DEBUG("ret = length = %d, ret%d\n", length, ret);
    }
    return 0;
}

static int key_event_handler(unsigned char *data, unsigned char len)
{
    if (len != 6) return -1;

    enum ArkKeyType type;
    enum ArkKeyStatus status;

    switch (data[0]) {
    case 1:
        status = AKS_Press;
        break;
    case 2:
        status = AKS_Release;
        break;
    default:
        status = AKS_Active;
        break;
    }

    switch (data[1]) {
    case 0x1:
        type = AKT_CommonPower;
        break;
    case 0x2:
        type = AKT_CommonVolumeMute;
        break;
    case 0x4:
        type = AKT_CommonVolumeIncrement;
        break;
    case 0x5:
        type = AKT_CommonVolumeDecrement;
        break;
    case 0x6:
        type = AKT_BtPickUp;
        break;
    case 0x7:
        type = AKT_BtHangUp;
        break;
    case 0x8:
        type = AKT_CommonMode;
        break;
    case 0x13:
    case 0x72:
        type = AKT_CommonNext;
        break;
    case 0x14:
    case 0x73:
        type = AKT_CommonPrevious;
        break;
    case 0x40:
    case 0x41:
    case 0x42:
        type = AKT_SwitchToMedia;
        break;
    case 0x43:
        type = AKT_SwitchToNavi;
        break;
    case 0x44:
        type = AKT_SwitchToRadio;
        break;
    case 0x45:
        type = AKT_SwitchToDVD;
        break;
    case 0x46:
        type = AKT_SwitchToBt;
        break;
    case 0x47:
        type = AKT_SwitchToTouchCal;
        break;
    default:
        LOG_DEBUG("unknow key %d.\n", data[1]);
        return -1;
    }

    notify_key_event(type, status);

    return 0;
}

static void* uart_frame_handler(void* arg)
{
    //    LOG_DEBUG("%d\n", frame_thread_exit_flag);
    while (0 == frame_thread_exit_flag) {
        sem_wait(&frame_sem);
        //        LOG_DEBUG("sem_wait1=check_rx_frame=%d, uart_rx_frame=%d\n", check_rx_frame, uart_rx_frame);
        if (frame_thread_exit_flag) break;
        while (check_rx_frame == uart_rx_frame) {
            //            LOG_FFLDEBUG("No frame, wait...\n");
            usleep(100000);
        }
        unsigned char cmd_type = uart_rx_buffer[check_rx_frame][1];
        unsigned char cmd_code = uart_rx_buffer[check_rx_frame][2];
        int len = uart_rx_buffer[check_rx_frame][3] + 5;
        unsigned char *data = &uart_rx_buffer[check_rx_frame][4];
        LOG_DEBUG("MCU to ARM(%d): ", len);
        int i;
        for (i = 0; i < len; i++) {
            LOG_DEBUG("%.2x ", uart_rx_buffer[check_rx_frame][i]);
        }
        LOG_DEBUG("\n");
        for (i = 0; i < len; i++) {
            if ((uart_rx_buffer[check_rx_frame][i] >= ' ')
                    && (uart_rx_buffer[check_rx_frame][i] <= 'z')) {
                LOG_DEBUG("%c ", uart_rx_buffer[check_rx_frame][i]);
            }
        }
        switch (cmd_code) {
        case Mcu_Send_HandShakeCmd:
            notify_control_event(ACLT_Handshake, NULL, 0);
            break;
        case Mcu_Req_RadioInfoCmd:
            if (data[0] == ERR_SUCCESS)
                get_radio_info(data + 1, len - 6);
            else
                LOG_DEBUG("Mcu_Req_RadioInfoCmd error %d.\n", data[0]);
            break;
        case Mcu_Send_RadioInfoCmd:
            get_radio_info(data, len - 5);
            break;
        case Mcu_Req_McuDeviceStateCmd:
            if (data[0] == ERR_SUCCESS)
                get_device_state(data + 1, len - 6);
            else
                LOG_DEBUG("Mcu_Req_McuDeviceStateCmd error %d.\n", data[0]);
            break;
        case Mcu_Send_McuDevieStateCmd:
            get_device_state(data, len - 5);
            break;
        case CMD_ID_SEND_BT_CMD:
            bt_cmd_handler(data, len - 5);
            break;
        case Mcu_Req_SwcTabCmd:
            if (data[0] == ERR_SUCCESS)
                get_swc_table(data + 1, len - 6);
            else
                LOG_DEBUG("Mcu_Req_SwcTabCmd error %d.\n", data[0]);
            break;
        case Mcu_Send_SwcTabCmd:
            get_swc_table(data + 1, len - 5);
            break;
        case Mcu_Send_KeyCmd:
            key_event_handler(data, len - 5);
            break;
        case Mcu_Send_SysInfoCmd: {
            int i = 0;
            unsigned char tmp[7] = {0};
            for (i = 1; i < 8; ++i) {
                tmp[i - 1] = (data[i] - (data[i] >> 4) * 6);
                //                LOG_FFLDEBUG("decimal=%d\n", tmp[i - 1]);
            }
            //            LOG_FFLDEBUG("i = %d\n", i);
            if (8 == i) {
                struct ArkCar car;
                memset(&car, 0, sizeof(struct ArkCar));
                car.type = ACT_Time;
                car.status = ACS_Undefine;
                car.data = (void*)tmp;
                car.length = 7;
                struct ArkProtocol protocol;
                protocol.type = AT_Car;
                protocol.direction = AD_Notify;
                protocol.data = &car;
                callback_notify(&protocol);
            }
            break;
        }
        case Mcu_Send_Update_Status: {
            mcu_update_handler(data, len - 5);
            break;
        }
        case Mcu_Send_ACC_STATUS: {
            if (6 == len) {
                struct ArkCar car;
                memset(&car, 0, sizeof(struct ArkCar));
                car.type = ACT_ACC;
                car.status = ACS_ACCOff;
                car.data = NULL;
                car.length = 0;
                struct ArkProtocol protocol;
                protocol.type = AT_Car;
                protocol.direction = AD_Notify;
                protocol.data = &car;
                callback_notify(&protocol);
            }
            break;
        }
        case Arm_Req_McuVerCmd: {
            LOG_FFLDEBUG("len=%x\n", len);
            if ((26 == len)
                    && (ERR_SUCCESS == data[0])) {
                struct ArkCar car;
                memset(&car, 0, sizeof(struct ArkCar));
                car.type = ACT_Version;
                car.status = ACS_Undefine;
                car.data = (void*)(&data[1]);
                car.length = len - 6;
                struct ArkProtocol protocol;
                protocol.type = AT_Car;
                protocol.direction = AD_Notify;
                protocol.data = &car;
                callback_notify(&protocol);
            }
            break;
        }
        default:
            LOG_DEBUG("Unimplemented CMD %.2x handler.\n", cmd_code);
            break;
        }

        check_rx_frame = (check_rx_frame + 1) % RX_FRAME_NUMBER;
        //                LOG_DEBUG("sem_wait2=check_rx_frame=%d, uart_rx_frame=%d\n", check_rx_frame, uart_rx_frame);
    }
    return NULL;
}

int ark169_mcu_read(const int fd)
{
    if (-1 != fd && mcu_serial_port_fd == fd) {
        static unsigned char buffer;
        static int ret = 0;
        for (;;) {
            ret = read(fd, &buffer, 1);
            if (1 == ret) {
                //                LOG_DEBUG("buffer=0x%2x\n", buffer);
                switch (uart_rx_step) {
                case 0: //header
                    if (buffer == ARM_HEADCMD) {
                        uart_rx_buffer[uart_rx_frame][uart_rx_index++] = buffer;
                        uart_rx_step++;
                    }
                    break;
                case 1: //cmd type
                    if (buffer == ARM_SENDDATA_CMD || buffer == MCU_SENDDATA_CMD) {
                        uart_rx_buffer[uart_rx_frame][uart_rx_index++] = buffer;
                        uart_rx_step++;
                        uart_rx_checksum = buffer;

                    } else {
                        uart_rx_step = 0;
                        uart_rx_index = 0;
                    }
                    break;
                case 2: //cmd code
                    uart_rx_buffer[uart_rx_frame][uart_rx_index++] = buffer;
                    uart_rx_step++;
                    uart_rx_checksum ^= buffer;
                    break;
                case 3: //data len
                    uart_rx_buffer[uart_rx_frame][uart_rx_index++] = buffer;
                    uart_rx_step++;
                    uart_rx_checksum ^= buffer;
                    uart_rx_frame_len = buffer;
                    if (buffer == 0) uart_rx_step++;
                    else if (buffer > RX_BUFFER_SIZE - 5) {
                        uart_rx_step = 0;
                        uart_rx_index = 0;
                    }
                    break;
                case 4: //data
                    if (uart_rx_index < RX_BUFFER_SIZE && uart_rx_index - 4 < uart_rx_frame_len) {
                        uart_rx_buffer[uart_rx_frame][uart_rx_index++] = buffer;
                        uart_rx_checksum ^= buffer;
                        if (uart_rx_index - 4 == uart_rx_frame_len)
                            uart_rx_step++;
                    } else {
                        uart_rx_step = 0;
                        uart_rx_index = 0;
                    }
                    break;
                case 5: //checksum
                    if (buffer == uart_rx_checksum) {
                        uart_rx_buffer[uart_rx_frame][uart_rx_index] = buffer;
                        //                        LOG_DEBUG("sem_post1=uart_rx_frame%d, check_rx_frame=%d\n", uart_rx_frame, check_rx_frame);
                        int index;
                        while ((index = (uart_rx_frame + 1) % RX_FRAME_NUMBER) == check_rx_frame) {
                            //                            LOG_DEBUG("Uart rx buffer full, wait...\n");
                            usleep(100000);
                        }
                        uart_rx_frame = index;
                        //                        LOG_DEBUG("sem_post2=uart_rx_frame%d, check_rx_frame=%d\n", uart_rx_frame, check_rx_frame);
                        sem_post(&frame_sem);
                    }
                    uart_rx_step = 0;
                    uart_rx_index = 0;
                    break;
                default:
                    break;
                }
            } else {
                break;
            }
        }
    } else if (-1 != fd && input_fd == fd) {
        struct input_event t;
        if (read(input_fd, &t, sizeof(t)) == sizeof(t)) {
            if (t.type == EV_KEY) {
                enum ArkKeyType type;
                enum ArkKeyStatus status;
                switch (t.code) {
                case KEY_POWER:
                    type = AKT_CommonPower;
                case KEY_MUTE:
                    type = AKT_CommonVolumeMute;
                    break;
                case KEY_HOME:
                    type = AKT_CommonHome;
                    break;
                case KEY_BACK:
                    type = AKT_CommonReturn;
                    break;
                case KEY_VOLUMEUP:
                    type = AKT_CommonVolumeIncrement;
                    break;
                case KEY_VOLUMEDOWN:
                    type = AKT_CommonVolumeDecrement;
                    break;
                }
                if (t.value == TOUCH_KEY_PRESS)
                    status = AKS_Press;
                else if (t.value == TOUCH_KEY_RELEASE)
                    status = AKS_Release;
                else if (t.value == TOUCH_KEY_LEAVE)
                    status = AKS_Leave;
                else if (t.value == TOUCH_KEY_HOVER)
                    status = AKS_Hover;
                notify_key_event(type, status);
            }
        } else {
            LOG_FFLDEBUG("\n");
        }
    } else if ((-1 != fd)
               && (fd == reversing_fd)) {
        unsigned char data;
        //        LOG_DEBUG("read(reversing_fd in\n");
        if (1 == read(reversing_fd, &data, 1)) {
            enum ArkCarStatus status = (data > 0)? ACS_ReversingOn : ACS_ReversingOff;
            if (data > 0) {
                LOG_DEBUG("REVERSING ON\n");
                notify_car_event(ACT_Reversing, status, NULL, 0);
            } else {
                LOG_DEBUG("REVERSING OFF\n");
                notify_car_event(ACT_Reversing, status, NULL, 0);
            }
        } else {
            LOG_DEBUG("read reversing_fd error\n");
            perror("read");
        }
        //        LOG_DEBUG("read(reversing_fd out\n");
    } else if ((-1 != fd)
               && (fd == ill_fd)) {
        unsigned char data;
        if (1 == read(ill_fd, &data, 1)) {
            enum ArkCarStatus status = (data > 0)? ACS_ILLLightOff : ACS_ILLLightOn;
            if (data > 0) { //低检测，拉低为小灯开
                LOG_DEBUG("ILL OFF\n");
                export_gpio(34);
                direction_gpio(34, "out");
                set_value_gpio(34, 0);
                notify_car_event(ACT_ILLLight, status, NULL, 0);
            } else {
                LOG_DEBUG("ILL ON\n");
                export_gpio(34);
                direction_gpio(34, "out");
                set_value_gpio(34, 1);
                notify_car_event(ACT_ILLLight, status, NULL, 0);
            }
        } else {
            LOG_DEBUG("read ill_fd error\n");
        }
    }else if ((-1 != fd)
               && (fd == brake_fd)) {
        unsigned char data;
        if (1 == read(brake_fd, &data, 1)) {
            enum ArkCarStatus status = (data > 0)? ACS_BrakeOn : ACS_BrakeOff;
            if (data > 0) {//高检测，拉高为手刹拉起
                LOG_DEBUG("brake ON\n");
                notify_car_event(ACT_Brake, status, NULL, 0);
            } else {
                LOG_DEBUG("brake OFF\n");
                notify_car_event(ACT_Brake, status, NULL, 0);
            }
        } else {
            LOG_DEBUG("read brake_fd error\n");
        }
    }

    return 0;
}

int send_bt_protocol(enum ArkBtType type, unsigned char *buffer,
                     unsigned char *data, unsigned char length)
{
    int len = 0;
    int i;

    buffer[1] = ARM_SENDDATA_CMD;
    buffer[2] = CMD_ID_SEND_BT_CMD;
    buffer[3] = 5;
    buffer[4] = 'A';
    buffer[5] = 'T';
    buffer[6] = '#';
    len = 10;

    switch (type) {
    case ABT_SyncPhoneBook:
        buffer[7] = 'P';
        buffer[8] = 'A';
        if (bt_sync_type == ABST_Undefine)
            bt_sync_type = ABST_PhoneBook;
        break;
    case ABT_SyncReceiveRecord:
        buffer[7] = 'P';
        buffer[8] = 'I';
        if (bt_sync_type == ABST_Undefine)
            bt_sync_type = ABST_ReceiveRecord;
        break;
    case ABT_SyncDialedRecord:
        buffer[7] = 'P';
        buffer[8] = 'H';
        if (bt_sync_type == ABST_Undefine)
            bt_sync_type = ABST_DialedRecord;
        break;
    case ABT_SyncMissRecord:
        buffer[7] = 'P';
        buffer[8] = 'J';
        if (bt_sync_type == ABST_Undefine)
            bt_sync_type = ABST_MissRecord;
        break;
    case ABT_CancelSync:
        buffer[7] = 'P';
        buffer[8] = 'Z';
        break;
    case ABT_DeletePairedListIndex: {
        buffer[7] = 'P';
        buffer[8] = 'P';
        if (data != NULL && length == 1) {
            buffer[3] = 6;
            buffer[9] = *data;
            len = 11;
        }
        break;
    }
    case ABT_PairedMode:
        buffer[7] = 'C';
        buffer[8] = 'A';
        break;
    case ABT_CancelPairedMode:
        buffer[7] = 'C';
        buffer[8] = 'B';
        break;
    case ABT_Connect: {
        buffer[7] = 'C';
        buffer[8] = 'C';
        if (data != NULL && length == 1) {
            buffer[3] = 6;
            buffer[9] = *data;
            len = 11;
        }
        if (ABS_CONNECTING != bt_info.status) {
            bt_info.status = ABS_CONNECTING;
            LOG_FFLDEBUG("bt_info.status = %d\n", bt_info.status);
            unsigned char st = bt_info.status;
            notify_bt_event(ABT_GetStatus, &st, 1);
        }
        break;
    }
    case ABT_DisConnect:
        buffer[7] = 'C';
        buffer[8] = 'D';
        break;
    case ABT_PickUp:
        buffer[7] = 'C';
        buffer[8] = 'E';
        break;
    case ABT_RejectCall:
        buffer[7] = 'C';
        buffer[8] = 'F';
        break;
    case ABT_Hangup:
        buffer[7] = 'C';
        buffer[8] = 'G';
        break;
    case ABT_Redial:
        buffer[7] = 'C';
        buffer[8] = 'H';
        break;
    case ABT_VoiceSwitch:
        buffer[7] = 'C';
        buffer[8] = 'P';
        break;
    case ABT_DialPhone:
        buffer[7] = 'C';
        buffer[8] = 'W';
        if (data != NULL && length > 0) {
            for (i = 0; i < length; i++)
                buffer[9+i] = data[i];
            buffer[9+i] = '\r';
            buffer[3] = 5 + length + 1;
            len = 10 + length + 1;
        }
        break;
    case ABT_DialNumber:
        buffer[7] = 'C';
        buffer[8] = 'X';
        if (data != NULL && length == 1) {
            buffer[9] = *data;
            buffer[3] = 6;
            len = 11;
        }
        break;
    case ABT_MusicPlay:
        buffer[7] = 'M';
        buffer[8] = 'A';
        break;
    case ABT_MusicPause:
        buffer[7] = 'M';
        buffer[8] = 'B';
        break;
    case ABT_MusicStop:
        buffer[7] = 'M';
        buffer[8] = 'C';
        break;
    case ABT_MusicNext:
        buffer[7] = 'M';
        buffer[8] = 'D';
        break;
    case ABT_MusicPre:
        buffer[7] = 'M';
        buffer[8] = 'E';
        break;
    case ABT_SetLocalName:
        buffer[7] = 'M';
        buffer[8] = 'M';
        if (data != NULL && length > 0) {
            for (i = 0; i < length; i++)
                buffer[9+i] = data[i];
            buffer[3] = 5 + length;
            len = 10 + length;
        }
        break;
    case ABT_SetPinCode:
        buffer[7] = 'M';
        buffer[8] = 'N';
        if (data != NULL && length == 4) {
            for (i = 0; i < length; i++)
                buffer[9+i] = data[i];
            buffer[3] = 9;
            len = 14;
        }
        break;
    case ABT_SetAutoAnswer:
        buffer[7] = 'M';
        buffer[8] = 'P';
        break;
    case ABT_CancelAutoAnswer:
        buffer[7] = 'M';
        buffer[8] = 'Q';
        break;
    case ABT_SetAutoConnect:
        buffer[7] = 'M';
        buffer[8] = 'G';
        break;
    case ABT_CancelAutoconnect:
        buffer[7] = 'M';
        buffer[8] = 'H';
        break;
    case ABT_QueryAnsConCfg:
        buffer[7] = 'M';
        buffer[8] = 'F';
        break;
    }

    return len;
}

int ark169_mcu_write(const struct ArkProtocol* protocol)
{
    unsigned char buffer[576] = {0};
    unsigned char checksum = 0;
    unsigned short int len = 0;
    int i;
    unsigned short int data_length = 0;
    gettimeofday(&old_tv, 0);
    buffer[0] = 0x02;
    ++data_length;
    if (mcu_serial_port_fd >= 0 && NULL != protocol) {
        switch (protocol->type) {
        case AT_Control:
            if (NULL != protocol->data) {
                struct ArkControl *control = (struct ArkControl*)protocol->data;
                switch (control->type) {
                case ACLT_Handshake:
                    buffer[1] = ARM_SENDDATA_CMD;
                    buffer[2] = Arm_Send_HandShakeCmd;
                    buffer[3] = 0x0;
                    len = 5;
                    break;
                case ACLT_Heartbeat:
                    buffer[1] = ARM_SENDDATA_CMD;
                    buffer[2] = Arm_Send_HeartBeatCmd;
                    buffer[3] = 0x0;
                    len = 5;
                    break;
                case ACLT_QueryDeviceState:
                    buffer[1] = ARM_SENDDATA_CMD;
                    buffer[2] = Arm_Send_McuDeviceStateCmd;
                    buffer[3] = 0x0;
                    len = 5;
                    break;
                case ACLT_RequestSourceSwitch: {
                    buffer[1] = ARM_SENDDATA_CMD;
                    buffer[2] = Arm_Send_ChangeSourceCmd;
                    buffer[3] = 0x02;
                    buffer[4] = control->data[0];
                    buffer[5] = 0x00;
                    len = 7;
                    break;
                }
                case ACLT_SetReset: {
                    buffer[1] = ARM_SENDDATA_CMD;
                    buffer[2] = Arm_Send_KeyCmd;
                    buffer[3] = 1;
                    buffer[4] = C_KEY_SETTING_RESET;
                    //                    buffer[5] = *control->data;
                    //                    buffer[6] = buffer[7] = buffer[8] = 0;
                    len = 6;
                    //                    buffer[1] = ARM_SENDDATA_CMD;
                    //                    buffer[2] = C_KEY_SETTING_RESET;
                    //                    buffer[3] = 0x01;
                    //                    buffer[4] = 0x01;
                    //                    len = 6;
                    break;
                }
                case ACLT_SWCStartSampling:
                    LOG_FFLDEBUG("\n");
                    buffer[1] = ARM_SENDDATA_CMD;
                    buffer[2] = Arm_Send_SwcStudyCmd;
                    buffer[3] = 0x01;
                    buffer[4] = 0x01;
                    len = 6;
                    break;
                case ACLT_SWCExitSampling:
                    LOG_FFLDEBUG("\n");
                    buffer[1] = ARM_SENDDATA_CMD;
                    buffer[2] = Arm_Send_SwcStudyCmd;
                    buffer[3] = 0x01;
                    buffer[4] = 0x00;
                    len = 6;
                    break;
                case ACLT_SWCGetTable:
                    buffer[1] = ARM_SENDDATA_CMD;
                    buffer[2] = Arm_Req_SwcTabCmd;
                    buffer[3] = 0x00;
                    len = 5;
                    break;
                case ACLT_SWCSave:
                    buffer[1] = ARM_SENDDATA_CMD;
                    buffer[2] = Arm_Send_SwcStudyCmd;
                    buffer[3] = 0x01;
                    buffer[4] = 0x02;
                    len = 6;
                    break;
                case ACLT_SWCReset:
                    buffer[1] = ARM_SENDDATA_CMD;
                    buffer[2] = Arm_Send_SwcStudyCmd;
                    buffer[3] = 0x01;
                    buffer[4] = 0x03;
                    len = 6;
                    break;
                case ACLT_SWCStudy:
                    if (control->data != NULL) {
                        buffer[1] = ARM_SENDDATA_CMD;
                        buffer[2] = Arm_Send_KeyCmd;
                        buffer[3] = 5;
                        buffer[4] = C_KEY_WHEEL_STUDY;
                        buffer[5] = *control->data;
                        buffer[6] = buffer[7] = buffer[8] = 0;
                        len = 10;
                    }
                    break;
                default: {

                    break;
                }
                }
            }
            break;
        case AT_Radio:
            if (NULL != protocol->data) {
                struct ArkRadio *radio = (struct ArkRadio*)protocol->data;
                switch (radio->type) {
                case ART_KEY_Smeter: {
                    buffer[1] = ARM_SENDDATA_CMD;
                    buffer[2] = Arm_Send_KeyCmd;
                    buffer[3] = 7;
                    buffer[4] = 0xE2;
                    buffer[5] = radio->data[0];
                    buffer[6] = radio->data[1];
                    buffer[7] = radio->data[2];
                    buffer[8] = radio->data[3];
                    buffer[9] = radio->data[4];
                    buffer[10] = radio->data[5];
                    len = 12;
                    break;
                }
                case ART_QueryFreqInfo:
                    buffer[1] = ARM_SENDDATA_CMD;
                    buffer[2] = Arm_Req_RadioInfoCmd;
                    buffer[3] = 1;
                    buffer[4] = 0;
                    len = 6;
                    break;
                case ART_QueryPresetListInfo:
                    buffer[1] = ARM_SENDDATA_CMD;
                    buffer[2] = Arm_Req_RadioInfoCmd;
                    buffer[3] = 1;
                    buffer[4] = 1;
                    len = 6;
                    break;
                case ART_Key_1:
                case ART_Key_2:
                case ART_Key_3:
                case ART_Key_4:
                case ART_Key_5:
                case ART_Key_6: {
                    buffer[1] = ARM_SENDDATA_CMD;
                    buffer[2] = Arm_Send_KeyCmd;
                    buffer[3] = 5;
                    buffer[4] = 0x20 + radio->type - ART_KEY;
                    buffer[5] = buffer[6] = buffer[7] = buffer[8] = 0;
                    len = 10;
                    break;
                }
                case ART_KEY_Band:
                case ART_KEY_TuningUp:
                case ART_KEY_TuningDown:
                case ART_KEY_SeekUp:
                case ART_KEY_SeekDown:
                case ART_KEY_AutoScan:
                case ART_KEY_PS:
                case ART_KEY_LOC:
                case ART_KEY_TA:
                case ART_KEY_PTY:
                case ART_KEY_AF:
                case ART_KEY_REG:
                case ART_KEY_CT:
                case ART_KEY_EON:
                case ART_KEY_RDS:
                case ART_KEY_RDSType:
                case ART_KEY_PtySeek:
                case ART_KEY_StereoStateChange:
                    buffer[1] = ARM_SENDDATA_CMD;
                    buffer[2] = Arm_Send_KeyCmd;
                    buffer[3] = 5;
                    buffer[4] = 0x50 + radio->type - ART_KEY_Band;
                    buffer[5] = buffer[6] = buffer[7] = buffer[8] = 0;
                    len = 10;
                    break;
                case ART_KEY_FMFreq:
                case ART_KEY_AMFreq:
                    if (NULL != radio->data && radio->length == 2) {
                        buffer[1] = ARM_SENDDATA_CMD;
                        buffer[2] = Arm_Send_KeyCmd;
                        buffer[3] = 5;
                        buffer[4] = 0x50 + radio->type - ART_KEY_Band;
                        buffer[5] = buffer[6] = 0;
                        buffer[7] = radio->data[0];
                        buffer[8] = radio->data[1];
                        len = 10;
                    }
                    break;
                case ART_KEY_PresetSave:
                case ART_KEY_PresetLoad:
                    if (NULL != radio->data && radio->length == 1 &&
                            *(radio->data) >=1 && *(radio->data) <=6) {
                        buffer[1] = ARM_SENDDATA_CMD;
                        buffer[2] = Arm_Send_KeyCmd;
                        buffer[3] = 5;
                        buffer[4] = 0x50 + radio->type - ART_KEY_Band;
                        buffer[5] = *(radio->data);
                        buffer[6] = buffer[7] = buffer[8] = 0;
                        len = 10;
                    }
                    break;
                }
            }
            break;
        case AT_Bluetooth:
            if (NULL != protocol->data) {
                struct ArkBlueTooth *bt = (struct ArkBlueTooth*)protocol->data;
                len = send_bt_protocol(bt->type, buffer, bt->data, bt->length);
            }
            break;
        case AT_Car: {
            if (NULL != protocol->data) {
                struct ArkCar* car = (struct ArkCar*)(protocol->data);
                if (NULL != car) {
                    switch (car->type) {
                    case ACT_Time: {
                        int i = 0;
                        for (i = 0; i <  car->length; ++i) {
                            LOG_DEBUG("data[i] = %d\n", car->data[i]);
                        }
                        buffer[data_length++] = ARM_SENDDATA_CMD;
                        buffer[data_length++] = Arm_Send_SysInfoCmd;
                        buffer[data_length++] = 0x00;
                        buffer[data_length++] = 0x02;
                        unsigned char iter = 0;
                        for (iter = 0; iter < car->length; ++iter) {
                            buffer[data_length++] = car->data[iter];
                        }
                        buffer[data_length++] = 0x02;
                        buffer[0x03] = data_length - 4;
                        len = data_length + 1;
                        break;
                    }
                    case ACT_Version: {
                        LOG_FFLDEBUG("\n");
                        buffer[data_length++] = ARM_SENDDATA_CMD;
                        buffer[data_length++] = Arm_Req_McuVerCmd;
                        buffer[data_length++] = 0x00;
                        buffer[0x03] = data_length - 4;
                        len = data_length + 1;
                        break;
                    }
                    case ACT_MCUUpdate: {
                        switch (car->status) {
                        case ACS_MCUUpdateStart: {
#if 0
                            //                            04 02 00 00 10
                            //                            char* tmp = {0x02, 0x00, 0x00, 0x10};
                            //                            mcu_update_handler(tmp, 04);
                            LOG_FFLDEBUG("len = %d\n", len);
                            int j = 0;
                            //                            for (j = 0; j < len; ++j) {
                            //                                LOG_FFLDEBUG("%d=%d\n", j, data[j]);
                            //                            }
                            unsigned char buffer[256 + 8];
                            unsigned char checksum = 0;
                            unsigned char length = 0;
                            j = 0;
                            buffer[j++] = 0x02;
                            int count;
                            for (count = 0; count < 16; ++count) {
                                //                usleep(50 * 1000);
                                sleep(1);
                                LOG_FFLDEBUG("!!%d\n", count);
                                j = 1;
                                buffer[j++] = ARM_SENDDATA_CMD;
                                buffer[j++] = Mcu_Send_Update_Status;
                                buffer[j++] = 0x00;
                                buffer[j++] = ERR_SUCCESS;
                                buffer[j++] = 0x02;
                                buffer[j++] = count + 1;
                                int l = 0;
                                for (l = 0; l < 3; ++l) {
                                    buffer[j++] = l;
                                }
                                buffer[0x03] = j - 4;
                                length = j + 1;
                                if (length > 0) {
                                    int i;
                                    for (i = 1; i < length - 1; i++) {
                                        checksum ^= buffer[i];
                                    }
                                    buffer[length - 1] = checksum;
                                    LOG_DEBUG("ARM to MCU : ");
                                    for (i = 0; i < length; i++)
                                        LOG_DEBUG("%.2x ", buffer[i]);
                                    LOG_DEBUG("\n");
                                    for (i = 0; i < length; i++) {
                                        if ((buffer[i] >= ' ')
                                                && (buffer[i] < 'z')) {
                                            LOG_DEBUG("%c ", buffer[i]);
                                        }
                                    }
                                    LOG_DEBUG("\n");
                                    int ret = write(mcu_serial_port_fd, buffer, length);
                                    fsync(mcu_serial_port_fd);
                                    LOG_DEBUG("ret = length = %d, ret%d\n", length, ret);
                                }
                            }
                            return 0;
#endif
                            //                            LOG_FFLDEBUG("\n");
                            buffer[data_length++] = ARM_SENDDATA_CMD;
                            buffer[data_length++] = Arm_Send_McuUpdateCmd;
                            buffer[data_length++] = 0x01;
                            buffer[data_length++] = 0x00;
                            buffer[0x03] = data_length - 4;
                            //                            len = 6;
                            len = data_length + 1;
                            break;
                        }
                        case ACS_MCUUpdateFileLen: {
                            buffer[data_length++] = ARM_ACKDATA_CMD;
                            buffer[data_length++] = Mcu_Send_Update_Status;
                            buffer[data_length++] = 0x00;
                            buffer[data_length++] = ERR_SUCCESS;
                            buffer[data_length++] = 0x01;
                            unsigned char iter = 0;
                            for (iter = 0; iter < car->length; ++iter) {
                                buffer[data_length++] = car->data[iter];
                            }
                            buffer[0x03] = data_length - 4;
                            len = data_length + 1;
                            //                            LOG_FFLDEBUG("\n");
                            break;
                        }
                        case ACS_MCUUpdateTransfer: {
                            //                            LOG_FFLDEBUG("car->length=%d\n", car->length);
                            buffer[data_length++] = ARM_ACKDATA_CMD;
                            buffer[data_length++] = Mcu_Send_Update_Status;
                            buffer[data_length++] = 0x00;
                            buffer[data_length++] = ERR_SUCCESS;
                            buffer[data_length++] = 0x02;
                            buffer[data_length++] = 0x01;
                            unsigned short int iter = 0;
                            for (iter = 0; iter < car->length; ++iter) {
                                buffer[data_length++] = car->data[iter];
                            }
                            buffer[0x03] = (data_length - 4);
                            len = data_length + 1;
                            //                            LOG_FFLDEBUG("len=%d\n", len);
#if 0
                            //            int count;
                            //            for (count = 0; count < data[3]; ++count) {
                            //                usleep(50 * 1000);
                            //            LOG_FFLDEBUG("!!%d\n", count);
                            j = 1;
                            buffer[j++] = ARM_ACKDATA_CMD;
                            buffer[j++] = Mcu_Send_Update_Status;
                            buffer[j++] = 0x00;
                            buffer[j++] = ERR_SUCCESS;
                            buffer[j++] = 0x02;
                            buffer[j++] = 0x01;
                            //            buffer[j++] = /*count + 1*/data[1];
                            //            buffer[j++] = /*count + 1*/data[2];
                            int l = 0;
                            for (l = 0; l < 128/*256*/; ++l) {
                                buffer[j++] = l;
                            }
                            buffer[0x03] = (j - 4);
                            length = j + 1;

                            if (length > 0) {
                                int i;
                                for (i = 1; i < length - 1; i++) {
                                    checksum ^= buffer[i];
                                }
                                buffer[length - 1] = checksum;
                                LOG_DEBUG("ARM to MCU : ");
                                for (i = 0; i < length; i++)
                                    LOG_DEBUG("%.2x ", buffer[i]);
                                LOG_DEBUG("\n");
                                //                for (i = 0; i < length; i++) {
                                //                    if ((buffer[i] >= ' ')
                                //                            && (buffer[i] < 'z')) {
                                //                        LOG_DEBUG("%c ", buffer[i]);
                                //                    }
                                //                }
                                LOG_DEBUG("\n");
                                int ret = write(mcu_serial_port_fd, buffer, length);
                                fsync(mcu_serial_port_fd);
                                LOG_DEBUG("ret = length = %d, ret%d\n", length, ret);
                                return 0;
                            }
#endif
                            break;
                        }
                        case ACS_MCUUpdateTransferEnd: {
                            //                            LOG_FFLDEBUG("\n");
                            break;
                        }
                        default: {
                            break;
                        }
                        }
                        break;
                    }
                    default: {
                        break;
                    }
                    }
                }
            }
            break;
        }
        default:
            LOG_DEBUG("Unimplemented protocal type %d handler.\n", protocol->type);
            break;

        }
        for (i = 1; i < len - 1; i++) {
            checksum ^= buffer[i];
        }
        if (len > 0) {
            buffer[len - 1] = checksum;
            LOG_DEBUG("ARM to MCU : ");
            for (i = 0; i < len; i++)
                LOG_DEBUG("%.2x ", buffer[i]);
            LOG_DEBUG("\n");
            for (i = 0; i < len; i++) {
                if ((buffer[i] >= ' ')
                        && (buffer[i] <= 'z')) {
                    LOG_DEBUG("%c ", buffer[i]);
                }
            }
            LOG_DEBUG("\n");
            int ret = write(mcu_serial_port_fd, buffer, len);
            if (ret != len) {
                LOG_FFLDEBUG("ret not equal len\n");
            }
            //            LOG_DEBUG("ret = len = %d, ret%d\n", len, ret);
        } else {
            LOG_FFLDEBUG("len <= 0\n");
        }
    }
    return 0;
}
//#Protocal
//export TOUCHSERIAL=/dev/ttyS2
//export TOUCHSERIAL_BAUDRATE=19200
//export TOUCHSERIAL=/dev/ttyS2
//export COMMANDSERIAL_BAUDRATE=19200
//export PROTOCOL_ID=ark169
enum ArkCustomer ark169_init()
{
    LOG_DEBUG("ark169_init\n");
    char* customer_id = (char*)getenv("PROTOCOL_ID");
    if (NULL != customer_id) {
        if (0 == strcmp(customer_id, "ark169")) {
            //            export_gpio(69);
            //            direction_gpio(69, "out");
            //            set_value_gpio(69, 1);
        } else return AC_Undefine;
    } else return AC_Undefine;

    reversing_fd = open("/dev/ark_display", O_RDONLY);
    LOG_FFLDEBUG("reversing_fd=%d\n", reversing_fd);
    if (reversing_fd < 0) {
        LOG_DEBUG("open /dev/ark_display fail!\n");
        reversing_fd = -1;
        //        goto error;
    }
    ark169.fd[0] = reversing_fd;
    LOG_DEBUG("reversing_fd = %d\n", reversing_fd);

    frame_thread_exit_flag = 0;
    if (0 != pthread_create(&frame_handler_pthread, NULL, (void*(*)(void*))uart_frame_handler, NULL)) {
        LOG_DEBUG("pthread_create frame_handler_pthread error\n");
        //        goto error;
    }

    mcu_serial_port_fd = open("/dev/ttyS2", O_RDWR);
    if (mcu_serial_port_fd < 0) {
        LOG_DEBUG("open mcu uart %s error\n", "/dev/ttyS2");
        mcu_serial_port_fd = -1;
        //        goto error;
        //        return AC_Error;
    } else {
        set_speed(mcu_serial_port_fd, 115200);
        if (set_parity(mcu_serial_port_fd, TOUCH_DATA_BITS, TOUCH_STOP_BITS, TOUCH_PARITY_STATUS) == false) {
            LOG_DEBUG("uart_touch_init: Set parity error\n");
            if (mcu_serial_port_fd >= 0) {
                close(mcu_serial_port_fd);
                mcu_serial_port_fd = -1;
            }
            //        return AC_Error;
        }
    }
    ark169.fd[1] = mcu_serial_port_fd;
    LOG_DEBUG("mcu_serial_port_fd = %d\n", mcu_serial_port_fd);

    if (0 == access(KEY_INPUT_DEVICE_PATH, F_OK)) {
        if (-1 == (input_fd = open(KEY_INPUT_DEVICE_PATH, O_RDONLY))) {
            LOG_DEBUG("open %s error\n", KEY_INPUT_DEVICE_PATH);
            input_fd = -1;
            //    return AC_Error;
        }
    }
    ark169.fd[2] = input_fd;
    LOG_DEBUG("input_fd = %d\n", input_fd);

    ill_fd = open("/dev/ill_detection", O_RDONLY);
    if (ill_fd < 0) {
        LOG_DEBUG("open /dev/ill_detection fail!\n");
        goto error;
    }
    ark169.fd[3] = ill_fd;
    LOG_DEBUG("ill_fd = %d\n", ill_fd);

    brake_fd = open("/dev/brake_detection", O_RDONLY);
    if (brake_fd < 0) {
        LOG_DEBUG("open /dev/brake_detection fail!\n");
        goto error;
    }
    ark169.fd[4] = brake_fd;
    LOG_DEBUG("brake_fd = %d\n", brake_fd);
    
    set_heartbeat_tick(10);

    return AC_ARK169;
error:
    //    unexport_gpio(69);
    if (mcu_serial_port_fd >= 0) {
        close(mcu_serial_port_fd);
        mcu_serial_port_fd = -1;
    }
    ark169.fd[0] = -1;
    if (input_fd >= 0) {
        close(input_fd);
        input_fd = -1;
    }
    ark169.fd[1] = -1;
    if (reversing_fd >= 0) {
        close(reversing_fd);
        reversing_fd = -1;
    }
    ark169.fd[2] = -1;
    if (ill_fd >= 0) {
        close(ill_fd);
        ill_fd = -1;
        ark169.fd[3] = -1;
    }
    if (brake_fd >= 0) {
        close(brake_fd);
        brake_fd = -1;
        ark169.fd[4] = -1;
    }

    return AC_Error;
}

int ark169_uninit()
{
    LOG_DEBUG("ark169_uninit\n");
    //    unexport_gpio(69);

    frame_thread_exit_flag = 1;
    sem_post(&frame_sem);
    pthread_join(frame_handler_pthread, NULL);
    sem_destroy(&frame_sem);

    if (-1 != mcu_serial_port_fd) {
        close(mcu_serial_port_fd);
        mcu_serial_port_fd = -1;
    }
    if (-1 != input_fd) {
        close(input_fd);
        input_fd = -1;
    }
    if (reversing_fd >= 0) {
        close(reversing_fd);
        reversing_fd = -1;
        ark169.fd[2] = -1;
    }
    return 0;
}

struct CustomerProtocol ark169 = {
    .customer = AC_ARK169,
    .fd = {-1, -1, -1},
    .init = ark169_init,
    .read = ark169_mcu_read,
    .write = ark169_mcu_write,
    .uninit = ark169_uninit,
};
