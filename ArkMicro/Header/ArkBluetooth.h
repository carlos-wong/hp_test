#ifndef ARKBLUETOOTH_H
#define ARKBLUETOOTH_H

enum ArkBtType {
    ABT_Undefine = -1,
    ABT_GetStatus,              //获取到蓝牙当前状态，参数为一个字节，值为enum ArkBtStatus
    ABT_GetLocalName,           //获取到本地蓝牙设备名称，参数长度为len的值
    ABT_GetRemoteName,          //获取到远程连接蓝牙设备名称，参数长度为len的值
    ABT_GetPinCode,             //获取到蓝牙PinCode码，参数为4个字节
    ABT_GetLocalAddr,           //获取到本地蓝牙地址，参数长度为len的值
    ABT_GetRemoteAddr,          //获取到远程连接蓝牙地址，参数长度为len的值
    ABT_GetA2DPConnect,         //获取到A2DP已连接
    ABT_GetA2DPDisconnect,      //获取到A2DP已断开
    ABT_GetMusicPlaying,        //获取到蓝牙音乐正播放
    ABT_GetVoiceConnect,        //获取到语音连接建立
    ABT_GetVoiceDisconnect,     //获取到语音连接断开
    ABT_GetOutgoingNumber,      //获取到当前拨号或来电或通话中的电话号码，参数长度为len的值
    ABT_GetIncomingNumber,      //获取到当前拨号或来电或通话中的电话号码，参数长度为len的值
    ABT_GetPhoneNumberNow,      //获取到当前拨号或来电或通话中的电话号码，参数长度为len的值
    ABT_GetPhoneBookEntry,      //获取到一条电话本记录，参数长度是len，数据为UTF-8编码，姓名和电话号码之间用0xFF隔开
    ABT_GetReceiveRecordEntry,  //获取到一条已接来电记录，参数长度是len，数据为UTF-8编码，姓名和电话号码之间用0xFF隔开
    ABT_GetDialedRecordEntry,   //获取到一条已拨电话记录，参数长度是len，数据为UTF-8编码，姓名和电话号码之间用0xFF隔开
    ABT_GetMissRecordEntry,     //获取到一条未接来电记录，参数长度是len，数据为UTF-8编码，姓名和电话号码之间用0xFF隔开
    ABT_GetAutoAnswer,          //获取到自动应答已设置
    ABT_GetNotAutoAnswer,       //获取到自动应答已取消
    ABT_GetAutoConnect,         //获取到自动连接已设置
    ABT_GetNotAutoConnect,      //获取到自动连接已取消
    ABT_SyncPhoneBook,          //发起开始同步电话本,需要注意的是，发起某个同步命令后，在没有接到同步结束通知之前再发起其他同步命令是无效的
    ABT_SyncPhoneBookCancelling,
    ABT_SyncPhoneBookEnd,       //获取到同步结束通知,不管是电话本还是各种通话记录同步完成后都会收到该通知
    ABT_SyncReceiveRecord,      //发起开始同步已拨电话记录
    ABT_SyncReceiveRecordCancelling,
    ABT_SyncReceiveRecordEnd,   //获取到同步结束通知,不管是电话本还是各种通话记录同步完成后都会收到该通知
    ABT_SyncDialedRecord,       //发起开始同步已拨电话记录
    ABT_SyncDialedRecordCancelling,
    ABT_SyncDialedRecordEnd,    //获取到同步结束通知,不管是电话本还是各种通话记录同步完成后都会收到该通知
    ABT_SyncMissRecord,         //发起开始同步未接来电记录
    ABT_SyncMissRecordCancelling,
    ABT_SyncMissRecordEnd,      //获取到同步结束通知,不管是电话本还是各种通话记录同步完成后都会收到该通知
    ABT_CancelSync,             //取消同步
    ABT_Connect,                //连接
    ABT_DisConnect,             //断开连接
    ABT_PickUp,                 //接听电话
    ABT_RejectCall,             //拒绝来电
    ABT_Hangup,                 //挂断电话
    ABT_Redial,                 //重播
    ABT_VoicePhone,             //语音在蓝牙和手机间切换
    ABT_VoiceBluetooth,         //语音在蓝牙和手机间切换
    ABT_VoiceSwitch,            //语音在蓝牙和手机间切换
    ABT_DialPhone,              //拨打电话,参数长度是len，数据为拨出的电话号码,如:10086,
    ABT_DialNumber,             //拨分机号,参数长度是len，数据为按下的单个号码,如:1,*,#
    ABT_MusicPlay,              //播放蓝牙音乐
    ABT_MusicPause,             //暂停蓝牙音乐播放
    ABT_MusicStop,              //停止蓝牙音乐播放
    ABT_MusicNext,              //播放蓝牙音乐下一曲
    ABT_MusicPre,               //播放蓝牙音乐上一曲
    ABT_SetLocalName,           //设置蓝牙设备名称,参数长度是len，数据为设备名称字符
    ABT_SetPinCode,             //设置蓝牙设备pincode,参数长度是4，数据为数字字符
    ABT_SetAutoAnswer,          //设置自动应答
    ABT_CancelAutoAnswer,       //取消自动应答
    ABT_SetAutoConnect,         //设置自动连接
    ABT_CancelAutoconnect,      //取消自动连接
    ABT_QueryAnsConCfg,         //查询自动应答，自动连接设置
    ABT_PairedMode,             //进入配对模式
    ABT_CancelPairedMode,       //取消配对模式
    ABT_MusicName,              //a2dp歌曲名
    ABT_PairedListName,         //配对列表,数据为UTF-8编码，索引和名字之间用0xFF隔开
    ABT_DeletePairedListIndex,  //删除配对列表索引
};

enum ArkBtStatus {
    ABS_Undefine = -1,
    ABS_IDLE,           //未连接
    ABS_CONNECTING,     //正在连接
    ABS_CONNECTED,      //已连接
    ABS_OUTGOING,       //拨号中
    ABS_INCOMING,       //来电中
    ABS_TALKING,        //通话中
};

enum ArkBtSyncType {
    ABST_Undefine = -1,
    ABST_PhoneBook,
    ABST_ReceiveRecord,
    ABST_DialedRecord,
    ABST_MissRecord,
};

struct ArkBlueTooth {
    enum ArkBtType type;
    unsigned char length;
    unsigned char* data;
};

struct BtInfo {
    enum ArkBtStatus status;
};

#endif // ARKBLUETOOTH_H
