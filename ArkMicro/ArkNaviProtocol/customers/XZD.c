#include "XZD.h"
#include <linux/input.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdbool.h>
#include <unistd.h>

static int touch_serial_port_fd = -1;
static int command_serial_port_fd = -1;
static int input_fd = -1;
static unsigned short gpio_value = 0;
//static unsigned short gpio_magic_value = 10086;//magic number
static enum ArkUserInterfaceStatus ark_userinterface_status = AUIS_Undefine;

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

static int xzd_commond_parse(unsigned char *buffer, int len)
{
    unsigned char cmd = buffer[0];
    unsigned char *buf = &buffer[1];
    static unsigned char reverse = 0;
    switch(cmd)
    {
    case 0x07:
    case 0x08:
    case 0x09:
    case 0x0a:
    case 0x0b:
    {
        struct ArkCar car;
        memset(&car, 0, sizeof(struct ArkCar));
        car.type = ACT_Version;
        car.status = ACS_Undefine;
        car.data = (void*)buf;
        car.length = 8;
        struct ArkProtocol protocol;
        protocol.type = AT_Car;
        protocol.data = &car;
        callback_notify(&protocol);
        if (0x0b == cmd) {
            car.data = NULL;
            car.length = 0;
            callback_notify(&protocol);
        }
        break;
    }
    case 0x68:
    {
        struct ArkCar car;
        memset(&car, 0, sizeof(struct ArkCar));
        struct ArkProtocol protocol;
        memset(&protocol, 0, sizeof(struct ArkProtocol));
        //byte 0: reverse status
        if(len > 1)
        {
            if(buf[0] == 2)
            {
                car.type = ACT_Reversing;
                car.status = ACS_ReversingOn;
                car.data = NULL;
                car.length = 0;
                protocol.type = AT_Car;
                protocol.data = &car;
                reverse = 1;

                callback_notify(&protocol);
            }
            else if(buf[0] == 0 && reverse == 1)
            {
                car.type = ACT_Reversing;
                car.status = ACS_ReversingOff;
                protocol.type = AT_Car;
                protocol.data = &car;
                reverse = 0;
                callback_notify(&protocol);
            }
        }
        if(reverse==0)
            return 0;
        //byte 1: rear left rader
        /*
        int j = 0;
        for (; j < len; ++j) {
            fprintf(stderr, "%x ",buffer[j]);
        }
        fprintf(stderr, "\n");
        */
        if(len > 2)
        {
            car.type = ACT_Radar;
            car.status = ACS_RadarRearLeft;
            car.data = (void*)(buf+1);
            car.length = 1;
            protocol.type = AT_Car;
            protocol.data = &car;
            fprintf(stderr, "ACS_RadarRearLeft%d,%d\n ",ACS_RadarRearLeft,(unsigned char)*car.data );
            callback_notify(&protocol);
        }

        //byte 2: rear right rader
        if(len > 3)
        {
            car.type = ACT_Radar;
            car.status = ACS_RadarRearRight;
            car.data = (void*)(buf+2);
            car.length = 1;
            protocol.type = AT_Car;
            protocol.data = &car;
            fprintf(stderr, "ACS_RadarRearRight%d,%d\n ",ACS_RadarRearRight,(unsigned char)*car.data );
            callback_notify(&protocol);
        }

        //byte 3: front left rader
        if(len > 4)
        {
            car.type = ACT_Radar;
            car.status = ACS_RadarFrontLeft;
            car.data = (void*)(buf+3);
            car.length = 1;
            protocol.type = AT_Car;
            protocol.data = &car;
            fprintf(stderr, "ACS_RadarFrontLeft%d,%d\n ",ACS_RadarFrontLeft,(unsigned char)*car.data );
            callback_notify(&protocol);
        }

        //byte 4: front right rader
        if(len > 5)
        {
            car.type = ACT_Radar;
            car.status = ACS_RadarFrontRight;
            car.data = (void*)(buf+4);
            car.length = 1;
            protocol.type = AT_Car;
            protocol.data = &car;
            fprintf(stderr, "ACS_RadarFrontRight%d,%d\n ",ACS_RadarFrontRight,(unsigned char)*car.data );
            callback_notify(&protocol);
        }

        //byte 5: steering
        if(len > 6)
        {
            car.type = ACT_Steering;
            car.status = ACS_SteeringValue;
            car.data = (void*)(buf+5);
            car.length = 1;
            protocol.type = AT_Car;
            protocol.data = &car;

            callback_notify(&protocol);
        }

        //byte 6: speed
        if(len >7)
        {
            car.type = ACT_Speed;
            car.status = ACS_SpeedValue;
            car.data = (void*)(buf+6);
            car.length = 1;
            protocol.type = AT_Car;
            protocol.data = &car;

            callback_notify(&protocol);
        }

        //byte 7: reserve
        if(len > 8)
        {
            car.type = ACT_Undefine;
            car.status = ACS_Undefine;
            car.data = NULL;
            car.length = 0;
            protocol.type = AT_Car;
            protocol.data = &car;

            callback_notify(&protocol);
        }
        break;
    }
    default:
        break;

    }
}
int xzd_touch_command_read(const int fd)
{
    if (-1 != fd && touch_serial_port_fd == fd) {
        static unsigned char count = 0;
        static unsigned char buffer[6] = {0x00};
        static int ret = 0;
        ret = read(fd, &(buffer[count]), 1);
        if (1 == ret) {
            ++count;
        }
        if ((0 == ret)
                || (6 == count)) {
            if (6 == count) {
                static unsigned char check_sum;
                check_sum = 0x00;
                static unsigned char iter = 0;
                for (iter = 0; iter < 5; ++iter) {
                    check_sum += buffer[iter];
                }
                check_sum = ~check_sum;
                if (buffer[5] == check_sum) {
                    unsigned short x = (buffer[2] << 8) | (buffer[1] << 0);
                    unsigned short y = (buffer[4] << 8) | (buffer[3] << 0);
                    if (0xe1 == buffer[0]) {
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
            count = 0;
            memset(buffer, 0x00, sizeof(buffer));
            //Key
        }
    } else if (-1 != fd && command_serial_port_fd == fd) {
        static int ret = 0;
        static unsigned char buffer[16] = {0x00};
        static unsigned char iter = 0;
        ret = read(fd, &buffer[iter], 1);
        if ((1 == ret)
                && (0x0D == buffer[0])) {
            if (iter < 16) {
                if ((++iter > 2)
                        && (0x24 == buffer[1])
                        && (iter == (buffer[2] + 4))) {
                    int i = 2;
                    unsigned char crc = 0x00;
                    crc ^= 0xFF;
                    for (/**/; i < iter - 1; ++i) {
                        crc ^= buffer[i];
                    }
                    if (crc == buffer[iter - 1]) {

                        if ((0x61 <= buffer[3])
                                && (0x63 >= buffer[3])) {

                            fprintf(stderr, "xzd_touch_command_read read1 %x, %x\n", buffer[3], buffer[3]);
                            struct ArkCar car;
                            memset(&car, 0, sizeof(struct ArkCar));
                            car.type = ACT_MCUUpdate;
                            car.status = ACS_Undefine;
                            car.data = (void*)(buffer + 3);
                            car.length = buffer[2];
                            struct ArkProtocol protocol;
                            memset(&protocol, 0, sizeof(struct ArkProtocol));
                            protocol.type = AT_Car;
                            protocol.data = &car;
                            callback_notify(&protocol);
                            //     fprintf(stderr, "read0 %s, %d\n", __PRETTY_FUNCTION__, __LINE__);
                        }
                        else{
                            //      int j = 0;
                            //   for (/**/; j < buffer[2]; ++j) {
                            //        fprintf(stderr, "%x ",buffer[3+j]);
                            //    }
                            //   fprintf(stderr, "\n");
                            xzd_commond_parse(&buffer[3], buffer[2]);
                        }
                    }
                    iter = 0;
                    memset(buffer, 0, sizeof(buffer));
                }
            } else {
                iter = 0;
                memset(buffer, 0, sizeof(buffer));
            }
        }
    } else {
        //        static unsigned char count = 0;
        //        if (0 == access("/tmp/protocol1", F_OK)) {
        //            if ((++count % 10) == 0) {
        //                //
        //                unsigned char buffer[16] = {0x0D, 0x24, 0x09, 0x68, 0x02, 0x22, 0x22, 0x12, 0x21, 0x96, 0x00, 0x00};
        //                xzd_commond_parse(&buffer[3], buffer[2]);
        //            }
        //        }
        //        if (0 == access("/tmp/protocol2", F_OK)) {
        //            if ((++count % 10) == 0) {
        //                //
        //                unsigned char buffer[16] = {0x0D, 0x24, 0x09, 0x68, 0x02, 0x14, 0x41, 0x31, 0x13, 0x96, 0x00, 0x00};
        //                xzd_commond_parse(&buffer[3], buffer[2]);
        //            }
        //        }
        //timeout
    }

    return 0;
}

int xzd_write(const struct ArkProtocol* protocol)
{
    //    fprintf(stderr, "%s, %d\n", __PRETTY_FUNCTION__, __LINE__);
    int ret = 0;
    if ((command_serial_port_fd >= 0)
            && (NULL != protocol)
            && (NULL != protocol->data)) {
        fprintf(stderr, "%s, %d\n", __PRETTY_FUNCTION__, __LINE__);
        switch (protocol->type) {
        case AT_UserInterface: {
            struct ArkUserInterface* ui = (struct ArkUserInterface*)protocol->data;
            if (NULL != ui) {
                unsigned char buffer[32] = {0x00};
                unsigned char crc_index = 0;
                buffer[crc_index++] = 0x0D;
                buffer[crc_index++] = 0x24;
                switch (ui->type) {
                case AUIT_Launcher: {
                    if (AUIS_ArkUserInterfaceInit == ui->status) {
                        //                     fprintf(stderr, "%s, %d\n", __PRETTY_FUNCTION__, __LINE__);
                        buffer[crc_index++] = 0x03;
                        buffer[crc_index++] = 0x00;
                        buffer[crc_index++] = 0x01;
                        buffer[crc_index++] = 0xFF;
                    } else if (AUIS_ArkUserInterfaceForeground == ui->status) {
                        //                     fprintf(stderr, "%s, %d\n", __PRETTY_FUNCTION__, __LINE__);
                        buffer[crc_index++] = 0x02;
                        buffer[crc_index++] = 0x02;
                        buffer[crc_index++] = 0x04;
                        if (AUIS_ArkUserInterfaceForeground != ark_userinterface_status) {
                            ark_userinterface_status = AUIS_ArkUserInterfaceForeground;
                            //                            if (0 != gpio_magic_value) {
                            //                                gpio_magic_value = 0;
                            //                                set_value_gpio(69, gpio_magic_value);
                            //                            }
                        }
                    }  else if (AUIS_ArkUserInterfaceBackground == ui->status) {
                        //                     fprintf(stderr, "%s, %d\n", __PRETTY_FUNCTION__, __LINE__);
                        buffer[crc_index++] = 0x02;
                        buffer[crc_index++] = 0x02;
                        buffer[crc_index++] = 0x05;
                        if (AUIS_ArkUserInterfaceBackground != ark_userinterface_status) {
                            ark_userinterface_status = AUIS_ArkUserInterfaceBackground;
                            //                            if (1 != gpio_magic_value) {
                            //                                gpio_magic_value = 1;
                            //                                set_value_gpio(69, gpio_magic_value);
                            //                            }
                        }
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
                buffer[crc_index] ^= 0xFF;
                for (i = 2; i < 6; ++i) {
                    buffer[crc_index] ^= buffer[i];
                }
                ret = write(command_serial_port_fd, buffer, crc_index + 1);
            }
        }
        case AT_Car: {
            // fprintf(stderr, "%s, %d\n", __PRETTY_FUNCTION__, __LINE__);
            struct ArkCar* car = (struct ArkCar*)protocol->data;
            if (NULL != car) {
                //   fprintf(stderr, "%s, %d\n", __PRETTY_FUNCTION__, __LINE__);
                static unsigned char car_dev_status = 0x00;
                static unsigned char car_dev_status_tab[6] = {0, 0, 0, 0, 0, 0};
                unsigned char car_dev_val = 0x00;
                //  fprintf(stderr, "car->type = %d\n", car->type);
                switch (car->type) {
                case ACT_MCUUpdate: {
                    unsigned char buffer[256] = {0xFF};
                    int crc_index = 0;
                    buffer[crc_index++] = 0x0D;
                    buffer[crc_index++] = 0x24;
                    if(car->length>0)
                    {
                        memcpy(&buffer[crc_index],car->data,car->length);
                        crc_index += car->length;
                    }
                    int i;
                    buffer[crc_index] = 0;
                    buffer[crc_index] ^= 0xFF;
                    for (i = 2; i < crc_index; ++i) {
                        buffer[crc_index] ^= buffer[i];
                    }
                    /*  int iter = 0;
                    for (iter = 0; iter <= crc_index; ++iter) {
                        fprintf(stderr, "%x\n", buffer[iter]);
                    }
                    */
                    ret = write(command_serial_port_fd, buffer, crc_index + 1);
                    fprintf(stderr, "ACT_MCUUpdate ret :%d\n",ret);
                    return ret;
                    break;
                }
                case ACT_Reversing: {
                    if (ACS_ReversingOn == car->status) {
                        //       set_value_gpio(69, 1);
                        return ;
                    } else if (ACS_ReversingOff == car->status) {
                        //      set_value_gpio(69, 0);
                        return ;
                    }
                    break;
                }
                case ACT_Camera: {
                    switch (car->status) {
                    case ACS_CameraOriginal: {
                        car_dev_val = 0x00;
                        break;
                    }
                    case ACS_CameraExtra: {
                        car_dev_val = 0x01;
                        break;
                    }
                    case ACS_Camera360: {
                        car_dev_val = 0x02;
                        break;
                    }
                    default: {
                        break;
                    }
                    }
                    car_dev_status_tab[0] = car_dev_val;
                    car_dev_status |= (0x01 << 0);
                    if (0x07 == car_dev_status) {
                        struct ArkProtocol protocol;
                        protocol.type = AT_Car;
                        protocol.data = car;
                        callback_notify(&protocol);
                    }
                    break;
                }
                case ACT_Radar: {
                    //     fprintf(stderr, "asddsda->%d\n", car->status);
                    switch (car->status) {
                    case ACS_RadarOn: {
                        car_dev_val = 0x00;
                        break;
                    }
                    case ACS_RadarOff: {
                        car_dev_val = 0x01;
                        break;
                    }
                    default: {
                        break;
                    }
                    }
                    car_dev_status_tab[5] &= ~(0x02);
                    //    fprintf(stderr, "car_dev_status_tab[5]%d\n", car_dev_status_tab[5]);
                    if (car_dev_val) {
                        car_dev_status_tab[5] |= 0x02;
                    }
                    //    fprintf(stderr, "car_dev_status_tab[5]%d\n", car_dev_status_tab[5]);
                    //    fprintf(stderr, "%s, %d, car_dev_val=%d\n", __PRETTY_FUNCTION__, __LINE__, car_dev_val);
                    car_dev_status |= (0x01 << 2);
                    if (0x07 == car_dev_status) {
                        struct ArkProtocol protocol;
                        protocol.type = AT_Car;
                        protocol.data = car;
                        fprintf(stderr, "callback_notify, %d\n", __LINE__);
                        callback_notify(&protocol);
                    }
                    break;
                }
                case ACT_Track: {
                    switch (car->status) {
                    case ACS_TrackOn: {
                        car_dev_val = 0x00;
                        break;
                    }
                    case ACS_TrackOff: {
                        car_dev_val = 0x01;
                        break;
                    }
                    default: {
                        break;
                    }
                    }
                    car_dev_status_tab[5] &= ~(0x01);
                    if (car_dev_val) {
                        car_dev_status_tab[5] |= 0x01;
                    }
                    car_dev_status |= (0x01 << 1);
                    if (0x07 == car_dev_status) {
                        struct ArkProtocol protocol;
                        protocol.type = AT_Car;
                        protocol.data = car;
                        //     fprintf(stderr, "callback_notify, %d\n", __LINE__);
                        callback_notify(&protocol);
                    }
                    break;
                }
                case ACT_AssistTrack: {
                    switch (car->status) {
                    case ACS_AssistTrackOn: {
                        break;
                    }
                    case ACS_AssistTrackOff: {
                        break;
                    }
                    default: {
                        break;
                    }
                    }
                    if (0x07 == car_dev_status) {
                        struct ArkProtocol protocol;
                        protocol.type = AT_Car;
                        protocol.data = car;
                        //    fprintf(stderr, "callback_notify, %d\n", __LINE__);
                        callback_notify(&protocol);
                    }
                    break;
                }
                default: {
                    break;
                }
                }
                //              fprintf(stderr, "1111111111111111111-----%x\n", car_dev_status);
                if (car_dev_status == 0x07) {
                    unsigned char buffer[32] = {0x00};
                    unsigned char crc_index = 0;
                    buffer[crc_index++] = 0x0D;
                    buffer[crc_index++] = 0x24;
                    buffer[crc_index++] = 0x07;
                    buffer[crc_index++] = 0x06;
                    int i;
                    unsigned char start_index = crc_index;
                    for (i = 0; i < 6; ++i) {
                        buffer[i + start_index] = car_dev_status_tab[i];
                        //                        fprintf(stderr, "%s, %d,  i + crc_index = %x %x %x\n", __PRETTY_FUNCTION__, __LINE__, i + crc_index, buffer[i + crc_index], car_dev_status_tab[i]);
                        crc_index++;
                    }
                    buffer[crc_index] ^= 0xFF;
                    for (i = 2; i < 10; ++i) {
                        buffer[crc_index] ^= buffer[i];
                        //                        fprintf(stderr, "buffer[i] = %x\n", buffer[i]);
                    }
                    //                    fprintf(stderr, "buffer[crc_index] = %x\n", buffer[crc_index]);
                    //                    for (i = 0; i < (crc_index + 1); ++i) {
                    //                        fprintf(stderr, "ggggg  %d   %x\n", i, buffer[i]);
                    //                    }
                    //   fprintf(stderr, "write, %d\n", __LINE__);
                    ret = write(command_serial_port_fd, buffer, crc_index + 1);
                }

            }
            break;
        }
        case AT_Audio: {
            LOG_FFLDEBUG("\n");
            struct ArkAudio* audio = (struct ArkAudio*)protocol->data;
            if (NULL != audio) {
                LOG_FFLDEBUG("\n");
                switch (audio->type) {
                case AAT_Idle: {
                    break;
                }
                case AAT_Radio: {
                    LOG_FFLDEBUG("\n");
                    if (1 != gpio_value) {
                        LOG_FFLDEBUG("\n");
                        gpio_value = 1;
                        set_value_gpio(69, gpio_value);
                    }
                    break;
                }
                case AAT_Music:
                case AAT_Video:
                case AAT_Carplay:
                case AAT_CarlifeMusic: {
                    if (0 != gpio_value) {
                        gpio_value = 0;
                        set_value_gpio(69, gpio_value);
                    }
                    unsigned char buffer[32] = {0x00};
                    unsigned char crc_index = 0;
                    buffer[crc_index++] = 0x0D;
                    buffer[crc_index++] = 0x24;
                    buffer[crc_index++] = 0x02;
                    buffer[crc_index++] = 0x02;
                    buffer[crc_index++] = 0x0B;
                    int i;
                    buffer[crc_index] ^= 0xFF;
                    for (i = 2; i < 6; ++i) {
                        buffer[crc_index] ^= buffer[i];
                    }
                    ret = write(command_serial_port_fd, buffer, crc_index + 1);
                    for (i = 0; i < crc_index + 1; ++i) {
                        LOG_DEBUG("%d %x\n", i, buffer[i]);
                    }
                    LOG_DEBUG("\n");
                    break;
                }
                case AAT_CarlifePhone: {
                    break;
                }
                default: {
                    break;
                }
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

enum ArkCustomer xzd_init()
{
    //  fprintf(stderr, "xzd_init\n");
    char* customer_id = (char*)getenv("PROTOCOL_ID");
    if (NULL != customer_id) {
        if (0 == strcmp(customer_id, "XZD")) {
            export_gpio(69);
            direction_gpio(69, "out");
            gpio_value = 1;
            set_value_gpio(69, gpio_value);
        } else return AC_Undefine;
    } else return AC_Undefine;

    char* touch_serial_port_name = (char*)getenv("TOUCHSERIAL");

    if (NULL != touch_serial_port_name) {
        touch_serial_port_fd = open(touch_serial_port_name, O_RDWR);
        if (touch_serial_port_fd < 0) {
            fprintf(stderr, "open %s error\n", touch_serial_port_name);
            return AC_Error;
        } else {
            int baudrate = atoi((char*)getenv("TOUCHSERIAL_BAUDRATE"));
            set_speed(touch_serial_port_fd, baudrate);
            if (set_parity(touch_serial_port_fd, TOUCH_DATA_BITS, TOUCH_STOP_BITS, TOUCH_PARITY_STATUS) == false) {
                printf("uart_touch_init: Set parity error\n");
                return AC_Error;
            }
        }
    }

    char* command_serial_port_name = (char*)getenv("COMMANDSERIAL");
    if (NULL != command_serial_port_name) {
        command_serial_port_fd = open(command_serial_port_name, O_RDWR);
        if (command_serial_port_fd < 0) {
            fprintf(stderr, "open command_serial_port_name:%s error\n", command_serial_port_name);
        } else {
            char* cmd_baudrate = (char*)getenv("COMMANDSERIAL_BAUDRATE");
            if (NULL != cmd_baudrate) {
                int baudrate = atoi(cmd_baudrate);
                set_speed(command_serial_port_fd, baudrate);
                if (set_parity(command_serial_port_fd, TOUCH_DATA_BITS, TOUCH_STOP_BITS, TOUCH_PARITY_STATUS) == false) {
                    printf("uart_touch_init: Set parity error\n");
                }
            }
        }
    }

    xzd.fd[0] = touch_serial_port_fd;
    xzd.fd[1] = command_serial_port_fd;
    //  fprintf(stderr, "xzd.fd[0]=%d\n", xzd.fd[0]);
    // fprintf(stderr, "xzd.fd[1]=%d\n", xzd.fd[1]);
    if (-1 == (input_fd = open(INPUT_DEVICE_PATH, O_WRONLY))) {
        fprintf(stderr, "open %s error\n", INPUT_DEVICE_PATH);
        return AC_Error;
    }

    return AC_XZD;
}

int xzd_uninit()
{
    fprintf(stderr, "xzd_uninit\n");
    unexport_gpio(69);

    if (-1 != touch_serial_port_fd) {
        close(touch_serial_port_fd);
        touch_serial_port_fd = -1;
    }
    if (-1 != command_serial_port_fd) {
        close(command_serial_port_fd);
        command_serial_port_fd = -1;
    }
    if (-1 != input_fd) {
        close(input_fd);
        input_fd = -1;
    }

    return 0;
}

struct CustomerProtocol xzd = {
    .customer = AC_XZD,
    .fd = {-1, -1, -1},
    .init = xzd_init,
    .read = xzd_touch_command_read,
    .write = xzd_write,
    .uninit = xzd_uninit,
};
