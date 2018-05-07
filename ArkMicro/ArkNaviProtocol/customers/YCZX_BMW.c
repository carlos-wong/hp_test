#include "YCZX_BMW.h"
#include <linux/input.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <memory.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdlib.h>

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

const struct arkkeytype_map yczx_bmw_key_map[] = {
{0x03, AKT_CommonReturn},
{0x04, AKT_CommonEnter},
{0x05, AKT_CommonLeft},
{0x06, AKT_CommonRight},
{0x07, AKT_CommonUp},
{0x08, AKT_CommonDown},
{0x09, AKT_CommonMenu},
{0x0A, AKT_CommonVoice},
{0x0B, AKT_CommonPhoneAcceptRejectToggle},
{0x0C, AKT_CommonIdle},
{0x0D, AKT_CommonVolumeMute},
{0x0E, AKT_CommonVolumeDecrement},
{0x0F, AKT_CommonVolumeIncrement},
{0x12, AKT_CommonPrevious},
{0x13, AKT_CommonNext},
{0x14, AKT_CommonNaviEnterExitToggle},
{0x15, AKT_CommonPlayPauseToggle},
{0x31, AKT_CommonRightTurn},
{0x32, AKT_CommonLeftTurn},
};

int yczx_bmw_touch_command_read(const int fd)
{
    if (-1 != fd && touch_command_serial_port_fd == fd) {
        static unsigned char count = 0;
        static unsigned char buffer[12] = {0x00};
        static int ret = 0;
        ret = read(fd, &(buffer[count]), 1);
        //        LOG_DEBUG("%s=%x\n", __PRETTY_FUNCTION__, buffer[count]);
        if (1 == ret) {
            ++count;
        }
        if ((0 == ret)
                || (12 == count)) {
            if (12 == count) {
                static unsigned char check_sum;
                check_sum = 0x00;
                static unsigned char iter = 0;
                for (iter = 2; iter < 11; ++iter) {
                    check_sum += buffer[iter];
                }
                if (buffer[11] == check_sum) {
                    if ((0xFF == buffer[0])
                            && (0x55 == buffer[1])
                            && (0x09 == buffer[2])) {
                        if (0x05 == buffer[3]) {
                            static float width_scalability_factor = -1.0f;
                            static float height_scalability_factor = -1.0f;
                            if ((width_scalability_factor < 0.0f)
                                    || (height_scalability_factor < 0.0f)) {
                                unsigned short maxX = buffer[8] + (((buffer[9] & 0xF0) >> 4) << 8);
                                unsigned short maxY = buffer[10] + ((buffer[9] & 0x0F) << 8);
                                width_scalability_factor = maxX / (float)(800);
                                height_scalability_factor = maxY / (float)(480);
                            }
                            unsigned short x = buffer[5] + (((buffer[6] & 0xF0) >> 4) << 8);
                            unsigned short y = buffer[7] + ((buffer[6] & 0x0F) << 8);
                            x = x / width_scalability_factor;
                            y = y / height_scalability_factor;
                            if (0x03 == buffer[4]) {
                                input_report(EV_ABS, ABS_PRESSURE, 0x000);
                                input_report(EV_SYN, 0, 0);
                            } else {
                                input_report(EV_ABS, ABS_X, x);
                                input_report(EV_ABS, ABS_Y, y);
                                input_report(EV_ABS, ABS_PRESSURE, 0xFFF);
                                input_report(EV_SYN, 0, 0);
                            }
                        } else if (0x01 == buffer[3]) {
                            unsigned char carplay[] = {0xFF, 0x55, 0x09, 0x01, 0x02, 0x00, 0x00, 0xF0, 0x01, 0x00, 0x00, 0xFD};
                            unsigned char carlife[] = {0xFF, 0x55, 0x09, 0x01, 0x01, 0x00, 0x00, 0xF0, 0x01, 0x00, 0x00, 0xFD};
                            unsigned char query[] = {0xFF, 0x55, 0x09, 0x01, 0x04, 0x00, 0x00, 0xF0, 0x01, 0x00, 0x00, 0xFF};
                            if (0 == memcmp(buffer, carplay, sizeof(carplay))) {
                            } else if (0 == memcmp(buffer, carlife, sizeof(carlife))) {
                            } else if (0 == memcmp(buffer, query, sizeof(query))) {
                            }
                        }
                    }
                }
            }
            count = 0;
            memset(buffer, 0x00, sizeof(buffer));
            //Key 0x2E, 0x06, 0x02, 0x00, 0x00, 0x08
        } else if (0x2E == buffer[0]) {
            if ((0x06 == buffer[1])
                    && (0x02 == buffer[2])) {
                int j = 0;
                for (j = 0; j < count; ++j) {
                    LOG_DEBUG("j=%d, buffer = %x\n", j, buffer[j]);
                }
                if (count > 5) {
                    unsigned char check = 0x00;
                    int iterator;
                    for (iterator = 1; iterator < count - 1; ++iterator) {
                        check += buffer[iterator];
                    }
                    check ^= 0xFF;
                    int j = 0;
                    for (j = 0; j < count; ++j) {
                        LOG_DEBUG("j = %x\n", buffer[j]);
                    }
                    LOG_DEBUG("\\\\\\\\\\\\\\");
                    if (check == buffer[count - 1]) {
                        LOG_DEBUG("key check sum ok\n");
                        LOG_DEBUG("key=%x, status=%x\n", buffer[3], buffer[4]);
                        int iter;
                        for (iter = 0; iter < sizeof(yczx_bmw_key_map) / sizeof(yczx_bmw_key_map[0]); ++iter) {
                            if (buffer[3] == yczx_bmw_key_map[iter].src_key) {
                                struct ArkKey key;
                                key.type = yczx_bmw_key_map[iter].src_key;
                                if (0x00 == buffer[4]) {
                                    key.status = AKS_Release;
                                } else if (0x01 == buffer[4]) {
                                    key.status = AKS_Press;
                                } else {
                                    if ((key.type != AKT_CommonLeftTurn)
                                            || (key.type != AKT_CommonRightTurn)) {
                                        break;
                                    } else {
                                        key.status = buffer[4];
                                    }
                                }
                                struct ArkProtocol protocol;
                                protocol.type = AT_Key;
                                protocol.data = &key;
                                callback_notify(&protocol);
                                break;
                            }
                        }
                    }
                    count = 0;
                    memset(buffer, 0x00, sizeof(buffer));
                }
            }
        } else if (((count > 0)
                    && (0xFF != buffer[0]))
                   || ((count > 1)
                       && (0x55 != buffer[1]))
                   || ((count > 2) && (0x09 != buffer[2]))) {
            count = 0;
            memset(buffer, 0x00, sizeof(buffer));
        }
    } else {
        //timeout;
        //        LOG_DEBUG("timeout\n");
    }

    return 0;
}

int yczx_bmw_write(const struct ArkProtocol* protocol)
{
    //    LOG_DEBUG("%s, %d\n", __PRETTY_FUNCTION__, __LINE__);
    //    LOG_DEBUG("fd, %d\n", fd);
    //    LOG_DEBUG("protocol, %x\n", protocol);
    //    if (NULL != protocol) {
    //        LOG_DEBUG("protocol->data, %x\n", protocol->data);
    //        LOG_DEBUG("protocol->type, %d\n", protocol->type);
    //    }
    int ret = 0;
    if ((touch_command_serial_port_fd >= 0)
            && (NULL != protocol)
            && (NULL != protocol->data)) {
        switch (protocol->type) {
        case AT_UserInterface: {
            struct ArkUserInterface* launcher = (struct ArkUserInterface*)protocol->data;
            if (NULL != launcher) {
                switch (launcher->type) {
                case AUIT_Launcher: {
                    if (AUIS_ArkUserInterfaceForeground == launcher->status) {
                        unsigned char buffer[] = {0xFF, 0x55, 0x09, 0x01, 0x03, 0x00, 0x00, 0xF0, 0x01, 0x00, 0x00, 0xFE};
                        ret = write(touch_command_serial_port_fd, (void*)buffer, sizeof(buffer));
                    } else if (AUIS_ArkUserInterfaceBackground == launcher->status) {
                        unsigned char buffer[] = {0xFF, 0x55, 0x09, 0x03, 0x05, 0x00, 0x00, 0xF0, 0x01, 0x00, 0x00, 0x02};
                        ret = write(touch_command_serial_port_fd, (void*)buffer, sizeof(buffer));
                    }
                    break;
                }
                default: {
                    break;
                }
                }
            }
        }
        case AT_Multimedia: {
            struct ArkMultimedia* multimedia = (struct ArkMultimedia*)protocol->data;
            if (AMS_DiskRemove == multimedia->status) {
                unsigned char buffer[] = {0xFF, 0x55, 0x09, 0x03, 0x03, 0x00, 0x00, 0xF0, 0x01, 0x00, 0x00, 00};
                ret = write(touch_command_serial_port_fd, (void*)buffer, sizeof(buffer));
            } else if (AMS_DiskBusy == multimedia->status) {
                unsigned char buffer[] = {0xFF, 0x55, 0x09, 0x03, 0x04, 0x00, 0x00, 0xF0, 0x01, 0x00, 0x00, 0x01};
                ret = write(touch_command_serial_port_fd, (void*)buffer, sizeof(buffer));
            }
            break;
        }
        case AT_Link: {
            struct ArkLink* link = (struct ArkLink*)protocol->data;
            switch (link->type) {
            case ALT_LinkCarplay:
            case ALT_LinkCarlife:
            case ALT_LinkIOSCarlife:
            case ALT_LinkAndroidCarlife:
            case ALT_LinkAndroidMirror: {
                if (ALT_LinkInserted == link->status) {
                    LOG_DEBUG("ALT_LinkInserted\n");
                    unsigned char buffer[] = {0xFF, 0x55, 0x09, 0x03, 0x04, 0x00, 0x00, 0xF0, 0x01, 0x00, 0x00, 0x01};
                    ret = write(touch_command_serial_port_fd, (void*)buffer, sizeof(buffer));
                } else if ((ALT_LinkExited == link->status)
                           || (ALT_LinkNotInserted == link->status)) {
                    LOG_DEBUG("ALT_LinkExited ALT_LinkNotInserted\n");
                    unsigned char buffer[] = {0xFF, 0x55, 0x09, 0x03, 0x03, 0x00, 0x00, 0xF0, 0x01, 0x00, 0x00, 00};
                    ret = write(touch_command_serial_port_fd, (void*)buffer, sizeof(buffer));
                }
                break;
            }
            default: {
                break;
            }
            }
        }
        default: {
            break;
        }
        }
    }
    return ret;
}

enum ArkCustomer yczx_bmw_init()
{
    LOG_DEBUG("yczx_bmw_init\n");
    char* customer_id = (char*)getenv("PROTOCOL_ID");
    if (NULL != customer_id) {
        if (0 == strcmp(customer_id, "YCZX_BMW")) {
            export_gpio(69);
            direction_gpio(69, "out");
            //                set_value_gpio(69, 1);
        } else return AC_Undefine;
    } else return AC_Undefine;

    char* touch_serial_port_name = (char*)getenv("TOUCHSERIAL");

    if (NULL != touch_serial_port_name) {
        touch_command_serial_port_fd = open(touch_serial_port_name, O_RDWR);
        if (touch_command_serial_port_fd < 0) {
            LOG_DEBUG("open %s error\n", touch_serial_port_name);
            return AC_Error;
        } else {
            int baudrate = atoi((char*)getenv("TOUCHSERIAL_BAUDRATE"));
            set_speed(touch_command_serial_port_fd, baudrate);
            if (set_parity(touch_command_serial_port_fd, TOUCH_DATA_BITS, TOUCH_STOP_BITS, TOUCH_PARITY_STATUS) == false) {
                LOG_DEBUG("uart_touch_init: Set parity error\n");
                return AC_Error;
            }
        }
    }

    yczx_bmw.fd[0] = touch_command_serial_port_fd;

    if (-1 == (input_fd = open(INPUT_DEVICE_PATH, O_WRONLY))) {
        LOG_DEBUG("open %s error\n", INPUT_DEVICE_PATH);
        return AC_Error;
    }

    return AC_YCZXBMW;
}

int yczx_bmw_uninit()
{
    LOG_DEBUG("yczx_bmw_uninit\n");
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

struct CustomerProtocol yczx_bmw = {
    .customer = AC_YCZXBMW,
    .fd = {-1, -1, -1},
    .init = yczx_bmw_init,
    .read = yczx_bmw_touch_command_read,
    .write = yczx_bmw_write,
    .uninit = yczx_bmw_uninit,
};
