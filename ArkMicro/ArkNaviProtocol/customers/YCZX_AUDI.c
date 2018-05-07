#include "YCZX_AUDI.h"
#include <linux/input.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
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

int yczx_audi_touch_command_read(const int fd)
{
    if (-1 != fd && touch_command_serial_port_fd == fd) {
        static unsigned char start = 0;
        static unsigned char buffer[48] = {0x00};
        static unsigned char length = 0;
        static unsigned char iter = 0;
        static int ret = 0;
        static unsigned char tmp = 0x00;
        ret = read(fd, &tmp, 1);
        if (1 == ret) {
            if ((0 == start)
                    && (0xF0 == tmp)) {
                start = 1;
            } else if (1 == start) {
                if (tmp < 48) {
                    length = tmp + 1;
                    start = 2;
                } else {
                    length = 0;
                    start = 0;
                }
                iter = 0;
            } else {
                if (iter < length) {
                    buffer[iter++] = tmp;
                } else {
                    unsigned char checkSum = length - 1;
                    int i;
                    for (i = 0; i < length - 1; ++i) {
                        checkSum += buffer[i];
                    }
                    if (checkSum == buffer[length - 1]) {
                        if ((0x00 == buffer[0])
                                && (0x0C == buffer[1])
                                && (0x01 == buffer[2])) {
                            unsigned short x = buffer[4] + (buffer[5] << 8);
                            unsigned short y = buffer[6] + (buffer[7] << 8);
                            if (0x00 == buffer[3]) {
                                input_report(EV_ABS, ABS_PRESSURE, 0x000);
                                input_report(EV_SYN, 0, 0);
                            } else {
                                input_report(EV_ABS, ABS_X, x);
                                input_report(EV_ABS, ABS_Y, y);
                                input_report(EV_ABS, ABS_PRESSURE, 0xFFF);
                                input_report(EV_SYN, 0, 0);
                            }
                        }
                    }
                    start = 0;
                    length = 0;
                    iter = 0;
                    tmp = 0x00;
                    memset(buffer, 0x00, sizeof(buffer));
                }
            }
        }
    } else {
        //timeout;
        //        LOG_DEBUG("timeout\n");
    }

    return 0;
}

int yczx_audi_write(const struct ArkProtocol* protocol)
{
    int ret = 0;
    if ((touch_command_serial_port_fd >= 0)
            && (NULL != protocol)
            && (NULL != protocol->data)
            && (AT_UserInterface == protocol->type)) {
        struct ArkUserInterface* ui = (struct ArkUserInterface*)protocol->data;
        if (NULL != ui) {
            unsigned char buffer[32] = {0x00};
            unsigned char crc_index = 0;
            switch (ui->type) {
            case AUIT_Launcher: {
                if (AUIS_ArkUserInterfaceInit == ui->status) {
                    buffer[crc_index++] = 0xF0;
                    buffer[crc_index++] = 0x04;
                    buffer[crc_index++] = 0x00;
                    buffer[crc_index++] = 0x00;
                    buffer[crc_index++] = 0x00;
                    buffer[crc_index++] = 0x01;
                } else {
                    return ret;
                }
                break;
            }
            default: {
                return ret;
                break;
            }
            }
            int i;
            for (i = 1; i < 6; ++i) {
                buffer[crc_index] += buffer[i];
            }
            ret = write(touch_command_serial_port_fd, buffer, crc_index + 1);
        }
    }
    return ret;
}

enum ArkCustomer yczx_audi_init()
{
    LOG_DEBUG("yczx_audi_init\n");
    char* customer_id = (char*)getenv("PROTOCOL_ID");
    if (NULL != customer_id) {
        if (0 == strcmp(customer_id, "YCZX_AUDI")) {
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
            int baudrate = atoi((char*)getenv("TOUCHSERIAL_BAUDRATE"));
            set_speed(touch_command_serial_port_fd, baudrate);
            if (set_parity(touch_command_serial_port_fd, TOUCH_DATA_BITS, TOUCH_STOP_BITS, TOUCH_PARITY_STATUS) == false) {
                LOG_DEBUG("uart_touch_init: Set parity error\n");
                return AC_Error;
            }
        }
    }

    yczx_audi.fd[0] = touch_command_serial_port_fd;

    if (-1 == (input_fd = open(INPUT_DEVICE_PATH, O_WRONLY))) {
        LOG_DEBUG("open %s error\n", INPUT_DEVICE_PATH);
        return AC_Error;
    }

    return AC_YCZXAUDI;
}

int yczx_audi_uninit()
{
    LOG_DEBUG("yczx_audi_uninit\n");
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

struct CustomerProtocol yczx_audi = {
    .customer = AC_YCZXAUDI,
    .fd = {-1, -1, -1},
    .init = yczx_audi_init,
    .read = yczx_audi_touch_command_read,
    .write = yczx_audi_write,
    .uninit = yczx_audi_uninit,
};
