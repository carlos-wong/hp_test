#ifndef ARKCONTROL_H
#define ARKCONTROL_H

enum ArkControlType {
    ACLT_Undefine = -1,
    ACLT_Handshake,
    ACLT_Heartbeat,
    ACLT_SetBaud,
    ACLT_RequestInit,
    ACLT_SetStatusDetect,
    ACLT_QueryDeviceState,
    ACLT_RequestSourceSwitch,
    ACLT_GetSourceSwitch,
    ACLT_SetReset,
    /*-----------------------SWC方控学习协议简单说明-------------------------*/
    //方控学习只能学习9个按键，如下所示，每个索引号和对应的按键是固定的
    //1,V_KEY_PRE, 2,V_KEY_NEXT, 3,C_KEY_VOL_INC, 4,C_KEY_POWER, 5,C_KEY_VOL_DEC,
    //6,V_KEY_MUTE_LAMP, 7,C_KEY_PICK_UP, 8,C_KEY_HANG_UP, 9,C_KEY_MODE
    //方控学习流程，首先发送ACLT_SWCStartSampling开始采样，之后按住方控按键不释放，
    //车机端点击要学习的按键后发送ACLT_SWCStudy命令，参数为一个字节，值为1~9,表示要学习按键的索引号，
    //最后发送ACLT_SWCSave确认。
    //ACLT_SWCStudyOK为收到的消息，参数为一个字节，值为1~9,代表按键的索引号，表示该按键已经完成学习。
    //ACLT_SWCStudyCancel和ACLT_SWCStudyOK类似，只是表示该按键的学习已经取消。
    //ACLT_SWCGetTable用来获取当前方控按键映射表，发送后会收到ACLT_SWCStudyOK和ACLT_SWCStudyCancel消息，
    //可以根据收到的消息来初始化或者更新UI
    ACLT_SWCStartSampling,
    ACLT_SWCExitSampling,
    ACLT_SWCGetTable,
    ACLT_SWCSave,
    ACLT_SWCReset,
    ACLT_SWCStudy,
    ACLT_SWCStudyOK,
    ACLT_SWCStudyCancel,
};

struct ArkControl {
    enum ArkControlType type;
    unsigned char length;
    unsigned char* data;
};

#endif //ARKCONTROL_H
