#include "ArkNaviProtocol.h"
#include "customers/customers.h"
#include "GPIOSet.h"
#include <stddef.h>
#include <stdio.h>
#include <assert.h>
#include <linux/input.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termio.h>
#include <stdbool.h>
#include <string.h>
#include <bits/types.h>
#include <pthread.h>
#include <semaphore.h>

static int speed_arr[9] = {B115200, B57600, B38400, B19200, B9600, B4800, B2400, B1200, B300};
static int name_arr[9] = {115200, 57600, 38400,  19200,  9600,  4800,  2400,  1200,  300};

static bool protocal_init = false;
static ARKPROTOCOLHANDLER notify_callback = NULL;
static void* notify_callback_arg = NULL;
static pthread_t select_pthread = 0;
static unsigned char select_pthread_exit_flag = 1;
#ifdef WRITE_QUEUE
static pthread_t write_pthread = 0;
static unsigned char write_pthread_exit_flag = 1;
static sem_t write_sem;
static pthread_mutex_t list_mutex;
static struct list_head protocol_list[AP_PRIO_MAX];
#endif
static struct CustomerProtocol customer = {AC_Undefine, {-1}, NULL, NULL, NULL, NULL,};

void callback_notify(const struct ArkProtocol* protocol)
{
    if (NULL != notify_callback) {
        notify_callback(protocol, notify_callback_arg);
    }
}

static int notify_event(enum ArkType prtl_type, enum ArkDirection dir,
                        enum ArkPriority prio, void *data)
{
    struct ArkProtocol prtl;
    prtl.type = prtl_type;
    prtl.direction = dir;
    prtl.prio = prio;
    prtl.data = data;
    callback_notify(&prtl);

    return 0;
}

int notify_control_event(enum ArkControlType type, unsigned char *data, unsigned char len)
{
    struct ArkControl ctrl;
    ctrl.type = type;
    ctrl.data = data;
    ctrl.length = len;
    notify_event(AT_Control, AD_Getter, AP_PRIO_NORMAL, &ctrl);
}

int notify_radio_event(enum ArkRadioType type, unsigned char *data, unsigned char len)
{
    struct ArkRadio radio;
    radio.type = type;
    radio.data = data;
    radio.length = len;
    notify_event(AT_Radio, AD_Getter, AP_PRIO_NORMAL, &radio);
}

int notify_car_event(enum ArkCarType type, enum ArkCarStatus status, unsigned char *data, unsigned char len)
{
    struct ArkCar car;
    car.type = type;
    car.status = status;
    car.data = data;
    car.length = len;
    notify_event(AT_Car, AD_Getter, AP_PRIO_NORMAL, &car);
}

int notify_bt_event(enum ArkBtType type, unsigned char *data, unsigned char len)
{
    struct ArkBlueTooth bt;
    bt.type = type;
    bt.data = data;
    bt.length = len;
    notify_event(AT_Bluetooth, AD_Getter, AP_PRIO_NORMAL, &bt);
}

int notify_key_event(enum ArkKeyType type, enum ArkKeyStatus status)
{
    struct ArkKey key;
    key.type = type;
    key.status = status;
    notify_event(AT_Key, AD_Getter, AP_PRIO_NORMAL, &key);
}

static void* select_loop(void* arg)
{
    fd_set select_set;
    int maximum_fd = -1;
    int i;
    struct timeval tv;

    for (i = 0; i < AC_MAX_COMFD_NUM; i++) {
        LOG_DEBUG("customer.fd[%d] = %d\n", i, customer.fd[i]);
        if (customer.fd[i] > maximum_fd)  maximum_fd = customer.fd[i];
    }

    if (-1 == maximum_fd) return;

    int ret = 0;
    while (0 == select_pthread_exit_flag) {
        tv.tv_sec = 0;
        tv.tv_usec = 100 * 1000;
        FD_ZERO(&select_set);
        for (i = 0; i < AC_MAX_COMFD_NUM; i++) {
            if (-1 != customer.fd[i]) {
                FD_SET(customer.fd[i], &select_set);
            }
        }
        ret = select(maximum_fd + 1, &select_set, NULL, NULL, &tv);
        if (ret > 0) {
            for (i = 0; i < AC_MAX_COMFD_NUM; i++) {
                if(-1 != customer.fd[i] && FD_ISSET(customer.fd[i], &select_set)) {
                    customer.read(customer.fd[i]);
                }
            }
        }else if(0 == ret)
        {
            customer.read(-1);
        }
    }
    return NULL;
}

#ifdef WRITE_QUEUE
static void* write_loop(void* arg)
{
    int i;
    while (0 == write_pthread_exit_flag) {
        sem_wait(&write_sem);
        pthread_mutex_lock(&list_mutex);
        for (i = AP_PRIO_MAX - 1; i >= 0; i--) {
            if (!list_empty(&protocol_list[i])) {
                struct ArkProtocol* protocol = list_first_entry(&protocol_list[i], struct ArkProtocol, list);
                if (NULL != protocol) {
                    customer.write(protocol);
                    list_del(&protocol->list);
                    free(protocol);
                }
                break;
            }
        }
        pthread_mutex_unlock(&list_mutex);
    }
    return NULL;
}
#endif

void set_speed(int fd, unsigned int speed)
{
    struct termios opt;
    int status;
    int i;
    tcgetattr(fd, &opt);
    for (i = 0; i < sizeof(speed_arr) / sizeof(int); ++i) {
        if (speed == name_arr[i]) {
            tcflush(fd, TCIOFLUSH);
            cfsetispeed(&opt, speed_arr[i]);	//set baud rate again
            cfsetospeed(&opt, speed_arr[i]);
            status = tcsetattr(fd, TCSANOW, &opt);
            if(status != 0) {
                perror("tcsetarrt fd");
                return;
            }
            tcflush(fd, TCIOFLUSH);
        }
    }
}

int set_parity(int fd, int databits, int stopbits, char parity)
{
    struct termios opt;

    if(tcgetattr(fd, &opt) != 0) {
        perror("tcgetattr");
        return false;
    }

    opt.c_cflag &= ~CSIZE;

    //原始模式：不经回车或换行命令，直接发送
    opt.c_oflag &= ~OPOST;	//output
    opt.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);


    opt.c_cflag |= (CLOCAL | CREAD);


    //屏蔽流控制
    opt.c_iflag &= ~ (IXON | IXOFF | IXANY);

    //避免回车和换行当成同一个字符
    opt.c_iflag &= ~ (INLCR | ICRNL | IGNCR);
    opt.c_oflag &= ~(ONLCR | OCRNL);

    //set data bit
    switch (databits) {
    case 7: {
        opt.c_cflag |= CS7;
        break;
    }
    case 8: {
        opt.c_cflag |= CS8;
        break;
    }
    default: {
        LOG_DEBUG("Unsupport data bit");
        return false;
        break;
    }
    }

    //set parity
    switch (parity) {
    case 'n':
    case 'N': {
        opt.c_cflag &= ~PARENB;	//clear parity enable
        opt.c_iflag &= ~INPCK;	//enable parity checking
        break;
    }
    case 'o':
    case 'O': {
        opt.c_cflag |= (PARODD | PARENB); /* 设置为奇校验*/
        opt.c_iflag |= INPCK;             /* Disnable parity checking */
        break;
    }
    case 'e':
    case 'E': {
        opt.c_cflag |= PARENB;     /* Enable parity */
        opt.c_cflag &= ~PARODD;   /* 转换为偶校验*/
        opt.c_iflag |= INPCK;       /* Disnable parity checking */
        break;
    }
    case 's':
    case 'S': {
        opt.c_cflag &= ~PARENB;
        opt.c_cflag &= ~CSTOPB;
        break;
    }
    default: {
        LOG_DEBUG("Unsupport parity\n");
        return false;
    }
    }

    //set stop bit
    switch (stopbits) {
    case 1: {
        opt.c_cflag &= ~CSTOPB;
        break;
    }
    case 2: {
        opt.c_cflag |= CSTOPB;
        break;
    }
    default: {
        LOG_DEBUG("Unsupport stop bit\n");
        return false;
    }
    }

    //set input parity option
    if(parity != 'n') {
        opt.c_iflag |= INPCK;
    }

    tcflush(fd, TCIFLUSH);	//

    opt.c_cc[VTIME] = 1; /* set timeout 1 100ms*/
    opt.c_cc[VMIN] = 0; /* define the minimum bytes data to be readed*/

    if (tcsetattr(fd, TCSANOW, &opt) != 0) {
        perror("tcsetattr");
        return false;
    }
    return true;
}

enum ArkCustomer ark_protocol_initialize(const ARKPROTOCOLHANDLER callback, const void* callback_arg)
{
    int i;

    if (protocal_init) return;
#ifdef WRITE_QUEUE
    for (i = 0; i < AP_PRIO_MAX; i++) {
        INIT_LIST_HEAD(&protocol_list[i]);
    }

    sem_init(&write_sem, 0, 0);
    pthread_mutex_init(&list_mutex, NULL);
    write_pthread_exit_flag = 0;
#endif
    notify_callback = (void*)callback;
    notify_callback_arg = (void*)callback_arg;
    if (NULL == notify_callback) {
        LOG_DEBUG("ark_protocol_initialize callback is null!\n");
    }
    select_pthread_exit_flag = 0;
    enum ArkCustomer ret = AC_Undefine;
    for (i = 0; (NULL != customer_array[i]) && (i < sizeof(customer_array) / sizeof(customer_array[0])); ++i) {
        LOG_FFLDEBUG("\n");
        if (NULL != customer_array[i]->init) {
            ret = customer_array[i]->init();
            if (AC_Undefine != ret) {
                customer.customer = customer_array[i]->customer;
                int j;
                for (j = 0; j < AC_MAX_COMFD_NUM; ++j) {
                    customer.fd[j] = customer_array[i]->fd[j];
                }
                customer.init = customer_array[i]->init;
                customer.read = customer_array[i]->read;
                customer.write = customer_array[i]->write;
                customer.uninit = customer_array[i]->uninit;
                break;
            }
        }
    }
    if ((AC_Undefine != customer.customer)
            && (AC_Error != customer.customer)) {
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        if (0 == pthread_attr_setschedpolicy(&attr, SCHED_RR)) {
            struct sched_param param;
            param.sched_priority = sched_get_priority_max(SCHED_RR);
            pthread_attr_setschedparam(&attr, &param);
            pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
        }
        if (0 != pthread_create(&select_pthread, &attr, (void*(*)(void*))select_loop, NULL)) {
            LOG_DEBUG("pthread_create select_pthread error\n");
        } else {
        }
#ifdef WRITE_QUEUE
        if (0 != pthread_create(&write_pthread, &attr, (void*(*)(void*))write_loop, NULL)) {
            LOG_DEBUG("pthread_create write_pthread error\n");
        } else {
        }
#endif
        pthread_attr_destroy(&attr);
    }

    protocal_init = true;
    return customer.customer;
}

int ark_protocol_send(const struct ArkProtocol* protocol)
{
    LOG_FFLDEBUG("\n");
    int ret = 0;
    if ((NULL != protocol)
            && (NULL != protocol->data)) {
        if ((AC_Undefine != customer.customer)
                && (AC_Error != customer.customer)
                && (protocol->prio >= 0) && (protocol->prio < AP_PRIO_MAX)) {
            LOG_FFLDEBUG("\n");
#ifdef WRITE_QUEUE
            struct ArkProtocol* prtl = malloc(sizeof(struct ArkProtocol));
            if (NULL != prtl) {
                memcpy(prtl, protocol, sizeof(struct ArkProtocol));
                pthread_mutex_lock(&list_mutex);
                list_add_tail(&prtl->list, &protocol_list[prtl->prio]);
                LOG_FFLDEBUG("\n");
                pthread_mutex_unlock(&list_mutex);
                sem_post(&write_sem);
            }
#else
            if (NULL != customer.write) {
                customer.write(protocol);
            } else {
                LOG_DEBUG("NULL == customer.write\n");
            }
        }
#endif
        LOG_FFLDEBUG("\n");
    } else {
        LOG_FFLDEBUG("\n");
        LOG_DEBUG("ark_protocol_send error: protocol error!\n");
        ret = -1;
    }
    LOG_FFLDEBUG("\n");
    return ret;
}

int ark_protocol_uninitialize()
{
    if (0 != select_pthread) {
        select_pthread_exit_flag = 1;
        pthread_join(select_pthread, NULL);
        select_pthread = 0;
    }
#ifdef WRITE_QUEUE
    if (0 != write_pthread) {
        write_pthread_exit_flag = 1;
        sem_post(&write_sem);
        pthread_join(write_pthread, NULL);
        write_pthread = 0;
    }
#endif
    if (customer.uninit) {
        customer.uninit();
    }

    customer.customer = AC_Undefine;
    customer.init = NULL;
    customer.read = NULL;
    customer.write = NULL;
    customer.uninit = NULL;
    notify_callback = NULL;
#ifdef WRITE_QUEUE
    pthread_mutex_destroy(&list_mutex);
    sem_destroy(&write_sem);

    struct ArkProtocol *prtl;
    int i;
    for (i = 0; i < AP_PRIO_MAX; i++) {
        list_for_each_entry(prtl, &protocol_list[i], list) {
            list_del(prtl);
            free(prtl);
        }
    }
#endif
    return 0;
}

int export_gpio(const unsigned short gpio)
{
    gpio_export(gpio);
}

int unexport_gpio(const unsigned short gpio)
{
    gpio_unexport(gpio);
}

int direction_gpio(const unsigned short gpio, const char *direction)
{
    gpio_set_dir(gpio, direction);
}

int set_value_gpio(const unsigned short gpio, const unsigned short value)
{
    gpio_set_value(gpio, value);
}
