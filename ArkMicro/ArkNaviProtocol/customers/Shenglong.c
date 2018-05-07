#include "Shenglong.h"
#include <linux/input.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <stdbool.h>

static int touch_command_serial_port_fd = -1;
static int input_fd = -1;

static int input_report(int type, int code, int val)
{
    static struct input_event ie;
    ie.type = type;
    ie.code = code;
    ie.value = val;
    int ret = 0;
    if (-1 != input_fd) {
        ret = write(input_fd, &ie, sizeof(struct input_event));
    }
    return ret;
}

int shenglong_touch_command_read(const int fd)
{
    static unsigned char press = 0x00;
    if (-1 != fd && touch_command_serial_port_fd == fd) {
        static unsigned char count = 0;
        static unsigned char buffer[6] = {0x00};
        static int ret = 0;
        ret = read(fd, &(buffer[count]), 1);
#ifdef DEBUG
        LOG_DEBUG("%s-%d,%x\n", __PRETTY_FUNCTION__, __LINE__, buffer[count]);
#endif
        if (1 == ret) {
            switch (buffer[0]) {
            case 0x61:
            case 0xA1:
            case 0xE1: {
                ++count;
                break;
            }
            default: {
                count = 0;
                memset(buffer, 0x00, sizeof(buffer));
                break;
            }
            }
        }
        if ((0 == ret)
                || (6 == count)) {
            if (6 == count) {
#ifdef DEBUG
                int iter1 = 0;
                LOG_DEBUG("in->%x\n");
                for (iter1 = 0; iter1 < count; ++iter1) {
                    LOG_DEBUG("%x ", buffer[iter1]);
                }
                LOG_DEBUG("out->%x\n");
#endif
                static unsigned char check_sum;
                check_sum = 0x00;
                static unsigned char iter = 0;
                for (iter = 0; iter < 5; ++iter) {
                    check_sum += buffer[iter];
                }
                check_sum = ~check_sum;
                if (buffer[5] == check_sum) {
                    if (0xe1 == buffer[0]) {
                        input_report(EV_ABS, ABS_PRESSURE, 0x000);
                        input_report(EV_SYN, 0, 0);
                    } else if ((0xA1 == buffer[0])
                               || (0x61 == buffer[0])){
                        unsigned short x = (buffer[2] << 8) | (buffer[1] << 0);
                        unsigned short y = (buffer[4] << 8) | (buffer[3] << 0);
                        if (press != ABS_PRESSURE) {
                            press = ABS_PRESSURE;
                        }
                        input_report(EV_ABS, ABS_X, x);
                        input_report(EV_ABS, ABS_Y, y);
                        input_report(EV_ABS, ABS_PRESSURE, 0xFFF);
                        input_report(EV_SYN, 0, 0);
                    }
                } else {
                    LOG_DEBUG("%s, checksum err\n", __PRETTY_FUNCTION__);
                }
            }
            count = 0;
            memset(buffer, 0x00, sizeof(buffer));
            //Key
        }
    } else {
        //timeout;
        if (ABS_PRESSURE == press) {
            press = 0x00;
            input_report(EV_ABS, ABS_PRESSURE, 0x000);
            input_report(EV_SYN, 0, 0);
        }
//        LOG_DEBUG("timeout\n");
    }
    return 0;
}

int shenglong_write(const struct ArkProtocol* protocol)
{
    LOG_DEBUG("%s, %d\n", __PRETTY_FUNCTION__, __LINE__);
    int ret = 0;
    if ((touch_command_serial_port_fd >= 0)
            && (NULL != protocol)
            && (NULL != protocol->data)) {
    LOG_DEBUG("protocol->type = %d, %s, %d\n", protocol->type, __PRETTY_FUNCTION__, __LINE__);
        switch (protocol->type) {
        case AT_UserInterface: {
            struct ArkUserInterface* launcher = (struct ArkUserInterface*)protocol->data;
            if (NULL != launcher) {
                switch (launcher->type) {
                case AUIT_Launcher: {
                    if (AUIS_ArkUserInterfaceBackground == launcher->status) {
                        unsigned char buffer[32] = {0x00};
                        unsigned char crc_index = 0;
                        buffer[crc_index++] = 0x12;
                        buffer[crc_index++] = 0x55;
                        buffer[crc_index++] = 0xaa;
                        int i;
                        for (i = 1; i < 4; ++i) {
                            buffer[crc_index] += buffer[i];
                        }
                        buffer[crc_index] = ~buffer[crc_index];
                        ret = write(touch_command_serial_port_fd, buffer, crc_index + 1);
                    }
                    break;
                }
                default: {
                    break;
                }
                }
            }
            break;
        }
        case AT_Multimedia: {
            struct ArkMultimedia* multimedia = (struct ArkMultimedia*)protocol->data;
            if (AMS_AVPlay == multimedia->status) {
                set_value_gpio(69, 0);
    LOG_DEBUG("%s, %d\n", __PRETTY_FUNCTION__, __LINE__);
//                unsigned char buffer[] = {0xFF, 0x55, 0x09, 0x03, 0x03, 0x00, 0x00, 0xF0, 0x01, 0x00, 0x00, 00};
//                ret = write(touch_command_serial_port_fd, (void*)buffer, sizeof(buffer));
            } else if (AMS_AVPause == multimedia->status) {
                set_value_gpio(69, 1);
    LOG_DEBUG("%s, %d\n", __PRETTY_FUNCTION__, __LINE__);
//                unsigned char buffer[] = {0xFF, 0x55, 0x09, 0x03, 0x04, 0x00, 0x00, 0xF0, 0x01, 0x00, 0x00, 0x01};
//                ret = write(touch_command_serial_port_fd, (void*)buffer, sizeof(buffer));
            }
            break;
        }
        case AT_Link: {
            LOG_DEBUG("%s, %d\n", __PRETTY_FUNCTION__, __LINE__);
            struct ArkLink* link = (struct ArkLink*)protocol->data;
            switch (link->type) {
            case ALT_LinkCarplay:
            case ALT_LinkCarlife:
            case ALT_LinkIOSCarlife:
            case ALT_LinkAndroidCarlife:
            case ALT_LinkAndroidMirror: {
                LOG_DEBUG("link->type = %d\n", link->type);
                LOG_DEBUG("link->status = %d\n", link->status);
                if (ALS_LinkSuccess == link->status) {
                    set_value_gpio(69, 0);
//                    LOG_DEBUG("ALT_LinkInserted\n");
//                    unsigned char buffer[] = {0xFF, 0x55, 0x09, 0x03, 0x04, 0x00, 0x00, 0xF0, 0x01, 0x00, 0x00, 0x01};
//                    ret = write(touch_command_serial_port_fd, (void*)buffer, sizeof(buffer));
                } else if ((ALT_LinkExited == link->status)
                           || (ALT_LinkNotInserted == link->status)) {
                    set_value_gpio(69, 1);
//                    LOG_DEBUG("ALT_LinkExited ALT_LinkNotInserted\n");
//                    unsigned char buffer[] = {0xFF, 0x55, 0x09, 0x03, 0x03, 0x00, 0x00, 0xF0, 0x01, 0x00, 0x00, 00};
//                    ret = write(touch_command_serial_port_fd, (void*)buffer, sizeof(buffer));
                }
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
    return ret;
}
#if 0
#Protocal
export TOUCHSERIAL=/dev/ttyHS0
        export TOUCHSERIAL_BAUDRATE=19200
        export COMMANDSERIAL=/dev/ttyHS0
        export COMMANDSERIAL_BAUDRATE=19200
        export PROTOCOL_ID=Shenglong
#endif
enum ArkCustomer shenglong_init()
{
    LOG_DEBUG("shenglong_init\n");

    char* customer_id = (char*)getenv("PROTOCOL_ID");
    if (NULL != customer_id) {
        if (0 == strcmp(customer_id, "Shenglong")) {
            export_gpio(69);
            direction_gpio(69, "out");
            set_value_gpio(69, 1);
        } else return AC_Undefine;
    } else return AC_Undefine;

    char* touch_serial_port_name = (char*)getenv("TOUCHSERIAL");

    if (NULL != touch_serial_port_name) {
        touch_command_serial_port_fd = open(touch_serial_port_name, O_RDWR);
        if (touch_command_serial_port_fd < 0) {
            LOG_DEBUG("open %s error\n", touch_serial_port_name);
            return AC_Error;
        } else {
            char* touch_serial_port_baudrate = (char*)getenv("TOUCHSERIAL_BAUDRATE");
            if (NULL != touch_serial_port_baudrate) {
                int baudrate = atoi(touch_serial_port_baudrate);
                set_speed(touch_command_serial_port_fd, baudrate);
                if (set_parity(touch_command_serial_port_fd, TOUCH_DATA_BITS, TOUCH_STOP_BITS, TOUCH_PARITY_STATUS) == false) {
                    LOG_DEBUG("uart_touch_init: Set parity error\n");
                    return AC_Error;
                }
            } else {
                LOG_DEBUG("TOUCHSERIAL_BAUDRATE env is null");
                return AC_Error;
            }
        }
    }

    shenglong.fd[0] = touch_command_serial_port_fd;

    if (-1 == (input_fd = open(INPUT_DEVICE_PATH, O_WRONLY))) {
        LOG_DEBUG("open %s error\n", INPUT_DEVICE_PATH);
        return AC_Error;
    }

    return AC_Shenglong;
}

int shenglong_uninit()
{
    LOG_DEBUG("shenglong_bmw_uninit\n");
    unexport_gpio(69);

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

struct CustomerProtocol shenglong = {
    .customer = AC_Shenglong,
    .fd = {-1, -1, -1},
    .init = shenglong_init,
    .read = shenglong_touch_command_read,
    .write = shenglong_write,
    .uninit = shenglong_uninit,
};
