#include "ArkProtocolWrapper.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

ArkProtocolWrapper::ArkProtocolWrapper()
{
    type = AT_Undefine;
    direction = AD_Undefine;
    data = NULL;
    handler = false;
    filter = false;
    count = 0;
}

ArkProtocolWrapper::ArkProtocolWrapper(const ArkProtocol &protocol)
{
    type = protocol.type;
    direction = protocol.direction;
    data = NULL;
    handler = false;
    filter = false;
    count = 1;
    switch (type) {
    case AT_Car: {
        struct ArkCar* ptr = (struct ArkCar*)(protocol.data);
        struct ArkCar* tmp = (struct ArkCar*)malloc(sizeof(struct ArkCar));
        memcpy(tmp, ptr, sizeof(struct ArkCar));
        tmp->data = NULL;
        if ((NULL != ptr->data)
                && (ptr->length > 0)) {
            tmp->data = (unsigned char*)malloc(sizeof(unsigned char) * ptr->length);
            memcpy(tmp->data, ptr->data, ptr->length);
        }
        data = tmp;
        break;
    }
    case AT_Control: {
        struct ArkControl* ptr = (struct ArkControl*)(protocol.data);
        struct ArkControl* tmp = (struct ArkControl*)malloc(sizeof(struct ArkControl));
        memcpy(tmp, ptr, sizeof(struct ArkControl));
        tmp->data = NULL;
        if ((NULL != ptr->data)
                && (ptr->length > 0)) {
            tmp->data = (unsigned char*)malloc(sizeof(unsigned char) * ptr->length);
            memcpy(tmp->data, ptr->data, ptr->length);
        }
        data = tmp;
        break;
    }
    case AT_Radio: {
        struct ArkRadio* ptr = (struct ArkRadio*)(protocol.data);
        struct ArkRadio* tmp = (struct ArkRadio*)malloc(sizeof(struct ArkRadio));
        memcpy(tmp, ptr, sizeof(struct ArkRadio));
        tmp->data = NULL;
        if ((NULL != ptr->data)
                && (ptr->length > 0)) {
            tmp->data = (unsigned char*)malloc(sizeof(unsigned char) * ptr->length);
            memcpy(tmp->data, ptr->data, ptr->length);
        }
        data = tmp;
        break;
    }
    case AT_Bluetooth: {
        struct ArkBlueTooth* ptr = (struct ArkBlueTooth*)(protocol.data);
        struct ArkBlueTooth* tmp = (struct ArkBlueTooth*)malloc(sizeof(struct ArkBlueTooth));
        memcpy(tmp, ptr, sizeof(struct ArkBlueTooth));
        tmp->data = NULL;
        if ((NULL != ptr->data)
                && (ptr->length > 0)) {
            tmp->data = (unsigned char*)malloc(sizeof(unsigned char) * ptr->length);
            memcpy(tmp->data, ptr->data, ptr->length);
        }
        data = tmp;
        break;
    }
    case AT_Key: {
        struct ArkKey* ptr = (struct ArkKey*)(protocol.data);
        struct ArkKey* tmp = (struct ArkKey*)malloc(sizeof(struct ArkKey));
        memcpy(tmp, ptr, sizeof(struct ArkKey));
        data = tmp;
        break;
    }
    case AT_Link: {
        struct ArkLink* ptr = (struct ArkLink*)(protocol.data);
        struct ArkLink* tmp = (struct ArkLink*)malloc(sizeof(struct ArkLink));
        memcpy(tmp, ptr, sizeof(struct ArkLink));
        data = tmp;
        break;
    }
    case AT_Multimedia: {
        struct ArkMultimedia* ptr = (struct ArkMultimedia*)(protocol.data);
        struct ArkMultimedia* tmp = (struct ArkMultimedia*)malloc(sizeof(struct ArkMultimedia));
        memcpy(tmp, ptr, sizeof(struct ArkMultimedia));
        data = tmp;
        break;
    }
    case AT_UserInterface: {
        struct ArkUserInterface* ptr = (struct ArkUserInterface*)(protocol.data);
        struct ArkUserInterface* tmp = (struct ArkUserInterface*)malloc(sizeof(struct ArkUserInterface));
        memcpy(tmp, ptr, sizeof(struct ArkUserInterface));
        data = tmp;
        break;
    }
    case AT_Audio: {
        struct ArkAudio* ptr = (struct ArkAudio*)(protocol.data);
        struct ArkAudio* tmp = (struct ArkAudio*)malloc(sizeof(struct ArkBlueTooth));
        memcpy(tmp, ptr, sizeof(struct ArkAudio));
        tmp->data = NULL;
        if ((NULL != ptr->data)
                && (ptr->length > 0)) {
            tmp->data = (unsigned char*)malloc(sizeof(unsigned char) * ptr->length);
            memcpy(tmp->data, ptr->data, ptr->length);
        }
        data = tmp;
        break;
    }
    default: {
        break;
    }
    }
}

ArkProtocolWrapper::ArkProtocolWrapper(const ArkProtocolWrapper &protocol)
{
    type = protocol.type;
    direction = protocol.direction;
    data = NULL;
    handler = protocol.handler;
    filter = protocol.filter;
    data = protocol.data;
    ++const_cast<ArkProtocolWrapper&>(protocol).count;
    count = protocol.count;
}

ArkProtocolWrapper::~ArkProtocolWrapper()
{
    if (--count == 0) {
        if (NULL != data) {
            switch (type) {
            case AT_Car: {
                struct ArkCar* tmp = (struct ArkCar*)(data);
                if ((NULL != tmp->data)
                        && (tmp->length > 0)) {
                    free(tmp->data);
                    data = NULL;
                }
                free(tmp);
                break;
            }
            case AT_Radio: {
                struct ArkRadio* tmp = (struct ArkRadio*)(data);
                if ((NULL != tmp->data)
                        && (tmp->length > 0)) {
                    free(tmp->data);
                    data = NULL;
                }
                free(tmp);
                break;
            }
            case AT_Bluetooth: {
                struct ArkBlueTooth* tmp = (struct ArkBlueTooth*)(data);
                if ((NULL != tmp->data)
                        && (tmp->length > 0)) {
                    free(tmp->data);
                    data = NULL;
                }
                free(tmp);
                break;
            }
            case AT_Control: {
                struct ArkControl* tmp = (struct ArkControl*)(data);
                if ((NULL != tmp->data)
                        && (tmp->length > 0)) {
                    free(tmp->data);
                    data = NULL;
                }
                free(tmp);
                break;
            }
            case AT_UserInterface: {
                struct ArkUserInterface* tmp = (struct ArkUserInterface*)(data);
                free(tmp);
                break;
            }
            case AT_Audio: {
                struct ArkAudio* tmp = (struct ArkAudio*)(data);
                if ((NULL != tmp->data)
                        && (tmp->length > 0)) {
                    free(tmp->data);
                    data = NULL;
                }
                free(tmp);
                break;
            }
            case AT_Key: {
                struct ArkKey* tmp = (struct ArkKey*)(data);
                free(tmp);
                break;
            }
            case AT_Link: {
                struct ArkLink* tmp = (struct ArkLink*)(data);
                free(tmp);
                break;
            }
            case AT_Multimedia: {
                struct ArkMultimedia* tmp = (struct ArkMultimedia*)(data);
                free(tmp);
                break;
            }
            default: {
                break;
            }
            }
        }
    }
}

struct ArkCar* arkProtocolWrapperToArkCar(const ArkProtocolWrapper &protocol)
{
    struct ArkCar* car = NULL;
    if ((NULL != protocol.data)
            && (AT_Car == protocol.type)) {
        car = static_cast<struct ArkCar*>(protocol.data);
    }
    return car;
}

struct ArkRadio *arkProtocolWrapperToArkRadio(const ArkProtocolWrapper &protocol)
{
    struct ArkRadio* radio = NULL;
    if ((NULL != protocol.data)
            && (AT_Radio == protocol.type)) {
        radio = static_cast<struct ArkRadio*>(protocol.data);
    }
    return radio;
}

struct ArkKey *arkProtocolWrapperToArkKey(const ArkProtocolWrapper &protocol)
{
    struct ArkKey* key = NULL;
    if ((NULL != protocol.data)
            && (AT_Key == protocol.type)) {
        key = static_cast<struct ArkKey*>(protocol.data);
    }
    return key;
}

struct ArkControl* arkProtocolWrapperToArkControl(const ArkProtocolWrapper &protocol)
{
    struct ArkControl* control = NULL;
    if ((NULL != protocol.data)
            && (AT_Control == protocol.type)) {
        control = static_cast<struct ArkControl*>(protocol.data);
    }
    return control;
}

struct ArkBlueTooth *arkProtocolWrapperToArkBluetooth(const ArkProtocolWrapper &protocol)
{
    struct ArkBlueTooth* bluetooth = NULL;
    if ((NULL != protocol.data)
            && (AT_Bluetooth == protocol.type)) {
        bluetooth = static_cast<struct ArkBlueTooth*>(protocol.data);
    }
    return bluetooth;
}
