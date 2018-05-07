#include "ArkNaviProtocol.h"
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/time.h>
struct timeval cur_tv;

static unsigned char flag = 1;
static enum ArkCustomer customer = AC_Undefine;

static void signalHandler(int signo)
{
    flag = 0;
}

void callback(const struct ArkProtocol* protocol, const void* parameter)
{
    int i;

    //LOG_DEBUG("protocol = %p\n", protocol);
    if (NULL != protocol) {
        //LOG_DEBUG("%s\n", __PRETTY_FUNCTION__, protocol->type);
        switch (protocol->type) {
        case AT_Car:
            if (NULL != protocol->data) {
                struct ArkCar* car = (struct ArkCar*)protocol->data;
                switch (car->type) {
                case ACT_Reversing:
                    if (car->status == ACS_ReversingOn)
                        LOG_DEBUG("reversing on.\n");
                    else if (car->status == ACS_ReversingOff)
                        LOG_DEBUG("reversing off.\n");
                    break;
                case ACT_ILLLight:
                    if (car->status == ACS_ILLLightOn)
                        LOG_DEBUG("ill light on.\n");
                    else if (car->status == ACS_ILLLightOff)
                        LOG_DEBUG("ill light off.\n");
                    break;
                case ACT_Brake:
                    if (car->status == ACS_BrakeOn)
                        LOG_DEBUG("brake on.\n");
                    else if (car->status == ACS_BrakeOff)
                        LOG_DEBUG("brake off.\n");
                    break;
                }
            }
            break;
        case AT_Control:
            if (NULL != protocol->data) {
                struct ArkControl *ctrl = (struct ArkControl*)protocol->data;
                switch (ctrl->type) {
                case ACLT_Heartbeat:
                    LOG_DEBUG("heatbeat tick is %d second.\n", *(ctrl->data));
                    break;
                case ACLT_Handshake:
                    LOG_DEBUG("receive ACLT_Handshake event.\n");
                    break;
                case ACLT_SWCStudyOK:
                    LOG_DEBUG("swc key %d study ok.\n",  *(ctrl->data));
                    break;
                case ACLT_SWCStudyCancel:
                    LOG_DEBUG("swc key %d study cancel.\n",  *(ctrl->data));
                    break;
                default: {
                    for (int i = 0; i < ctrl->length; ++i) {
                        LOG_DEBUG("%s, %x\n", __PRETTY_FUNCTION__, *(ctrl->data + i));
                    }
                    break;
                }
                }
            }
            break;
        case AT_Radio:
            if (NULL != protocol->data) {
                struct ArkRadio *radio = (struct ArkRadio*)protocol->data;
                switch (radio->type) {
                case ART_GetMinFreq:
                    if (NULL != radio->data) {
                        LOG_DEBUG("receive radio current min frequence is %d.\n", (radio->data[0] << 8) | radio->data[1]);
                    }
                    break;
                case ART_GetMaxFreq:
                    if (NULL != radio->data) {
                        LOG_DEBUG("receive radio current max frequence is %d.\n", (radio->data[0] << 8) | radio->data[1]);
                    }
                    break;
                case ART_GetGranularity:
                    if (NULL != radio->data) {
                        LOG_DEBUG("receive radio current granularity is %d.\n", radio->data[0]);
                    }
                    break;
                case ART_GetSignalIntensity:
                    if (NULL != radio->data) {
                        LOG_DEBUG("receive radio current sigal intensity is %d.\n", radio->data[0]);
                    }
                    break;
                case ART_GetStatus:
                    if (NULL != radio->data) {
                        int status = *radio->data;
                        LOG_DEBUG("radio current status is %d.\n", status);
                    }
                    break;
                case ART_GetCurfreqMono:
                    LOG_DEBUG("radio current frequence is mono.\n");
                    break;
                case ART_GetCurfreqStereo:
                    LOG_DEBUG("radio current frequence is stereo.\n");
                    break;
                case ART_GetUseMono:
                    LOG_DEBUG("radio use mono.\n");
                    break;
                case ART_GetUseStereo:
                    LOG_DEBUG("radio use stereo.\n");
                    break;
                case ART_GetAutoScan:
                    LOG_DEBUG("radio scan mode is auto.\n");
                    break;
                case ART_GetManualScan:
                    LOG_DEBUG("radio scan mode is manual.\n");
                    break;
                case ART_GetLOC:
                    LOG_DEBUG("radio is LOC now.\n");
                    break;
                case ART_GetDX:
                    LOG_DEBUG("radio is DX now.\n");
                    break;
                case ART_GetBand:
                    if (NULL != radio->data) {
                        LOG_DEBUG("radio current band is %d.\n", *radio->data);
                    }
                    break;
                case ART_GetPreset:
                    if (NULL != radio->data) {
                        LOG_DEBUG("radio preset channel is %d.\n", *radio->data);
                    }
                    break;
                case ART_GetFreq:
                    if (NULL != radio->data) {
                        LOG_DEBUG("radio preset frequence is %d.\n", (radio->data[0] << 8) | radio->data[1]);
                    }
                    struct timeval now;
                    gettimeofday(&now,0);
                    LOG_DEBUG("time-> %d, %d\n", now.tv_sec - cur_tv.tv_sec, now.tv_usec - cur_tv.tv_usec);
                    break;
                case ART_GetPresetList:
                    if (NULL != radio->data) {
                        int i;
                        for (i = 0; i < RADIO_PRESET_NUM; i++) {
                            LOG_DEBUG("radio preset ch[%d] frequence is %d.\n", i, (radio->data[2*i] << 8) | radio->data[2*i + 1]);
                        }
                    }
                    break;
                }
            }
            break;
        case AT_Bluetooth:
            if (NULL != protocol->data) {
                struct ArkBlueTooth *bt = (struct ArkBlueTooth*)protocol->data;
                int len = bt->length;
                switch (bt->type) {
                case ABT_GetStatus:
                    if (NULL != bt->data) {
                        LOG_DEBUG("bt status is %d.\n", *bt->data);
                    }
                    break;
                case ABT_GetLocalName:
                    if (NULL != bt->data) {
                        char name[16];
                        for (i = 0; i < 15 && len > 0; i++,len--)
                            name[i] = bt->data[i];
                        name[i] = 0;
                        LOG_DEBUG("bt lcoal device name is %s.\n", name);
                    }
                    break;
                case ABT_GetRemoteName:
                    if (NULL != bt->data) {
                        char name[16];
                        for (i = 0; i < 15 && len > 0; i++,len--)
                            name[i] = bt->data[i];
                        name[i] = 0;
                        LOG_DEBUG("bt remote device name is %s.\n", name);
                    }
                    break;
                case ABT_GetPinCode:
                    if (NULL != bt->data) {
                        char pincode[5];
                        for (i = 0; i < 4 && len > 0; i++,len--)
                            pincode[i] = bt->data[i];
                        pincode[i] = 0;
                        LOG_DEBUG("bt pincode is %s.\n", pincode);
                    }
                    break;
                case ABT_GetLocalAddr:
                    if (NULL != bt->data) {
                        char addr[16];
                        for (i = 0; i < 15 && len > 0; i++,len--)
                            addr[i] = bt->data[i];
                        addr[i] = 0;
                        LOG_DEBUG("bt local addr is %s.\n", addr);
                    }
                    break;
                case ABT_GetRemoteAddr:
                    if (NULL != bt->data) {
                        char addr[16];
                        for (i = 0; i < 15 && len > 0; i++,len--)
                            addr[i] = bt->data[i];
                        addr[i] = 0;
                        LOG_DEBUG("bt remote addr is %s.\n", addr);
                    }
                    break;
                case ABT_GetPhoneNumberNow:
                    if (NULL != bt->data) {
                        char number[32];
                        for (i = 0; i < 31 && len > 0; i++,len--)
                            number[i] = bt->data[i];
                        number[i] = 0;
                        LOG_DEBUG("phone number now is %s.\n", number);
                    }
                    break;
                case ABT_GetPhoneBookEntry:
                    LOG_DEBUG("get a phonebook enrty.\n");
                    break;
                case ABT_GetReceiveRecordEntry:
                    LOG_DEBUG("get a reveive record enrty.\n");
                    break;
                case ABT_GetDialedRecordEntry:
                    LOG_DEBUG("get a dialed record enrty.\n");
                    break;
                case ABT_GetMissRecordEntry:
                    LOG_DEBUG("get a miss record enrty.\n");
                    break;
                case ABT_SyncPhoneBookEnd:
                case ABT_SyncReceiveRecordEnd:
                case ABT_SyncDialedRecordEnd:
                case ABT_SyncMissRecordEnd:
                    LOG_DEBUG("sync end.\n");
                    break;
                case ABT_GetAutoAnswer:
                    LOG_DEBUG("bt auto answer.\n");
                    break;
                case ABT_GetNotAutoAnswer:
                    LOG_DEBUG("bt not auto answer.\n");
                    break;
                case ABT_GetAutoConnect:
                    LOG_DEBUG("bt auto connect.\n");
                    break;
                case ABT_GetNotAutoConnect:
                    LOG_DEBUG("bt not auto connect.\n");
                    break;
                case ABT_GetA2DPConnect:
                    LOG_DEBUG("a2dp is connected.\n");
                    break;
                case ABT_GetMusicPlaying:
                    LOG_DEBUG("bt music is playing.\n");
                    break;
                case ABT_GetVoiceConnect:
                    LOG_DEBUG("bt voice is connected.\n");
                    break;
                case ABT_GetVoiceDisconnect:
                    LOG_DEBUG("bt voice is disconnected.\n");
                    break;
                case ABT_GetA2DPDisconnect:
                    LOG_DEBUG("a2dp is disconnected.\n");
                    break;
                }
            }
            break;
        case AT_Key:
            if (NULL != protocol->data) {
                struct ArkKey *key = (struct ArkKey*)protocol->data;
                LOG_DEBUG("Key %d status is %d.\n", key->type, key->status);
            }
        }
    }
}

int main(int argc, char* argv[])
{
    gettimeofday(&cur_tv,0);
    if (SIG_ERR == signal(SIGINT, signalHandler)) {
        perror("signal SIGINT error");
    }
    customer = ark_protocol_initialize(callback, NULL);
    LOG_DEBUG("customer = %d\n", customer);
    if (AC_XZD == customer) {
        struct ArkUserInterface launcher;
        launcher.type = AUIT_Launcher;
        launcher.status = AUIS_ArkUserInterfaceInit;
        struct ArkProtocol protocol;
        protocol.type = AT_UserInterface;
        protocol.prio = AP_PRIO_NORMAL;
        protocol.data = (void*)&launcher;
        ark_protocol_send(&protocol);
    } else if (AC_ARK169 == customer) {
        struct ArkControl ctrl;
        ctrl.type = ACLT_Handshake;
        struct ArkProtocol protocol;
        protocol.type = AT_Control;
        protocol.data = (void*)&ctrl;
        protocol.prio = AP_PRIO_NORMAL;
        ark_protocol_send(&protocol);
        sleep(1);
        ark_protocol_send(&protocol);
        struct ArkRadio radio;
        radio.type = ART_QueryFreqInfo;
        protocol.type = AT_Radio;
        protocol.data = (void*)&radio;
        protocol.prio = AP_PRIO_NORMAL;
        ark_protocol_send(&protocol);
        sleep(1);
        radio.type = ART_QueryPresetListInfo;
        ark_protocol_send(&protocol);
        radio.type = ART_KEY_StereoStateChange;
        ark_protocol_send(&protocol);
    }
    while (flag) {
        sleep(10);
        struct ArkControl control;
        memset((void*)&control, 0, sizeof(struct ArkCar));
        struct ArkProtocol protocol;
        memset((void*)&protocol, 0, sizeof(struct ArkProtocol));
        protocol.type = AT_Control;
        protocol.direction = AD_Notify;
        protocol.data = &control;
        control.type = ACLT_Heartbeat;
        ark_protocol_send(&protocol);
    };
    ark_protocol_uninitialize();
    return 0;
}
