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
#include "../ArkCar.h"
#include "../ArkNaviProtocol.h"
#include "YXCX.h"

#define YXCX_TOUCH_RELEASE			0x00
#define YXCX_TOUCH_PRESSURE			0x01
#define YXCX_TOUCH_TIMEOUT			0x02
#define YXCX_PACK_HEAD				0x55

#define SYS_REG_BASE	0xE4900000
#define GPIO_REG_BASE	0xE4600000
#define REG_SIZE		0x1000

#define YXCX_USE_SIMULATION_TIMER
//#define YXCX_TEST   //use for test

//#define YXCX_MUTEX_LOCK
#ifdef YXCX_MUTEX_LOCK
#define PTHREAD_MUTEX_LOCK(x)       pthread_mutex_lock(x)
#define PTHREAD_MUTEX_UNLOCK(x)     pthread_mutex_unlock(x)
#else
#define PTHREAD_MUTEX_LOCK(x)
#define PTHREAD_MUTEX_UNLOCK(x)
#endif

typedef enum yxcx_cmd_type
{
    //ARM TO MCU
    YXCX_CMD_ARM_READY      =0x01,
    YXCX_CMD_SCR_MODE       =0x02,  //screen switch(switch to car or display)

    //MCU TO ARM
    YXCX_CMD_SCR_STATE      =0x10,  //screen current status(car/navition/reverse/360/off)
    YXCX_CMD_KEY_EVENT      =0x30,  //key event
    YXCX_CMD_MCU_VER        =0x60,  //mcu version
    YXCX_CMD_TIMEDATE       =0x70,  //time
    YXCX_CMD_SCREEN         =0x71,  //screen resolution
    YXCX_CMD_TOUCH2_POS     =0x80,  //touch screen coordinate
}E_YXCX_CMD_TYPE;

typedef enum yxcx_arm_cmd_status
{
    //arm initilize status
    YXCX_IN_THE_START = 0x0,
    YXCX_ARM_READY,
    YXCX_HEARTBEAT_PACKAGE,

    //screen status
    YXCX_CAR_SCREEN,
    YXCX_NAVI_SCREEN,
}E_YXCX_ARM_CMD_STATUS;

typedef enum yxcx_screen_type
{
    YXCX_SCREEN_STATUS_CAR          = 0x0,
    YXCX_SCREEN_STATUS_NAVI         = 0x01,
    YXCX_SCREEN_STATUS_ASTERN_360   = 0x02,
    YXCX_SCREEN_STATUS_POWEROFF     = 0x03,
    YXCX_SCREEN_STATUS_UNDEFINE     = 0xff,
}E_YXCX_SCREEN_TYPE;

typedef enum yxcx_screen_mode
{
    YXCX_SCREEN_INPUT           = 0x0,
    YXCX_SCREEN_OUTPUT_RGB888   = 0x01,
    YXCX_SCREEN_OUTPUT_VGA      = 0x02,
}E_YXCX_SCREEN_MODE;

typedef enum yxcx_key_type
{
    YXCX_UP_KEY         = 0x01,
    YXCX_DOWN_KEY       = 0x02,
    YXCX_LEFT_KEY       = 0x03,
    YXCX_RIGHT_KEY      = 0x04,
    YXCX_ENTER_KEY      = 0x05,
    YXCX_CANCEL_KEY     = 0x06,
    YXCX_RETURN_KEY     = 0x07,
    YXCX_LEFTTURN_KEY   = 0x08,
    YXCX_RIGHTTURN_KEY	= 0x09,
    YXCX_NAVI_KEY       = 0x0A,
    YXCX_SPEECH_KEY     = 0x0B,
}E_YXCX_KEY_TYPE;

enum yxcx_touch_status{
    TOUCH_RELEASE = 0x0,
    TOUCH_PRESS,
    TOUCH_CALIBRATE,
};
enum yxcx_screen_resolution
{
    RESOLUTION_UNDIFINE = -1,
    RESOLUTION_800X480 = 0x0,
    RESOLUTION_1280X480,
    RESOLUTION_1024X480,
    RESOLUTION_640X480,
};

typedef unsigned int  u32;
extern void callback_notify(const struct ArkProtocol* protocol);
extern int ark_protocol_send(const struct ArkProtocol* protocol);
static int get_and_parser_arkdata_ini(const char *src_file, const char *dist_file);
static int get_screen_resolution(void);
static void resolution_test(void);


static unsigned char read_buf[256];
static int pack_len = 0;
static int count = 0;
static unsigned char cmd_type_tab[] = {0x10,0x30,0x60,0x70,0x71,0x80};
static int arm_init_status = 0;
static int touch_status = TOUCH_RELEASE;
static E_YXCX_SCREEN_TYPE current_screen = YXCX_SCREEN_STATUS_UNDEFINE;
static pthread_mutex_t yxcx_mutex;

#define TP_LONG_PRESS_CALIBRATION  //touchscreen press more than 10s, then calibrate touchscreen.
#ifdef TP_LONG_PRESS_CALIBRATION
static struct timeval tp_tv = {0,0};    //touchscreen timeout
static int tmp_x,tmp_y;     //save coordinate value
#endif
static int touch_command_serial_port_fd = -1;
static int input_fd = -1;

static int input_report(int type, int code, int val)
{
    int ret = 0;
    static struct input_event ie;
    ie.type = type;
    ie.code = code;
    ie.value = val;

    if (-1 != input_fd)
    {
        ret = write(input_fd, &ie, sizeof(struct input_event));
    }

    return ret;
}

/*------------------------------------------------------------------------------
    Function name   : DWLMapRegisters
    Description     :

    Return type     : u32 - the HW ID
------------------------------------------------------------------------------*/
static u32 *DWLMapRegisters(int mem_dev, unsigned int base,
                     unsigned int regSize, u32 write)
{
    const int pageSize = getpagesize();
    const int pageAlignment = pageSize - 1;

    size_t mapSize;
    //const char *io = MAP_FAILED;
    const char *io = (char *)MAP_FAILED;

    /* increase mapping size with unaligned part */
    mapSize = regSize + (base & pageAlignment);

    /* map page aligned base */
    if(write)
        io = (char *) mmap(0, mapSize, PROT_READ | PROT_WRITE,
                           MAP_SHARED, mem_dev, base & ~pageAlignment);
    else
        io = (char *) mmap(0, mapSize, PROT_READ, MAP_SHARED,
                           mem_dev, base & ~pageAlignment);

    /* add offset from alignment to the io start address */
    if(io != (char *)MAP_FAILED)
        io += (base & pageAlignment);

    return (u32 *) io;
}

static void DWLUnmapRegisters(const void *io, unsigned int regSize)
{
    const int pageSize = getpagesize();
    const int pageAlignment = pageSize - 1;

    munmap((void *) ((int) io & (~pageAlignment)),
           regSize + ((int) io & pageAlignment));
}


/**
 * ch = 2 VGA
 * ch = 1 RGB888
 * ch = 0 io input
 **/
static void display_muxer_switch(int ch)
{
    int fd = -1;
    u32 *sysio = (u32 *)MAP_FAILED, *gpio = (u32 *)MAP_FAILED;

    fd = open("/dev/mem", O_RDWR | O_SYNC);
    if(fd == -1)
    {
        printf("failed to open /dev/mem, error %d is %s\n", errno, strerror(errno));
        return;
    }

    sysio = (u32 *)DWLMapRegisters(fd, SYS_REG_BASE, REG_SIZE, 1);
    if(sysio == (u32 *)MAP_FAILED)
    {
        printf("failed to mmap sys regs. error %d is %s\n", errno, strerror(errno));
        goto end;
    }

    gpio = (u32 *)DWLMapRegisters(fd, GPIO_REG_BASE, REG_SIZE, 1);
    if(gpio == (u32 *)MAP_FAILED)
    {
        printf("failed to mmap gpio regs. error %d is %s\n", errno, strerror(errno));
        goto end;
    }

    switch (ch)
    {
        case 0:	//io input
            *(sysio + 0x1C0 / 4) = 0;
            *(sysio + 0x1C4 / 4) = 0;
            *(sysio + 0x1C8 / 4) = 0;
            *(sysio + 0x1CC / 4) &= ~0xFFFF;
            *(gpio + 0) |= (0xFFFFFFF << 2);
            break;
        case 2:	//VGA
            *(sysio + 0x1C0 / 4) = (1<<28) |(1<<24) |(1<<20) |(1<<16) |(1<<12) |(1<<8) | (1<<4) |(1<<0);
            *(sysio + 0x1C4 / 4) = (1<<28) |(1<<24) |(1<<20) |(1<<16) |(1<<12) |(1<<8) | (1<<4) |(1<<0);
            *(sysio + 0x1C8 / 4) = (1<<28) |(1<<24) |(1<<20) |(1<<16) |(1<<12) |(1<<8) | (1<<4) |(1<<0);
            *(sysio + 0x1CC / 4) = (5<<12) |(5<<8) | (1<<4) |(1<<0);
            break;
        case 1:	//rgb888
        default:
            *(sysio + 0x1C0 / 4) = (1<<28) |(1<<24) |(1<<20) |(1<<16) |(1<<12) |(1<<8) | (1<<4) |(1<<0);
            *(sysio + 0x1C4 / 4) = (1<<28) |(1<<24) |(1<<20) |(1<<16) |(1<<12) |(1<<8) | (1<<4) |(1<<0);
            *(sysio + 0x1C8 / 4) = (1<<28) |(1<<24) |(1<<20) |(1<<16) |(1<<12) |(1<<8) | (1<<4) |(1<<0);
            *(sysio + 0x1CC / 4) = (1<<12) |(1<<8) | (1<<4) |(1<<0);
            break;
    }

end:
    if (sysio != (u32 *)MAP_FAILED)
    {
        DWLUnmapRegisters(sysio, REG_SIZE);
    }

    if (gpio != (u32 *)MAP_FAILED)
    {
        DWLUnmapRegisters(gpio, REG_SIZE);
    }

    if (fd != -1)
    {
        close(fd);
    }
}

/**
 * CRC checkout
 **/
static unsigned char crc_checkout(unsigned char *buf, int len)
{
    int i;
    unsigned char checkout = 0;

    //checkout
    for(i=0; i<len; i++)
        checkout ^= buf[i];

    return checkout;
}

/**
 * find cmd type form
 **/
static int find_cmd_type(unsigned char cmd)
{
    int i;

    for(i=0; i<(sizeof(cmd_type_tab)/sizeof(unsigned char)); i++)
    {
        if(cmd == cmd_type_tab[i])
            return cmd ;
    }

    return 0;
}

/**
 * touchscreen coordinate data parse
 **/
static void touch_parse(unsigned char *buf)
{
    unsigned int x,y;
    unsigned char head;

    head = buf[0];
    x = (buf[1] << 8) | (buf[2] << 0);
    y = (buf[3] << 8) | (buf[4] << 0);

    if(head == YXCX_TOUCH_RELEASE)	//release
    {
        if(touch_status != TOUCH_RELEASE)
        {
            //printf("release: x:%d, y:%d\n",x,y);
            input_report(EV_ABS,ABS_PRESSURE,0x0);	//pressure val
            input_report(EV_SYN,SYN_REPORT,0x0);	//sync
            touch_status = TOUCH_RELEASE;
        }
    }
    else if(head == YXCX_TOUCH_PRESSURE)		//pressure
    {
        //printf("pressure: x:%d, y:%d\n",x,y);
#ifdef TP_LONG_PRESS_CALIBRATION
        if(touch_status == TOUCH_RELEASE)   //first press
        {
            touch_status = TOUCH_PRESS;
            tmp_x = x;
            tmp_y = y;
            gettimeofday(&tp_tv,0);
        }
        else if(touch_status == TOUCH_PRESS)    //repeat press
        {
            if(tmp_x == x && tmp_y == y)    //touch point do not move
            {
                struct timeval cur_tv;
                int time;

                gettimeofday(&cur_tv,0);

                time = (cur_tv.tv_sec - tp_tv.tv_sec)*1000*1000 + cur_tv.tv_usec - tp_tv.tv_usec;
                if(time >= 10*1000*1000)    //10s
                {
                    printf("#### calibrate\n");
                    touch_status = TOUCH_CALIBRATE;

                    struct ArkCar car;
                    memset(&car, 0, sizeof(struct ArkCar));
                    car.type = ACT_Calibrate;
                    car.status = ACS_CalibrateOn;
                    car.data = NULL;
                    car.length = 0;

                    struct ArkProtocol protocol;
                    memset(&protocol, 0, sizeof(struct ArkProtocol));
                    protocol.type = AT_Car;
                    protocol.data = (void *)&car;
                    callback_notify(&protocol);
                }
            }
            else    //touch point move
            {
                tmp_x = x;
                tmp_y = y;
                gettimeofday(&tp_tv,0);
            }

        }
        //else  //repeat calibrate
        //{
        //}


        input_report(EV_ABS,ABS_X,x);	//x
        input_report(EV_ABS,ABS_Y,y);	//y
        input_report(EV_ABS,ABS_PRESSURE,0xFFF);	//pressure val
        input_report(EV_SYN,SYN_REPORT,0x0);		//sync

#else
        touch_status = TOUCH_PRESS;
        input_report(EV_ABS,ABS_X,x);	//x
        input_report(EV_ABS,ABS_Y,y);	//y
        input_report(EV_ABS,ABS_PRESSURE,0xFFF);	//pressure val
        input_report(EV_SYN,SYN_REPORT,0x0);		//sync
#endif
    }
    else if(head == YXCX_TOUCH_TIMEOUT)		//touch timeout, release
    {
        if(touch_status != TOUCH_RELEASE)
        {
            input_report(EV_ABS,ABS_PRESSURE,0x0);	//pressure val
            input_report(EV_SYN,SYN_REPORT,0x0);	//sync
            touch_status = TOUCH_RELEASE;
        }
    }
}

/**
 * uart data parse
 **/
static data_parse(unsigned char cmd, unsigned char *buf, int len)
{
    E_YXCX_CMD_TYPE cmd_type = (E_YXCX_CMD_TYPE)cmd;
    struct ArkProtocol protocol;

    memset(&protocol, 0, sizeof(struct ArkProtocol));

    switch(cmd_type)
    {
        case YXCX_CMD_SCR_STATE:
        {
            unsigned char screen_state = buf[0];
            switch(screen_state)
            {
                case YXCX_SCREEN_STATUS_CAR:
                {
                    //display switch to car
                    if(current_screen != YXCX_SCREEN_STATUS_CAR)
                    {
                        display_muxer_switch(YXCX_SCREEN_INPUT);
                        current_screen = YXCX_SCREEN_STATUS_CAR;
                    }
                    break;
                }
                case YXCX_SCREEN_STATUS_NAVI:
                {
                    //display switch to navigation
                    if(current_screen != YXCX_SCREEN_STATUS_NAVI)
                    {
                        display_muxer_switch(YXCX_SCREEN_OUTPUT_VGA);
                        current_screen = YXCX_SCREEN_STATUS_NAVI;
                    }
                    break;
                }
                case YXCX_SCREEN_STATUS_ASTERN_360:
                {
                    current_screen = YXCX_SCREEN_STATUS_ASTERN_360;
                    break;
                }
                case YXCX_SCREEN_STATUS_POWEROFF:
                {
                    current_screen = YXCX_SCREEN_STATUS_POWEROFF;
                    break;
                }
                default:
                    break;
            }
            break;
        }
        case YXCX_CMD_KEY_EVENT:
        {
            struct ArkKey key;
            memset(&key, 0, sizeof(struct ArkKey));

            unsigned char key_status = buf[0];
            unsigned char key_val = buf[1];

            if(key_status == 1)
                key.status = AKS_Press;
            else if(key_status == 0)
                key.status = AKS_Release;

            switch(key_val)
            {
                case YXCX_UP_KEY:
                    key.type = AKT_CommonUp;
                    break;
                case YXCX_DOWN_KEY:
                    key.type = AKT_CommonDown;
                    break;
                case YXCX_LEFT_KEY:
                    key.type = AKT_CommonLeft;
                    break;
                case YXCX_RIGHT_KEY:
                    key.type = AKT_CommonRight;
                    break;
                case YXCX_ENTER_KEY:
                    key.type = AKT_CommonEnter;
                    break;
                case YXCX_CANCEL_KEY:
                    key.type = AKT_CommonCancel;
                    break;
                case YXCX_RETURN_KEY:
                    key.type = AKT_CommonReturn;
                    break;
                case YXCX_LEFTTURN_KEY:
                    key.type = AKT_CommonLeftTurn;
                    break;
                case YXCX_RIGHTTURN_KEY:
                    key.type = AKT_CommonRightTurn;
                    break;
                case YXCX_NAVI_KEY:
                    key.type = AKT_CommonEnterNavi;
                    break;
                case YXCX_SPEECH_KEY:
                    key.type = AKT_CommonVoice;
                    break;
                default:
                    break;
            }

            protocol.type = AT_Key;
            protocol.data = (void *)&key;

            callback_notify(&protocol);
            break;
        }
        case YXCX_CMD_MCU_VER:  //mcu version
        {
            arm_init_status = 1;

//            if(len != 26)
//                break;

            struct ArkCar car;
            memset(&car, 0, sizeof(struct ArkCar));
            car.type = ACT_Version;
            car.status = ACS_Undefine;
            car.data = (void*)buf;
            car.length = len;

            protocol.type = AT_Car;
            protocol.data = (void *)&car;
            callback_notify(&protocol);

            break;
        }
        case YXCX_CMD_TIMEDATE:		//not use
        {
            arm_init_status = 1;
            break;
        }
        case YXCX_CMD_SCREEN:		//screen resolution
        {
            char src_file[100], dest_file[100],sys_cmd[100];
            char* arkdata_path = (char*)getenv("ARKDATA_FILE_PATH");
            char *p = arkdata_path;
            int file_size;
            int screen_resolution;

            if(arkdata_path == NULL)
                break;

            while(*p != '\0')
                ++p;

            //delete character space and '\0'
            while(p - arkdata_path > 0)
            {
                p--;
                if(*p != '/')  //no ' ' and '\0'
                    break;
                *p = '\0';
            }

            screen_resolution = get_screen_resolution();

            switch(buf[0])
            {
                case RESOLUTION_800X480:
                {
                    if(screen_resolution != RESOLUTION_800X480)
                    {
                        sprintf(src_file,"%s/%s",arkdata_path,"arkdata_800x480.ini");
                        sprintf(dest_file,"%s/%s",arkdata_path,"arkdata");
                        file_size = get_and_parser_arkdata_ini(src_file,dest_file);
                        if(file_size <= 0)
                            break;

                        sprintf(sys_cmd,"dd if=/dev/zero of=/dev/mtdblock7 bs=%d count=1",file_size);
                        system(sys_cmd);
                        sprintf(sys_cmd,"dd if=%s/arkdata of=/dev/mtdblock7 bs=%d count=1",arkdata_path,file_size);
                        system(sys_cmd);
                        sprintf(sys_cmd,"rm -rf %s/arkdata",arkdata_path);
                        system(sys_cmd);
                        system("sync && reboot");
                    }
                    break;
                }
                case RESOLUTION_1280X480:
                {
                    if(screen_resolution != RESOLUTION_1280X480)
                    {
                        sprintf(src_file,"%s/%s",arkdata_path,"arkdata_1280x480.ini");
                        sprintf(dest_file,"%s/%s",arkdata_path,"arkdata");
                        file_size = get_and_parser_arkdata_ini(src_file,dest_file);
                        if(file_size <= 0)
                            break;

                        sprintf(sys_cmd,"dd if=/dev/zero of=/dev/mtdblock7 bs=%d count=1",file_size);
                        system(sys_cmd);
                        sprintf(sys_cmd,"dd if=%s/arkdata of=/dev/mtdblock7 bs=%d count=1",arkdata_path,file_size);
                        system(sys_cmd);
                        sprintf(sys_cmd,"rm -rf %s/arkdata",arkdata_path);
                        system(sys_cmd);
                        system("sync && reboot");
                    }
                    break;
                }
                case RESOLUTION_1024X480:
                {
                    if(screen_resolution != RESOLUTION_1024X480)
                    {
                        sprintf(src_file,"%s/%s",arkdata_path,"arkdata_1024x480.ini");
                        sprintf(dest_file,"%s/%s",arkdata_path,"arkdata");
                        file_size = get_and_parser_arkdata_ini(src_file,dest_file);
                        if(file_size <= 0)
                            break;

                        sprintf(sys_cmd,"dd if=/dev/zero of=/dev/mtdblock7 bs=%d count=1",file_size);
                        system(sys_cmd);
                        sprintf(sys_cmd,"dd if=%s/arkdata of=/dev/mtdblock7 bs=%d count=1",arkdata_path,file_size);
                        system(sys_cmd);
                        sprintf(sys_cmd,"rm -rf %s/arkdata",arkdata_path);
                        system(sys_cmd);
                        system("sync && reboot");
                    }
                    break;
                }
                case RESOLUTION_640X480:
                {
                    if(screen_resolution != RESOLUTION_640X480)
                    {
                        sprintf(src_file,"%s/%s",arkdata_path,"arkdata_640x480.ini");
                        sprintf(dest_file,"%s/%s",arkdata_path,"arkdata");
                        file_size = get_and_parser_arkdata_ini(src_file,dest_file);
                        if(file_size <= 0)
                            break;

                        sprintf(sys_cmd,"dd if=/dev/zero of=/dev/mtdblock7 bs=%d count=1",file_size);
                        system(sys_cmd);
                        sprintf(sys_cmd,"dd if=%s/arkdata of=/dev/mtdblock7 bs=%d count=1",arkdata_path,file_size);
                        system(sys_cmd);
                        sprintf(sys_cmd,"rm -rf %s/arkdata",arkdata_path);
                        system(sys_cmd);
                        system("sync && reboot");
                    }
                    break;
                }
                default:
                    printf("### not support screen resolution\n");
                    break;
            }

            break;
        }
        case YXCX_CMD_TOUCH2_POS:
        {
            touch_parse(buf);
            break;
        }
        default:
            break;
    }
}

/**
 * ARM send data to MCU
 **/
static void arm_send(int type, unsigned char *buf, int len)
{
    unsigned char send_buf[256];
    int send_len = len + 4;
    int ret;

    send_buf[0] = YXCX_PACK_HEAD;	//head
    send_buf[1] = len+2;            //length
    send_buf[2] = type;             //cmd type
    memcpy(send_buf+3,buf,len);
    send_buf[len+3] = crc_checkout(send_buf+1,len+2);

    ret = write(touch_command_serial_port_fd,send_buf,send_len);
    if(ret < 0)
    {
        printf("### write failed\n");
    }
}
static void arm_send_cmd(E_YXCX_CMD_TYPE type, E_YXCX_ARM_CMD_STATUS status)
{
    unsigned char buf[1];
    //   int len;

    switch(type)
    {
        case YXCX_CMD_ARM_READY:
        {
            if(status == YXCX_IN_THE_START)
            {
                buf[0] = 0x0;
                arm_send(YXCX_CMD_ARM_READY,buf,1);
            }
            else if(status == YXCX_ARM_READY)
            {
                buf[0] = 0x01;
                arm_send(YXCX_CMD_ARM_READY,buf,1);
            }
            else if(status == YXCX_HEARTBEAT_PACKAGE)
            {
                buf[0] = 0x55;
                arm_send(YXCX_CMD_ARM_READY,buf,1);
            }

            break;
        }
        case YXCX_CMD_SCR_MODE:
        {
            if(status == YXCX_CAR_SCREEN)
            {
                buf[0] = 0x0;
                arm_send(YXCX_CMD_SCR_MODE,buf,1);
            }
            else if(status == YXCX_NAVI_SCREEN)
            {
                buf[0] = 0x01;
                arm_send(YXCX_CMD_SCR_MODE,buf,1);
            }
            break;
        }
        default:
            break;
    }
}

#ifndef YXCX_USE_SIMULATION_TIMER
/**
 * timer
 **/
static void time_handler(int signo)
{
    static int send_ready_times = 0;

    switch(signo)
    {
    case SIGALRM:
        if(!arm_init_status)	//arm send initilize status to mcu
        {
            printf("arm send initilize ready cmd\n");
            if(++send_ready_times == 3)		// 3 times later, if navigation board initilize uncompletly,then stop and send heartbeat package
            {
                arm_init_status = 1;
                send_ready_times = 0;
            }

            arm_send_cmd(CMD_ARM_READY,ARM_READY);
        }
        else	//arm send heartbeat package to mcu
        {
            arm_send_cmd(CMD_ARM_READY,HEARTBEAT_PACKAGE);

            if(current_screen != STATUS_SCREEN_GPS) //switch to GPS Screen
            {
                arm_send_cmd(CMD_SCR_MODE,NAVI_SCREEN);
            }
        }
        break;
    default:
        break;
    }
}

//timer init
static int arm_timer_init(void)
{
    int ret;
    struct itimerval value,ovalue;

    signal(SIGALRM, time_handler);
    value.it_value.tv_sec = 0;
    value.it_value.tv_usec = 500000;		//500ms
    value.it_interval.tv_sec = 0;
    value.it_interval.tv_usec = 500000;		//500ms
    ret = setitimer(ITIMER_REAL,&value,&ovalue);
    if(ret < 0)
    {
        printf("setitimer failed\n");
    }

    return ret;
}
#else

/**
 * simulation timer
 **/
static void simulate_timer(void)
{
    static int send_ready_times = 0;
    static struct timeval old_tv = {0,0};
    struct timeval cur_tv;
    int time = 0;

    gettimeofday(&cur_tv,0);
    //time = (cur_tv.tv_sec*1000*1000 + cur_tv.tv_usec) - (old_tv.tv_sec*1000*1000 + old_tv.tv_usec);
    time = (cur_tv.tv_sec - old_tv.tv_sec)*1000*1000 + cur_tv.tv_usec - old_tv.tv_usec;
    if(time >= 500*1000)    //500ms
    {
        //printf("time: %d\n",time);
        if(!arm_init_status)	//arm send initilize status to mcu
        {
            printf("arm send initilize ready cmd\n");
            if(++send_ready_times == 3)		// 3 times later, if navigation board initilized uncompletly,then stop and send heartbeat package
            {
                arm_init_status = 1;
                send_ready_times = 0;
            }
            PTHREAD_MUTEX_LOCK(&yxcx_mutex);
            arm_send_cmd(YXCX_CMD_ARM_READY,YXCX_ARM_READY);
            PTHREAD_MUTEX_UNLOCK(&yxcx_mutex);
        }
        else	//arm send heartbeat package to mcu
        {
            PTHREAD_MUTEX_LOCK(&yxcx_mutex);
            arm_send_cmd(YXCX_CMD_ARM_READY,YXCX_HEARTBEAT_PACKAGE);
            PTHREAD_MUTEX_UNLOCK(&yxcx_mutex);

#ifdef YXCX_TEST
            //for test
            if(current_screen != YXCX_SCREEN_STATUS_NAVI) //switch to GPS Screen
            {
                PTHREAD_MUTEX_LOCK(&yxcx_mutex);
                arm_send_cmd(YXCX_CMD_SCR_MODE,YXCX_NAVI_SCREEN);
                PTHREAD_MUTEX_UNLOCK(&yxcx_mutex);
            }
#endif
        }
        old_tv.tv_sec = cur_tv.tv_sec;
        old_tv.tv_usec = cur_tv.tv_usec;
    }
}
#endif

/**
 * uart read
 **/
static int yxcx_touch_command_read(const int fd)
{
#ifdef YXCX_USE_SIMULATION_TIMER
    simulate_timer();
#endif

    if (-1 != fd && touch_command_serial_port_fd == fd)
    {
        if(read(fd, &read_buf[count], 1) == 1)
        {
            //printf("-%x",read_buf[count]);
            if(read_buf[0] == YXCX_PACK_HEAD)	//head
            {
                if(++count > 1)	//skip the head
                {
                    if(read_buf[1] > 0)	//Ensure that the data length id greater than 0( len > 0 )
                    {
                        pack_len = read_buf[1];	//data length

                        if((count == 3) && (!find_cmd_type(read_buf[2])))	//check if cmd type if not exist
                        {
                            if(read_buf[1] == YXCX_PACK_HEAD)   //if read_buf[1] is equal to the value of the head,then read_buf[1] is new head
                            {
                                read_buf[0] = read_buf[1];
                                read_buf[1] = read_buf[2];
                                count--;
                                return 0;
                            }
                            else if(read_buf[2] == YXCX_PACK_HEAD)	//if read_buf[2] is equal to the value of the head,then read_buf[1] is new head
                            {
                                read_buf[0] = read_buf[2];
                                count -= 2;
                                return 0;
                            }
                            else	//abnormal
                            {
                                count = 0;
                                return 0;
                            }
                        }

                        if (count == pack_len+2)	//receive a complete package
                        {
                            unsigned char *buf = read_buf+3;
                            int cmd = read_buf[2];
                            int len = pack_len-2;

                            unsigned char crc = crc_checkout(read_buf+1,pack_len);
                            if(crc == read_buf[pack_len+1])
                            {
                                //printf("\n");
                                data_parse(cmd,buf,len);
                            }
                            else
                            {
                                printf("parse_mcu: crc failed,cal_crc:%x,rcv_crc:%x\n",crc,read_buf[pack_len+1]);
                            }

                            count = 0;
                        }
                    }
                    else
                    {
                        count = 0;
                    }
                }
            }
        }
    }
    else     //read timeout
    {
        if(touch_status != TOUCH_RELEASE)   //if touch do not release, then force release it
        {
            unsigned char buf[6];
            int len = 5;

            buf[0] = YXCX_TOUCH_TIMEOUT;
            buf[1] = 0x0;	//data1
            buf[2] = 0x0;	//data2
            buf[3] = 0x0;	//data3
            buf[4] = 0x0;	//data4
            data_parse(YXCX_CMD_TOUCH2_POS,buf,len);
        }
    }

    return 0;
}

/**
 * uart write
 **/
static int yxcx_write(const struct ArkProtocol* protocol)
{
    enum ArkType type = protocol->type;

    switch(type)
    {
        case AT_Car:
        {
            struct ArkCar *car = (struct ArkCar *)protocol->data;

            switch(car->type)
            {
                case ACT_Display:
                {
                    if(car->status == ACS_CarDisplay)   //car screen on
                    {
                        PTHREAD_MUTEX_LOCK(&yxcx_mutex);
                        arm_send_cmd(YXCX_CMD_SCR_MODE,YXCX_CAR_SCREEN);
                        PTHREAD_MUTEX_UNLOCK(&yxcx_mutex);
                    }
                    else if(car->status == ACS_NaviDisplay) //navigation screen on
                    {
                        PTHREAD_MUTEX_LOCK(&yxcx_mutex);
                        arm_send_cmd(YXCX_CMD_SCR_MODE,YXCX_NAVI_SCREEN);
                        PTHREAD_MUTEX_UNLOCK(&yxcx_mutex);
                    }
                    break;
                }
                default:
                    break;
            }
        }
        default:
            break;
    }
    return 0;
}

/**
 * initilize
 **/
static int yxcx_init()
{
    char* customer_id = (char*)getenv("PROTOCOL_ID");
    if (NULL != customer_id)
    {
        if (0 == strcmp(customer_id, "YXCX"))
        {
            ;
        } else return AC_Undefine;
    } else return AC_Undefine;

    char* touch_serial_port_name = (char*)getenv("TOUCHSERIAL");

    if (NULL != touch_serial_port_name) {
        touch_command_serial_port_fd = open(touch_serial_port_name, O_RDWR);
        if (touch_command_serial_port_fd < 0) {
            fprintf(stderr, "open %s error\n", touch_serial_port_name);
            return AC_Error;
        } else {
            int baudrate = atoi((char*)getenv("TOUCHSERIAL_BAUDRATE"));
            set_speed(touch_command_serial_port_fd, baudrate);
            if (set_parity(touch_command_serial_port_fd, TOUCH_DATA_BITS, TOUCH_STOP_BITS, TOUCH_PARITY_STATUS) == false) {
                printf("uart_touch_init: Set parity error\n");
                return AC_Error;
            }
        }
    }

    yxcx.fd[0] = touch_command_serial_port_fd;

    if (-1 == (input_fd = open(INPUT_DEVICE_PATH, O_WRONLY))) {
        fprintf(stderr, "open %s error\n", INPUT_DEVICE_PATH);
        return AC_Error;
    }

#ifndef YXCX_USE_SIMULATION_TIMER
    ret = arm_timer_init();
#endif

#ifdef YXCX_MUTEX_LOCK
    pthread_mutex_init(&yxcx_mutex,NULL);
#endif

#ifdef YXCX_TEST
    resolution_test();
#else
   //display switch to car by default
    display_muxer_switch(YXCX_SCREEN_INPUT);
    current_screen = YXCX_SCREEN_STATUS_CAR;
#endif

    printf("yxcx_init\n");

    return AC_YXCX;
}

/**
 * uninitilize
 **/
static int yxcx_uninit()
{
#ifdef YXCX_MUTEX_LOCK
    pthread_mutex_destroy(&yxcx_mutex);
#endif

    if (-1 != touch_command_serial_port_fd) {
        close(touch_command_serial_port_fd);
        touch_command_serial_port_fd = -1;
    }
    if (-1 != input_fd) {
        close(input_fd);
        input_fd = -1;
    }

    return 0;
}

struct CustomerProtocol yxcx = {
    .customer = AC_YXCX,
    .fd = {-1, -1, -1},
    .init = yxcx_init,
    .read = yxcx_touch_command_read,
    .write = yxcx_write,
    .uninit = yxcx_uninit,
};


/*********************************************************************************************************************************
 *  match different resolution
 *********************************************************************************************************************************/
enum screen_type_id {
    SCREEN_TYPE_RGB,
    SCREEN_TYPE_LVDS,
    SCREEN_TYPE_ITU601,
    SCREEN_TYPE_TVENC,
    SCREEN_TYPE_CVBS,
    SCREEN_TYPE_VGA,
    SCREEN_TYPE_YPBPR,
    SCREEN_TYPE_ITU656,
    SCREEN_TYPE_NONE = -1,
};
enum rgb_format_id {
    RGB_FORMAT_RGB0888,
    RGB_FORMAT_RGB565,
    RGB_FORMAT_NONE = -1,
};

enum lvds_format_id {
    LVDS_FORMAT_RGB0888,
    LVDS_FORMAT_RGB565,
    LVDS_FORMAT_NONE = -1,
};
enum cvbs_format_id {
    CVBS_FORMAT_PAL,
    CVBS_FORMAT_NTSC,
    CVBS_FORMAT_NONE = -1,
};
enum vga_format_id {
    VGA_FORMAT_800x480,
    VGA_FORMAT_800x600,

    VGA_FORMAT_1024x768HZ60,
    VGA_FORMAT_1280x1024HZ60,
    VGA_FORMAT_1900x1200HZ60,
    VGA_FORMAT_1280x1024HZ75,
    VGA_FORMAT_1280x960HZ85,
    VGA_FORMAT_1280x720HZ60,
    VGA_FORMAT_640x480HZ60,
    VGA_FORMAT_NONE = -1,
};
enum ypbpr_format_id {
    YPBPR_FORMAT_480I,
    YPBPR_FORMAT_576I,
    YPBPR_FORMAT_480P,
    YPBPR_FORMAT_576P,
    YPBPR_FORMAT_720P60HZ,
    YPBPR_FORMAT_720P50HZ,
    YPBPR_FORMAT_1080I60HZ,
    YPBPR_FORMAT_1080I50HZ,
    YPBPR_FORMAT_1080I50HZ_1250,
    YPBPR_FORMAT_1080P60HZ,
    YPBPR_FORMAT_1080P50HZ,
    YPBPR_FORMAT_NONE = -1,
};
enum ark_disp_tvenc_out_mode {
    ARKDISP_TVENC_OUT_YPBPR_I480HZ60       = 0,
    ARKDISP_TVENC_OUT_YPBPR_I576HZ50       = 1,
    ARKDISP_TVENC_OUT_YPBPR_P480HZ60       = 2,
    ARKDISP_TVENC_OUT_YPBPR_P576HZ50       = 3,
    ARKDISP_TVENC_OUT_YPBPR_P720HZ60       = 4,
    ARKDISP_TVENC_OUT_YPBPR_P720HZ50       = 5,
    ARKDISP_TVENC_OUT_YPBPR_I1080HZ60      = 6,
    ARKDISP_TVENC_OUT_YPBPR_I1080HZ50      = 7,
    ARKDISP_TVENC_OUT_YPBPR_I1080HZ50_1250 = 8,
    ARKDISP_TVENC_OUT_YPBPR_P1080HZ60      = 9,
    ARKDISP_TVENC_OUT_YPBPR_P1080HZ50      = 10,
    ARKDISP_TVENC_OUT_CVBS_PAL             = 11,
    ARKDISP_TVENC_OUT_CVBS_NTSC            = 12,
    ARKDISP_TVENC_OUT_ITU656_PAL           = 13,
    ARKDISP_TVENC_OUT_ITU656_NTSC          = 14,
    ARKDISP_TVENC_OUT_VGA_640x480HZ60      = 15,
    ARKDISP_TVENC_OUT_VGA_800x600HZ60      = 16,
    ARKDISP_TVENC_OUT_VGA_1024x768HZ60     = 17,
    ARKDISP_TVENC_OUT_VGA_1280x1024HZ60    = 18,
    ARKDISP_TVENC_OUT_VGA_1900x1200HZ60    = 19, // bandwidth  limit
    ARKDISP_TVENC_OUT_VGA_1280x1024HZ75    = 20,
    ARKDISP_TVENC_OUT_VGA_1280x960HZ85     = 21, // bandwidth  limit
    ARKDISP_TVENC_OUT_VGA_1280x720HZ60     = 22,
};
enum screen_id {
    SCREEN_QUN700,
    SCREEN_CVBS_NTSC,
    SCREEN_CVBS_PAL,
    SCREEN_VGA8060,
    SCREEN_TYPE_YPBPR720P,
    SCREEN_C101EAN,
    SCREEN_CLAA101,
    SCREEN_GM8284DD,
    SCREEN_USER_TYPE,
    SCREEN_MAX_NUM,
};
enum clk_source_id {
    SCREEN_CLKSEL_CPUPLL,
    SCREEN_CLKSEL_SYSPLL,
    SCREEN_CLKSEL_DDSCLK,
    SCREEN_CLKSEL_24MCLK = 4,
    SCREEN_CLKSEL_NONE = -1,
};
enum rgb_mode_id {
    RGB_MODE_BGR,
    RGB_MODE_GBR,
    RGB_MODE_RBG,
    RGB_MODE_BRG,
    RGB_MODE_GRB,
    RGB_MODE_RGB,
    RGB_MODE_NUM,
};
struct screen_info
{
    int screen_id;
    enum screen_type_id screen_type;
    int format;
    unsigned int width;
    unsigned int height;
    unsigned int vbp;
    unsigned int vfp;
    unsigned int vsw;
    unsigned int hbp;
    unsigned int hfp;
    unsigned int hsw;
    unsigned int vclk_active;
    unsigned int hsync_active;
    unsigned int vsync_active;
    unsigned int de_active;
    unsigned int tvenc;
    unsigned int tvout_format;

    unsigned int clk_source;
    unsigned int clk_freq;
    unsigned int clk_div1;
    unsigned int clk_div2;

    unsigned int src_format;
    unsigned int rgb_mode;
    unsigned int bpp;
    unsigned int lvds_cfg;
    unsigned int interlace;

    unsigned int frame_rate;
    unsigned int src_width;
    unsigned int src_height;
    unsigned int pad_unset;
};
typedef struct _VP_INFO
{
    unsigned int video_contrast;
    unsigned int video_brightness;
    unsigned int video_saturation;
    unsigned int video_hue;

    unsigned int video2_contrast;
    unsigned int video2_brightness;
    unsigned int video2_saturation;
    unsigned int video2_hue;

    unsigned int osd1_contrast;
    unsigned int osd1_brightness;
    unsigned int osd1_saturation;
    unsigned int osd1_hue;

    unsigned int osd2_contrast;
    unsigned int osd2_brightness;
    unsigned int osd2_saturation;
    unsigned int osd2_hue;

    unsigned int osd3_contrast;
    unsigned int osd3_brightness;
    unsigned int osd3_saturation;
    unsigned int osd3_hue;

}vp_info;

typedef struct _itu656in
{
    unsigned int ModeControl;
    unsigned int VGATE_DELAY;
    unsigned int DEN_V_STOP;
    unsigned int DEN_V_START;

    unsigned int NTSC_TVGDEL;
    unsigned int NTSC_TVSYNC;
    unsigned int NTSC_THGDEL;
    unsigned int NTSC_THSYNC;

    unsigned int NTSC_THLEN;
    unsigned int NTSC_THGATE;

    unsigned int NTSC_TVLEN;
    unsigned int NTSC_TVGATE;

    unsigned int NTSC_VFZ;
    unsigned int NTSC_HFZ;

    unsigned int NTSC_SYNC_UP_CNT;
    unsigned int NTSC_SYNC_DOWN_CNT;
    unsigned int NTSC_DATENA_INV;
    unsigned int NTSC_VSYNC_INV;
    unsigned int NTSC_HSYNC_INV;
    unsigned int NTSC_FIELD_INV;
    unsigned int NTSC_HV_DELAY;

    unsigned int PAL_TVGDEL;
    unsigned int PAL_TVSYNC;
    unsigned int PAL_THGDEL;
    unsigned int PAL_THSYNC;

    unsigned int PAL_THLEN;
    unsigned int PAL_THGATE;

    unsigned int PAL_TVLEN;
    unsigned int PAL_TVGATE;

    unsigned int PAL_VFZ;
    unsigned int PAL_HFZ;

    unsigned int PAL_SYNC_UP_CNT;
    unsigned int PAL_SYNC_DOWN_CNT;
    unsigned int PAL_DATENA_INV;
    unsigned int PAL_VSYNC_INV;
    unsigned int PAL_HSYNC_INV;
    unsigned int PAL_FIELD_INV;
    unsigned int PAL_HV_DELAY;

}itu656byp_info;
typedef struct _tGammaInfo
{
    unsigned int gamma_en;
    unsigned int gamma_regval[48];

} gamma_info;
typedef  struct _display_updatepara
{
    unsigned char flag_id[16];
    struct screen_info screeninfo;
    unsigned int  flag_vpinfo;
    unsigned int  flag_gamma;
    vp_info		  vpinfo;
    gamma_info    gammainfo;
    itu656byp_info itu656bypinfo;
}display_updatepara;

#define INI_NODE_MAX    200
struct ini_node
{
    char *name;
    int value;
};
struct ini_parser
{
    struct ini_node node[INI_NODE_MAX];
    int count;
    const char *src_file;
    const char *dest_file;
};

#define LCD_MODULE_HUA350       (0)    // 3.5" QVGA RGB16 (320x240)
#define LCD_MODULE_SAM350       (1)    // 3.5" QVGA RGB16 (320x240)
#define LCD_MODULE_QIM350       (2)    // 3.5" QVGA RGB16 (320x240)
#define LCD_MODULE_SHP430       (3)    // 4.3" WVGA RGB16 (480x272)
#define LCD_MODULE_QUN700       (4)    // 7.0" WVGA RGB16 (800x480)
#define LCD_MODULE_VGA8048      (5)    // VGA  RGB16 (800x480)
#define LCD_MODULE_VGA8060      (6)    // VGA  RGB16 (800x600)
#define LCD_MODULE_ITU656PAL    (7)    //
#define LCD_MODULE_ITU656NTSC   (8)   //
#define LCD_MODULE_CLAA101		(9)	   // 10.1" WSVGA (1024x600)
#define LCD_MODULE_Unk131       (10)   // 13.1" WSVGA (1366x768)
#define LCD_MODULE_R601			(11)

#define TVOUT_NONE				(0)
#define TVOUT_COMPOSITE			(1)
#define TVOUT_YPBPR				(2)
#define TVOUT_VGA				(3)
#define TVOUT_ITU656			(4)

#define DISPLAY_TYPE_RGB565		(1<<0)
#define DISPLAY_TYPE_RGB888		(1<<1)
#define DISPLAY_TYPE_LVDS		(1<<2)
#define DISPLAY_TYPE_VGA		(1<<3)
#define DISPLAY_TYPE_CVBS		(1<<4)
#define DISPLAY_TYPE_YPBPR		(1<<5)
#define DISPLAY_TYPE_ITU656		(1<<6)
#define DISPLAY_TYPE_ITU601		(1<<7)

#define SCREEN_TYPE_TVOUT		(SCREEN_TYPE_VGA | SCREEN_TYPE_CVBS | SCREEN_TYPE_YPBPR | SCREEN_TYPE_ITU656)

#define BSP_DISP_RGB_565			5
#define BSP_DISP_RGB_888			7

#define BSP_DISP_BGR				0
#define BSP_DISP_RGB				5

#define LCD_CLKSEL_CPUPLL		0
#define LCD_CLKSEL_SYSPLL		1
#define LCD_CLKSEL_DDSCLK		2
#define LCD_CLKSEL_24MCLK		4

#define GAMMA_REG_MAX		    48
#define VP_DATA_MAX		        20
#define VP_INFO_FLAG		    20
#define GAMMA_INFO_FLAG		    48
#define ITU656_DATA_MAX		    38

static char *gamma_name[GAMMA_REG_MAX]=
{
    "Gamma0",  "Gamma1",  "Gamma2",  "Gamma3",  "Gamma4",  "Gamma5",  "Gamma6",  "Gamma7",
    "Gamma8",  "Gamma9",  "Gamma10", "Gamma11", "Gamma12", "Gamma13", "Gamma14", "Gamma15",
    "Gamma16", "Gamma17", "Gamma18", "Gamma19", "Gamma20", "Gamma21", "Gamma22", "Gamma23",
    "Gamma24", "Gamma25", "Gamma26", "Gamma27", "Gamma28", "Gamma29", "Gamma30", "Gamma31",
    "Gamma32", "Gamma33", "Gamma34", "Gamma35", "Gamma36", "Gamma37", "Gamma38", "Gamma39",
    "Gamma40", "Gamma41", "Gamma42", "Gamma43", "Gamma44", "Gamma45", "Gamma46", "Gamma47",
};
static char *vp_name[VP_DATA_MAX]=
{
    "videoContrast",  "videoBrightness",  "videoSaturation",  "videoHue",
    "video2Contrast", "video2Brightness", "video2Saturation", "video2Hue",
    "osd1Contrast",   "osd1Brightness",   "osd1Saturation",    "osd1Hue",
    "osd2Contrast",   "osd2Brightness",   "osd2Saturation",    "osd2Hue",
    "osd3Contrast",   "osd3Brightness",   "osd3Saturation",    "osd3Hue",
};

static char *itu656_name[ITU656_DATA_MAX]=
{
    "ModeControl",     "VGATE_DELAY",      "DEN_V_STOP",     "DEN_V_START",
    "NTSC_TVGDEL",     "NTSC_TVSYNC",      "NTSC_THGDEL",    "NTSC_THSYNC",    "NTSC_THLEN",
    "NTSC_THGATE",     "NTSC_TVLEN",       "NTSC_TVGATE",    "NTSC_VFZ",       "NTSC_HFZ",
    "NTSC_SYNC_UP",	   "NTSC_SYNC_DOWN",   "NTSC_DATENA_INV","NTSC_VSYNC_INV", "NTSC_HSYNC_INV", "NTSC_FIELD_INV", "NTSC_HV_DELAY",
    "PAL_TVGDEL",      "PAL_TVSYNC",       "PAL_THGDEL",     "PAL_THSYNC",     "PAL_THLEN",
    "PAL_THGATE",      "PAL_TVLEN",        "PAL_TVGATE",     "PAL_VFZ",        "PAL_HFZ",
    "PAL_SYNC_UP",     "PAL_SYNC_DOWN",    "PAL_DATENA_INV", "PAL_VSYNC_INV",  "PAL_HSYNC_INV",  "PAL_FIELD_INV",  "PAL_HV_DELAY",
};

#if 0
static struct screen_info screens_default [] = {
    {SCREEN_QUN700, SCREEN_TYPE_RGB, LVDS_FORMAT_RGB0888, 800, 480, 40, 36, 16, 32, 32, 41, 0, 1, 1, 0, 0x8, 12,  SCREEN_CLKSEL_SYSPLL,0,13,1,  DISP_RGB_888,RGB_MODE_BGR,32,0,0, 0,0,0,0},
    {SCREEN_CVBS_NTSC, SCREEN_TYPE_CVBS, CVBS_FORMAT_NTSC, 720, 480, 0, 0, 22, 0, 0, 136, 0, 0, 0, 0, 0x1F,0,     0,0,0,0,  DISP_RGB_888,RGB_MODE_BGR,32,0,1, 0,0,0,0},
    {SCREEN_CVBS_PAL, SCREEN_TYPE_CVBS, CVBS_FORMAT_PAL, 720, 576, 0, 0, 24, 0, 0, 142, 0, 0, 0, 0, 0x1F,0,       0,0,0,0,  DISP_RGB_888,RGB_MODE_BGR,32,0,1, 0,0,0,0},
    {SCREEN_VGA8060, SCREEN_TYPE_VGA, VGA_FORMAT_800x600, 800, 600, 23, 1, 4, 80, 40, 128, 0, 0, 0, 0, 0x1F,0,    SCREEN_CLKSEL_DDSCLK,200,5,0, DISP_RGB_888,RGB_MODE_BGR,32,0,0, 0,0,0,0},
    {SCREEN_TYPE_YPBPR720P, SCREEN_TYPE_YPBPR, YPBPR_FORMAT_720P60HZ, 1280, 720, 0, 0, 30, 0, 0, 370, 0, 0, 0, 0, 0x1F,0,  SCREEN_CLKSEL_DDSCLK,297,4,0, DISP_RGB_888,RGB_MODE_BGR,32,0,0, 0,0,0,0},
    {SCREEN_C101EAN, SCREEN_TYPE_LVDS, LVDS_FORMAT_RGB0888, 1280, 720, 14, 14, 10, 60, 60, 46, 0, 1, 1, 0, 0x8, 12,        SCREEN_CLKSEL_SYSPLL,0,6,1,  DISP_RGB_888,RGB_MODE_BGR,32,0xE0EC,0, 0,0,0,0},
    {SCREEN_CLAA101, SCREEN_TYPE_LVDS, LVDS_FORMAT_RGB0888, 1024, 600, 10, 10, 16, 100, 100, 120, 0, 1, 1, 0, 0x8, 12,     SCREEN_CLKSEL_SYSPLL,0,4,2,  DISP_RGB_888,RGB_MODE_BGR,32,0x160FD,0, 0,0,0,0},
    {0},
    {0},//SCREEN_USER_TYPE
};
#endif
static vp_info vpinfo_default = {
    0x80,0x80,0x40,0x00,
    0x80,0x80,0x40,0x00,
    0x80,0x80,0x40,0x00,
    0x80,0x80,0x40,0x00,
    0x80,0x80,0x40,0x00,
};

//SCREEN_QUN700
static itu656byp_info itu656byp_default = {
    0x00001D80,0,10,10,
    2,7,28,22,948, 800,525,480,516,916,4,3,0,0,0,0,1,
    2,2, 2, 2,1168,800,520,480,618,908,4,3,0,0,0,0,1,
};
//SCREEN_CLAA101
static itu656byp_info itu656byp_default2 = {
    0x00001D80,0,10,10,
    2,7,28,22,1242, 1024,635,600,413,716,4,3,0,0,0,0,1,
    2,2, 2, 2,1515, 1024,635,600,494,710,4,3,0,0,0,0,1,
};


static int valid_flag = 0;
static int valid_flag_itu656 = 0;
static int valid_flag_vp = 0;
static int valid_flag_gamma = 0;
static char id_flag[16] = {"LcdArgInFlash"};
static vp_info  g_vp;
static struct screen_info g_screeninfo;
static gamma_info g_gammainfo;
static itu656byp_info g_itu656info;
static display_updatepara g_display_para;

#if 0   //read resolution parameters from configure file(saved advance)
static int get_screen_resolution(void)
{
    FILE *fp;
    char ar[20];
    int size,i;
    int screen_resolution = RESOLUTION_UNDIFINE;

    fp = fopen("/etc/screen/resolution","r");
    if(fp == NULL)
        return screen_resolution;

    //obtain file size
    fseek(fp, 0, SEEK_END);
    size = ftell(fp);   //return current offset
    rewind(fp);

    //drop '\0'
    for(i=0; i<size; i++)
    {
        if(ar[i] == '\0' || ar[i] == '\n')
            break;
    }
    size = i;

    fread(ar,1,size,fp);

    if(strncmp(ar,"800x480",size) == 0)
    {
        screen_resolution = RESOLUTION_800X480;
    }
    else if(strncmp(ar,"1280x480",size) == 0)
    {
        screen_resolution = RESOLUTION_1280X480;
    }
    else if(strncmp(ar,"1024x480",size) == 0)
    {
        screen_resolution = RESOLUTION_1024X480;
    }
    else if(strncmp(ar,"640x480",size) == 0)
    {
        screen_resolution = RESOLUTION_640X480;
    }

    fclose(fp);

    return screen_resolution;
}
#else   //read resolution parameters from nand flash
static int get_screen_resolution(void)
{
    display_updatepara para;
    int screen_resolution = RESOLUTION_UNDIFINE;
    char sys_cmd[100];
    int w,h;

    sprintf(sys_cmd,"dd if=/dev/mtdblock7 of=/tmp/yxcx_arkdata_ini bs=%d count=1",sizeof(display_updatepara));
    system(sys_cmd);

    FILE *fp = fopen("/tmp/yxcx_arkdata_ini","r");
    if(fp == NULL)
    {
        system("rm /tmp/yxcx_arkdata_ini");
        printf("## err: fopen /tmp/yxcx_arkdata_ini failed\n");
        return screen_resolution;
    }

    fread(&para,1,sizeof(display_updatepara),fp);

    w = para.screeninfo.width;
    h = para.screeninfo.height;
    //printf("### w:%d,h:%d\n",w,h);

    if(w == 800 && h == 480)
        screen_resolution = RESOLUTION_800X480;
    else if(w == 1280 && h == 480)
        screen_resolution = RESOLUTION_1280X480;
    else if(w == 1024 && h == 480)
        screen_resolution = RESOLUTION_1024X480;
    else if(w == 640 && h == 480)
        screen_resolution = RESOLUTION_640X480;

    fclose(fp);
    system("rm -rf /tmp/yxcx_arkdata_ini");

    return screen_resolution;
}
#endif

static void check_display_interface(void)
{
    int i = 0;
    int iValue = 0;

    //1. Check_ScreenType
    for( i = 0; i < 8; i++)
    {
        if(g_screeninfo.screen_type == (1 << i))
        {
            iValue = 1;
            break;
        }
    }

    if (iValue != 1)
    {
        printf("get ScreenType error!\n");
        valid_flag =  -1;
    }

    //2. Check_Format
    if (g_screeninfo.src_format == BSP_DISP_RGB_565)
    {
        g_screeninfo.bpp = 16;
    }
    else if (g_screeninfo.src_format == BSP_DISP_RGB_888)
    {
        g_screeninfo.bpp = 32;
    }
    else
    {
        valid_flag = -1;
        printf("get Format error!\n");
    }

    //3. Check_RgbMode
    if (g_screeninfo.rgb_mode != BSP_DISP_BGR && g_screeninfo.rgb_mode != BSP_DISP_RGB)
    {
        valid_flag = -1;
        printf("get RgbMode error!\n");
    }

    //4. Check_TvoutType   0-22 different to wince
    if (g_screeninfo.tvout_format < ARKDISP_TVENC_OUT_YPBPR_I480HZ60 || g_screeninfo.tvout_format > ARKDISP_TVENC_OUT_VGA_1280x720HZ60)
    {
        valid_flag = -1;
        printf("get TvoutType error!\n");
    }

    //5.check interlace
    if(g_screeninfo.screen_type==DISPLAY_TYPE_YPBPR||g_screeninfo.screen_type==DISPLAY_TYPE_CVBS||g_screeninfo.screen_type==DISPLAY_TYPE_ITU656)
    {
        if (g_screeninfo.interlace < 0 || g_screeninfo.interlace > 1)
        {
            valid_flag = -1;
            printf("get interlace error!\n");
        }
    }
    else
    {
        g_screeninfo.interlace = 0;
    }

    //6.tvenc
    if (g_screeninfo.screen_type >= DISPLAY_TYPE_VGA  && g_screeninfo.screen_type <= DISPLAY_TYPE_ITU656)
    {
        g_screeninfo.tvenc = 0x1f;
        g_screeninfo.tvout_format = 0;
        printf("set TvoutType=0 \n"  );
    }
    else
    {
        g_screeninfo.tvenc = 0x8;
    }

    //6.screen type and sub format  set
    if (g_screeninfo.screen_type == DISPLAY_TYPE_RGB565)
    {
        g_screeninfo.screen_type = SCREEN_TYPE_RGB;
        g_screeninfo.format= RGB_FORMAT_RGB565;
    }

    if (g_screeninfo.screen_type == DISPLAY_TYPE_RGB888)
    {
        g_screeninfo.screen_type = SCREEN_TYPE_RGB;
        g_screeninfo.format= RGB_FORMAT_RGB0888;
    }

    if (g_screeninfo.screen_type == DISPLAY_TYPE_LVDS)
    {
        g_screeninfo.screen_type = SCREEN_TYPE_LVDS;
        g_screeninfo.format= RGB_FORMAT_RGB0888;
    }

    if (g_screeninfo.screen_type == DISPLAY_TYPE_VGA)
    {
        g_screeninfo.screen_type = SCREEN_TYPE_VGA;

        if (g_screeninfo.width == 640 && g_screeninfo.height == 480)
        {
            g_screeninfo.format= VGA_FORMAT_640x480HZ60;
        }
        else if (g_screeninfo.width == 800 && g_screeninfo.height == 600)
        {
            g_screeninfo.format= VGA_FORMAT_800x600;
        }
        else if (g_screeninfo.width == 1024 && g_screeninfo.height == 768)
        {
            g_screeninfo.format= VGA_FORMAT_1024x768HZ60;
        }
        else if (g_screeninfo.width == 1280 && g_screeninfo.height == 1024)
        {
            if(g_screeninfo.clk_freq > 250000000)
                g_screeninfo.format= VGA_FORMAT_1280x1024HZ75;
            else
                g_screeninfo.format= VGA_FORMAT_1280x1024HZ60;
        }
        else if (g_screeninfo.width == 1900 && g_screeninfo.height == 1200)
        {
            g_screeninfo.format= VGA_FORMAT_1900x1200HZ60;
        }
        else if (g_screeninfo.width == 1280 && g_screeninfo.height == 960)
        {
            g_screeninfo.format= VGA_FORMAT_1280x960HZ85;
        }
        else if (g_screeninfo.width == 1280 && g_screeninfo.height == 720)
        {
            g_screeninfo.format= VGA_FORMAT_1280x720HZ60;
        }
        else if (g_screeninfo.width == 800 && g_screeninfo.height == 480)
        {
            g_screeninfo.format= VGA_FORMAT_800x480;
        }
    }

    if (g_screeninfo.screen_type == DISPLAY_TYPE_CVBS)
    {
        g_screeninfo.screen_type = SCREEN_TYPE_CVBS;

        if (g_screeninfo.width == 720 && g_screeninfo.height == 480)
            g_screeninfo.format= CVBS_FORMAT_NTSC;
        else
            g_screeninfo.format= CVBS_FORMAT_PAL;
    }

    if (g_screeninfo.screen_type == DISPLAY_TYPE_YPBPR)
    {
        g_screeninfo.screen_type = SCREEN_TYPE_YPBPR;

        if (g_screeninfo.width == 720 && g_screeninfo.height == 480 && g_screeninfo.interlace == 1)
        {
            g_screeninfo.format= YPBPR_FORMAT_480I;
        }
        else if(g_screeninfo.width == 720 && g_screeninfo.height == 576 && g_screeninfo.interlace == 1)
        {
            g_screeninfo.format= YPBPR_FORMAT_576I;
        }
        else if (g_screeninfo.width == 720 && g_screeninfo.height == 480 && g_screeninfo.interlace == 0)
        {
            g_screeninfo.format= YPBPR_FORMAT_480P;
        }
        else if(g_screeninfo.width == 720 && g_screeninfo.height == 576 && g_screeninfo.interlace == 0)
        {
            g_screeninfo.format= YPBPR_FORMAT_576P;
        }
        else if(g_screeninfo.width == 1280 && g_screeninfo.height == 720 && g_screeninfo.interlace == 0)
        {
            if(g_screeninfo.hsw < 550)
                g_screeninfo.format = YPBPR_FORMAT_720P60HZ;
            else
                g_screeninfo.format = YPBPR_FORMAT_720P50HZ;
        }
        else if(g_screeninfo.width == 1920 && g_screeninfo.height == 1080 && g_screeninfo.interlace == 1)
        {
            if(g_screeninfo.hsw < 360)
                g_screeninfo.format = YPBPR_FORMAT_1080I60HZ;
            else if(g_screeninfo.hsw > 600)
                g_screeninfo.format = YPBPR_FORMAT_1080I50HZ;
            else
                g_screeninfo.format = YPBPR_FORMAT_1080I50HZ_1250;
        }
        else if(g_screeninfo.width == 1920 && g_screeninfo.height == 1080 && g_screeninfo.interlace == 0)
        {
            if(g_screeninfo.hsw < 500)
                g_screeninfo.format = YPBPR_FORMAT_1080P60HZ;
            else
                g_screeninfo.format = YPBPR_FORMAT_1080P50HZ;
        }

    }

    if (g_screeninfo.screen_type == DISPLAY_TYPE_ITU656)
    {
        g_screeninfo.screen_type = SCREEN_TYPE_ITU656;

        if (g_screeninfo.width == 720 && g_screeninfo.height == 480)
            g_screeninfo.format= CVBS_FORMAT_NTSC;
        else
            g_screeninfo.format= CVBS_FORMAT_PAL;
    }

    if (g_screeninfo.screen_type == DISPLAY_TYPE_ITU601)
    {
        g_screeninfo.screen_type = SCREEN_TYPE_ITU601;
        g_screeninfo.format= RGB_FORMAT_RGB0888;
    }

    if (g_screeninfo.pad_unset!= 1)
        g_screeninfo.pad_unset = 0;


    printf("convert success, screen_type=%d, subformat=%d, tvenc=%d.\r\n",
    g_screeninfo.screen_type,g_screeninfo.format,g_screeninfo.tvenc);
}

static void check_timming(void)
{
    if(g_screeninfo.width > 0x1FFF || g_screeninfo.width <= 0)
    {
        valid_flag = -1;
        printf("get Width  >0x1fff or <=0 ,error!\n");
    }

    if(g_screeninfo.height > 0x7FF || g_screeninfo.height <= 0)
    {
        valid_flag = -1;
        printf("get Height >0x7ff or <=0 ,error!\n");
    }

    if(g_screeninfo.src_width > 0x1FFF || g_screeninfo.src_width < 0)
    {
        valid_flag = -1;
        printf("get SrcWidth  >0x1fff or <0 ,error!\n");
    }

    if(g_screeninfo.src_height > 0x7FF || g_screeninfo.src_height < 0)
    {
        valid_flag = -1;
        printf("get SrcHeight >0x7FF or <0 ,error!\n");
    }

    if(g_screeninfo.hsw> 0x3FF ||g_screeninfo.hfp> 0x3FF||g_screeninfo.hbp> 0x3FF||\
        g_screeninfo.vfp> 0x3FF || g_screeninfo.vbp> 0x3FF)
    {
        valid_flag = -1;
        printf("get HSW,HFP,HBP,VBP or VFP > 0x3ff, error!\n");
    }
    if(g_screeninfo.vsw > 0x3F)
    {
        valid_flag = -1;
        printf("get VSW > 0x3f,error!\n");
    }
    //1. Check HSYNCPolarity
    if (g_screeninfo.hsync_active < 0 || g_screeninfo.hsync_active > 1)
    {
        valid_flag = -1;
        printf("get HSYNCPolarity error!\n");
    }

    //2. Check VSYNCPolarity
    if (g_screeninfo.vsync_active < 0 || g_screeninfo.vsync_active > 1)
    {
        valid_flag = -1;
        printf("get VSYNCPolarity error!\n");
    }

    //3. Check DEPolarity
    if (g_screeninfo.de_active < 0 || g_screeninfo.de_active > 1)
    {
        valid_flag = -1;
        printf("get DEPolarity error!\n");
    }
}

static void check_clockdata(void)
{
    //1, Check CLKSource
    if (g_screeninfo.clk_source < 0 || g_screeninfo.clk_source > LCD_CLKSEL_24MCLK || g_screeninfo.clk_source == 3)
    {
        valid_flag = -1;
        printf("get CLKSource error!\n");
    }

    // 2.Check CLKPolarity
    if (g_screeninfo.vclk_active < 0 || g_screeninfo.vclk_active > 1)
    {
        valid_flag = -1;
        printf("get CLKPolarity error!\n");
    }

    //3. Check CLKDIV1 ,  Check CLKDIV2
    if(g_screeninfo.screen_type == SCREEN_TYPE_VGA || g_screeninfo.screen_type == SCREEN_TYPE_YPBPR || \
        g_screeninfo.screen_type == SCREEN_TYPE_CVBS || g_screeninfo.screen_type == SCREEN_TYPE_ITU656 )
    {

        if (g_screeninfo.clk_div1 > 47 )
        {
            valid_flag = -1;
            printf("get CLKDIV1 error!\n");
        }

        g_screeninfo.clk_div2 = 0;

    }
    else
    {
        if (g_screeninfo.clk_div1 > 15)
        {
            valid_flag = -1;
            printf("get CLKDIV1 error!\n");
        }

        if (g_screeninfo.clk_div2 > 7)
        {
            valid_flag = -1;
            printf("get CLKDIV2 error!\n");
        }
    }

    g_screeninfo.clk_freq /= 1000000;
    if(g_screeninfo.clk_freq > 1)
        printf("get clk_freq=%d(MHZ)\n",g_screeninfo.clk_freq);
}

static void check_vp(void)
 {
    unsigned int * value_pos = &g_vp.video_contrast;
    char ** name_pos = vp_name;
    unsigned int value = 0;
    unsigned int count = 0;
    int i = 0;

    for (i = 0;i < VP_DATA_MAX;i++)
    {
        value = *value_pos;
        //EdbgOutputDebugString("%s=%d, ",*name_pos,value);
        if (value < 0 || value > 0xFF)
        {
            valid_flag_vp = -1;
            printf("get %s > 255 ,error!\n",*name_pos);
        }
        else if(value == 0)
        {
            count++;
        }
        value_pos++;
        name_pos++;
    }

    if(count >= VP_DATA_MAX)
    {
        valid_flag_vp = -1;
        printf("all vp data 0, error!\n");
    }
}

static void check_gamma(void)
{
    unsigned int *value_pos = (unsigned int *)&g_gammainfo.gamma_regval;
    char **name_pos = gamma_name;
    unsigned int count = 0;
    int i = 0;

    if(g_gammainfo.gamma_en == 3)
    {
        for (i = 0;i < GAMMA_REG_MAX;i++)
        {
            if(*value_pos == 0)count++;
            //EdbgOutputDebugString("%s=0x%x \n",*name_pos,*value_pos);
            value_pos++;
            name_pos++;
        }

        if(count >= GAMMA_REG_MAX)
        {
            printf("Gamma_en=3, But all gamma data 0, error!\n");
            valid_flag_gamma = -1;
        }

    }
    else
    {
        valid_flag_gamma = -1;
        printf("get Gamma_en != 3\n");
    }
}

static void check_itu656data(void)
{
    unsigned int * value_pos = NULL;
    char **name_pos;
    int i;

    value_pos = &g_itu656info.ModeControl + 1;
    name_pos  = itu656_name + 1;
    for (i = 1;i < ITU656_DATA_MAX;i++)
    {
        if((i <= 7) || ( i >= 20 && i <= 24) || i == 37)
        {
            if(*value_pos < 0 || *value_pos > 0xFF)
            {
                printf("%s=%d >0xff or <0, error!\n",*name_pos,*value_pos);
                valid_flag_itu656 = -1;
            }
        }
        else if((i >= 8 && i <= 13) || (i >= 25 && i <= 30))
        {
            if(*value_pos > 0xFFFF || *value_pos <= 0)
            {
                printf("%s=%d >0xffff or <=0, error! \n",*name_pos,*value_pos);
                valid_flag_itu656 = -1;
            }
        }
        else if((i >= 14 && i <= 15) || (i >= 31 && i <= 32) )
        {
            if(*value_pos > 7 || *value_pos < 0)
            {
                printf("%s=%d >7 or <0, error!\n",*name_pos,*value_pos);
                valid_flag_itu656 = -1;
            }
        }
        else if((i >= 16 && i <= 19) || (i >= 33 && i <= 36) )
        {
            if(*value_pos > 1 || *value_pos < 0)
            {
                printf("%s=%d >1 or <0, error!\n",*name_pos,*value_pos);
                valid_flag_itu656 = -1;
            }
        }

        value_pos++;
        name_pos++;
    }
}

//convert string to int
static  int ini_hex_atoi(char * pstr)
{
    int ret_integer = 0;
    int integer_sign = 1;
    int tmp = 0;

    if(pstr == NULL)
    {
        //printf("Pointer is NULL\n");
        return 0;
    }

    if(*pstr == '-')
    {
        integer_sign = -1;
    }
    if(*pstr == '-' || *pstr == '+')
    {
        pstr++;
    }

    if ('0' == *pstr++)
    {
        if ( 'x' == *pstr  || 'X' == *pstr )
        {
            pstr++;
        }
        else
        {
            return 0;
        }

    }
    else
    {
        return 0;
    }

    while((*pstr >= '0' &&  *pstr <= '9')|| (*pstr >= 'a' && *pstr <= 'f')||(*pstr >= 'A' && *pstr <= 'F'))
    {

        if (*pstr >= '0' && *pstr <= '9')
        {
            tmp = *pstr - '0';
        }
        else if (*pstr >= 'a' && *pstr <= 'f')
        {
            tmp = *pstr - 'a'+10;
        }
        else if (*pstr >= 'A' && *pstr <= 'F')
        {
            tmp = *pstr - 'A'+10;
        }
        ret_integer = ret_integer * 16 + tmp;
        pstr++;
    }
    ret_integer = integer_sign * ret_integer;

    return ret_integer;
}
static int ini_atoi(char* pstr)
{
    int intVal = 0;
    int sign = 1;// 符号, 正数为 1, 负数为 -1

    if(pstr == 0)
        return 0;

    // 跳过前面的空格字符 ' '
    while(' '== *pstr)
        pstr++;

    if('-'==*pstr)
        sign = -1;// 判断正负号

    if('-'==*pstr || '+'==*pstr)
        pstr++;

    while(*pstr >= '0' && *pstr <= '9')
    {
        intVal = intVal * 10 + (((int)*pstr)-0x30);
        pstr++;
    }

    return intVal * sign;
}

static int parse_line(struct ini_parser *ini, char *line, int len)
{
    char *p = line;
    char *start = line;
    char *end = NULL;
    int count = 0;


    if(!line || len <= 0)
        return 0;

    while(*p == ' ')
        ++p;

    /*blank line*/
    if(p - line == len || *p == '\r' || *p == '\n' || *p == '\0')
        return 0;

    /*do not support group*/
    if(*p == '[' || *p == '#' || *p == ';')
        return 0;

    /* extract key */
    start = p;
    end = line + len;
    while(*p != '=' && p!= end)
        ++p;

    /* none '=' , invalid line */
    if(p == end)
        return 0;

    end = p - 1;
    while(*end == ' ') /* skip blank at the end */
        --end;
    count = end - start + 1;

    ini->node[ini->count].name = (char *)malloc(count + 1);
    memset(ini->node[ini->count].name,0,count+1);
    strncpy(ini->node[ini->count].name, start, count);

    /* extract value */
    ++p;
    end = line + len; /* next pos of the last char */
    while( *p == ' ' && p != end)
        ++p;
    if(p == end)
    {
        goto err_parse_line;
    }
    start = p;
    --end; /* to the last char */
    if(*end == '\n')
        { *end = 0; --end; }
    if(*end == '\r')
        { *end = 0; --end; }
    count = end - start + 1;

    if(count > 0)
    {
        char *value = (char *)malloc(count + 1);
        memset(value,0,count+1);
        strncpy(value, start, count);

        if (!strncmp("0x",value,2) || !strncmp("0X",value,2))
        {
            ini->node[ini->count].value =  ini_hex_atoi(value);
        }
        else
        {
            ini->node[ini->count].value = ini_atoi(value);
        }

        //if(ini->count < INI_NODE_MAX)
        ini->count++;

        free(value);
    }
    else
    {
        goto err_parse_line;
    }

    return 0;

err_parse_line:
    free(ini->node[ini->count].name);
    return 0;
}

static int parse_file(struct ini_parser *ini)
{
    FILE *fp;
    char buf[1024];
    int value;
    int ret = 0;

    fp = fopen(ini->src_file, "r");
    if(fp)
    {
        while(fgets(buf, 1024, fp) != NULL) //read a line
        {
            parse_line(ini, buf, strlen(buf));
        }
        fclose(fp);
        return 0;
    }

    return -1;
}

static int find_value_by_name(struct ini_parser *ini,const char *name)
{
    int i;

    for(i=0; i<ini->count; i++)
    {
        if(strcmp(ini->node[i].name,name) == 0)
        {
            return ini->node[i].value;
        }
    }

    return 0;
}

static void ini_free(struct ini_parser *ini)
{
    int i;

    for(i=0; i<ini->count; i++)
    {
        if(ini->node[i].name)
        {
            free(ini->node[i].name);
        }
    }
    ini->count = 0;
    ini->src_file = NULL;
    ini->dest_file = NULL;
}

static int store_display_para_to_file(struct ini_parser *ini)
{
    int ret = -1;
    FILE *fp = fopen(ini->dest_file,"w+");

    if(fp == NULL)
        return ret;

    ret = fwrite((const void *)(&g_display_para), 1, sizeof(display_updatepara), fp);

    fclose(fp);

    return ret;
}

static int get_and_parser_arkdata_ini(const char *src_file, const char *dist_file)
{
    unsigned int * value_pos = NULL;
    char **name_pos = NULL;
    struct ini_parser ini = {0};
    int i = 0;
    int ret = -1;


    ini.count = 0;
    ini.src_file = src_file;
    ini.dest_file = dist_file;


    //parse file and store paragrams
    ret = parse_file(&ini);
    if(ret < 0)
        return ret;

    // ScreenInfo
    g_screeninfo.screen_id		= find_value_by_name(&ini,"ScreenId");
    g_screeninfo.screen_type	= find_value_by_name(&ini,"ScreenType");
    g_screeninfo.src_format		= find_value_by_name(&ini,"Format");
    g_screeninfo.rgb_mode		= find_value_by_name(&ini,"RgbMode");
    g_screeninfo.interlace		= find_value_by_name(&ini,"interlace");
    g_screeninfo.tvout_format	= find_value_by_name(&ini,"TvoutType");
    g_screeninfo.frame_rate		= find_value_by_name(&ini,"FrameRate");

    g_screeninfo.height			= find_value_by_name(&ini,"Height");
    g_screeninfo.width			= find_value_by_name(&ini,"Width");
    g_screeninfo.vbp			= find_value_by_name(&ini,"VBP");
    g_screeninfo.vfp			= find_value_by_name(&ini,"VFP");
    g_screeninfo.vsw			= find_value_by_name(&ini,"VSW");
    g_screeninfo.hbp			= find_value_by_name(&ini,"HBP");
    g_screeninfo.hfp			= find_value_by_name(&ini,"HFP");
    g_screeninfo.hsw			= find_value_by_name(&ini,"HSW");
    g_screeninfo.hsync_active	= find_value_by_name(&ini,"IHS");
    g_screeninfo.vsync_active	= find_value_by_name(&ini,"IVS");
    g_screeninfo.de_active		= find_value_by_name(&ini,"IOE");

    g_screeninfo.src_height		= find_value_by_name(&ini,"SrcHeight");
    g_screeninfo.src_width		= find_value_by_name(&ini,"SrcWidth");

    g_screeninfo.clk_source		= find_value_by_name(&ini,"CLKSource");
    g_screeninfo.vclk_active	= find_value_by_name(&ini,"CLKPolarity");
    g_screeninfo.clk_freq		= find_value_by_name(&ini,"CLKFreq");
    g_screeninfo.clk_div1		= find_value_by_name(&ini,"CLKDIV1");
    g_screeninfo.clk_div2		= find_value_by_name(&ini,"CLKDIV2");

    g_screeninfo.lvds_cfg		= find_value_by_name(&ini,"LVDSCfg");

    //linux add
    g_screeninfo.format			= find_value_by_name(&ini,"SubScreenType"); //各种display输出子类型
    g_screeninfo.tvenc			= find_value_by_name(&ini,"TVEnc"); //CVBS VGA YPBPR  --0x1F ,  RGB LVDS -- 0x08
    g_screeninfo.pad_unset      = find_value_by_name(&ini,"PadUnset"); //uboot display interface initial no set pad

    //VP
    value_pos = &g_vp.video_contrast;
    name_pos = vp_name;
    for (i = 0;i < VP_DATA_MAX;i++)
    {
        *value_pos = find_value_by_name(&ini,*name_pos);
        value_pos++;
        name_pos++;
    }

    //ITU656
    value_pos = &g_itu656info.ModeControl;
    name_pos = itu656_name;

    for (i = 0;i < ITU656_DATA_MAX;i++)
    {
        *value_pos = find_value_by_name(&ini,*name_pos);
        value_pos++;
        name_pos++;
    }

    //Gamma
    g_gammainfo.gamma_en  =  find_value_by_name(&ini,"Gamma_en");
    value_pos = g_gammainfo.gamma_regval;;
    name_pos = gamma_name;
    for (i = 0;i < GAMMA_REG_MAX;i++)
    {
        *value_pos = find_value_by_name(&ini,*name_pos);
        value_pos++;
        name_pos++;
    }

    check_display_interface();
    check_timming();
    check_clockdata();
    check_vp();
    check_gamma();
    check_itu656data();

    memset(&g_display_para,0,sizeof(display_updatepara));

    if(valid_flag == 0)
    {
        memcpy(&g_display_para.screeninfo,&g_screeninfo,sizeof(struct screen_info));
        printf("screeninfo check ok\r\n");
    }
    else
    {
        printf("some screeninfo check error,update fail.\r\n");
        goto err_get_and_parser_arkdata_ini;
        //return -1;
    }

    if(valid_flag_itu656 == 0)
    {
        memcpy(&g_display_para.itu656bypinfo,&g_itu656info,sizeof(itu656byp_info));
        printf("itu656bypinfo check ok \r\n");
    }
    else
    {
        memcpy(&g_display_para.itu656bypinfo, &itu656byp_default, sizeof(itu656byp_info));
        printf("some itu656bypinfo check error,set default\r\n");
    }

    if(valid_flag_vp== 0)
    {
        g_display_para.flag_vpinfo = VP_INFO_FLAG;
        memcpy(&g_display_para.vpinfo,&g_vp,sizeof(vp_info));
        printf("vpinfo check ok.\r\n");
    }
    else
    {
        g_display_para.flag_vpinfo = 0;
        printf("vpinfo use default.\r\n");
    }

    if(valid_flag_gamma== 0)
    {
        g_display_para.flag_gamma = GAMMA_INFO_FLAG;
        memcpy(&g_display_para.gammainfo,&g_gammainfo,sizeof(gamma_info));
        printf("gammainfo check ok.\r\n");
    }
    else
    {
        g_display_para.flag_gamma = 0;
        printf("gammainfo use default.\r\n");
    }

    memcpy(g_display_para.flag_id,id_flag,16);

    //save display paragram to file
    ret = store_display_para_to_file(&ini);  //return length of data
    if(ret != sizeof(display_updatepara))
        ret = -1;
    //printf("### ret:%d,display_updatepara :%d\n",ret,sizeof(display_updatepara));

err_get_and_parser_arkdata_ini:
    ini_free(&ini);

    return ret;
}

#ifdef YXCX_TEST
static void resolution_test(void)
{
    int screen_resolution = get_screen_resolution();
    char src_file[100], dest_file[100],sys_cmd[100];
    char* arkdata_path = (char*)getenv("ARKDATA_FILE_PATH");
    char *p = arkdata_path;
    int i = 100;
    int file_size;

    if(screen_resolution == RESOLUTION_UNDIFINE || screen_resolution != RESOLUTION_800X480)
    {
        //printf("### arkdata_path:%s\n",arkdata_path);
        if(arkdata_path == NULL)
        {
            printf("## err: arkdata_path == NULL\n");
            return ;
        }

        //move p to end
        while(*p != '\0' && --i)
            ++p;

        //envirment err
        if(i == 0)
        {
            return ;
        }

        //delete character space and '\0'
        while(p - arkdata_path > 0)
        {
            p--;
            if(*p != ' ' && *p != '/')  //no ' ' and '\0'
                break;
            *p = '\0';
        }

        sprintf(src_file,"%s/%s",arkdata_path,"arkdata_800x480.ini");
        sprintf(dest_file,"%s/%s",arkdata_path,"arkdata");
        file_size = get_and_parser_arkdata_ini(src_file,dest_file);

        if(file_size <= 0)
        {
            printf("### err: arkdata.ini size\n");
            return ;
        }

        sprintf(sys_cmd,"dd if=/dev/zero of=/dev/mtdblock7 bs=%d count=1",file_size);
        system(sys_cmd);
        sprintf(sys_cmd,"dd if=%s/arkdata of=/dev/mtdblock7 bs=%d count=1",arkdata_path,file_size);
        system(sys_cmd);
        system("sync && reboot");
    }
}
#endif
