#ifndef ARKNAVIPROTOCOL_H
#define ARKNAVIPROTOCOL_H

#include "ArkUserInterface.h"
#include "ArkKey.h"
#include "ArkCar.h"
#include "ArkAudio.h"
#include "ArkRadio.h"
#include "ArkBluetooth.h"
#include "ArkLink.h"
#include "ArkMultimedia.h"
#include "ArkControl.h"
#include "list.h"

//#define WRITE_QUEUE

#define DEBUG_ON
#ifdef DEBUG_ON
#define LOG_DEBUG(fmt, args...) \
    do {\
        printf(fmt, ##args);\
    } while (0)
#define LOG_FFLDEBUG(fmt, args...) \
    do {\
        printf("%s<-%s<-%d:", __FILE__, __FUNCTION__, __LINE__);\
        printf(fmt, ##args);\
    } while (0)
#else
#define LOG_DEBUG(fmt, args...)
#define LOG_FFLDEBUG(fmt, args...)
#endif

#define AC_MAX_COMFD_NUM    5

#define INPUT_DEVICE_PATH "/dev/input/event0"
#define TOUCH_DATA_BITS 8
#define TOUCH_STOP_BITS 1
#define TOUCH_PARITY_STATUS 'n'

enum ArkType {
    AT_Undefine = -1,
    AT_Error,
    AT_UserInterface,
    AT_Key,
    AT_Car,
    AT_Radio,
    AT_Bluetooth,
    AT_Link,
    AT_Multimedia,
    AT_Control,
    AT_Audio,
};

enum ArkDirection {
    AD_Undefine = -1,
    AD_Setter,
    AD_Getter,
    AD_Notify,
};

enum ArkPriority {
    AP_Undefine = -1,
//    AP_PRIO_IDLE,
//    AP_PRIO_ABOVE_IDLE,
//    AP_PRIO_LOWEST,
//    AP_PRIO_BELOW_NORMAL,
    AP_PRIO_NORMAL,
    AP_PRIO_ABOVE_NORMAL,
    AP_PRIO_HIGHEST,
    AP_PRIO_TIME_CRITICAL,
    AP_PRIO_MAX,
};

struct ArkProtocol {
    enum ArkType type;
    enum ArkDirection direction;
    enum ArkPriority prio;
    void* data;
    struct list_head list;
};

enum ArkCustomer {
    AC_Undefine = -1,
    AC_Error,
    AC_Shenglong,
    AC_XZD,
    AC_YCZXAUDI,
    AC_YCZXBMW,
    AC_YXCX,
    AC_ARK169,
    AC_YAOXI,
};

struct arkkeytype_map {
    unsigned char src_key;
    enum ArkKeyType dest_key;
};

void set_speed(int fd, unsigned int speed);
int set_parity(int fd, int databits, int stopbits, char parity);

//gpio->69
int export_gpio(const unsigned short gpio);
int unexport_gpio(const unsigned short gpio);
//direction->"out"->output; direction->"in"->input
int direction_gpio(const unsigned short gpio, const char *direction);
//value->0->low;value->1->high
int set_value_gpio(const unsigned short gpio, const unsigned short value);
void callback_notify(const struct ArkProtocol* protocol);
int notify_car_event(enum ArkCarType type, enum ArkCarStatus status, unsigned char *data, unsigned char len);
int notify_control_event(enum ArkControlType type, unsigned char *data, unsigned char len);
int notify_radio_event(enum ArkRadioType type, unsigned char *data, unsigned char len);
int notify_bt_event(enum ArkBtType type, unsigned char *data, unsigned char len);
int notify_key_event(enum ArkKeyType type, enum ArkKeyStatus status);

typedef enum ArkCustomer (*CUSTOMERPROTOCOLINITHANDLER) ();
typedef int (*CUSTOMERPROTOCOLREADHANDLER) (const int);
typedef int (*CUSTOMERPROTOCOLWRITEHANDLER) (const struct ArkProtocol*);
typedef int (*CUSTOMERPROTOCOLUNINITHANDLER) ();
struct CustomerProtocol {
    enum ArkCustomer customer;
    int fd[AC_MAX_COMFD_NUM];
    CUSTOMERPROTOCOLINITHANDLER init;
    CUSTOMERPROTOCOLREADHANDLER read;
    CUSTOMERPROTOCOLWRITEHANDLER write;
    CUSTOMERPROTOCOLUNINITHANDLER uninit;
};

typedef void (*ARKPROTOCOLHANDLER) (const struct ArkProtocol*, const void*);

#ifdef __cplusplus
extern "C" {
#endif
enum ArkCustomer ark_protocol_initialize(const ARKPROTOCOLHANDLER callback, const void* callback_arg);
int ark_protocol_uninitialize();
int ark_protocol_send(const struct ArkProtocol* protocol);
#ifdef __cplusplus
}
#endif
#endif //ARKNAVIPROTOCOL_H
