#ifndef LINK_H
#define LINK_H

#include "BusinessLogic/Audio.h"
#include "DbusService.h"
#include <QObject>
#include <QScopedPointer>
#include <QCoreApplication>

#define PHONEBTADDR  "/tmp/phonebtaddr"

typedef enum
{
   HOME_KEY = 0x01,
   MENU_KEY,
   BACK_KEY,
   MIC_KEY,
   MEDIA_PLAY_KEY,
   MEDIA_PAUSE_KEY,
   MEDIA_NEXT_KEY,
   MEDIA_PREVIOUS_KEY,
   NAVI_KEY,
}KeyValue;
#define KeyValue int

typedef enum
{
    CARPLAY = 0X00,
    CARLIFE = 0x01,
    ANDROID_MIRROR = 0x02,
    ANDROID_CARLIFE = 0x03,
    IOS_CARLIFE = 0x04,
    AUTO = 0x05,
} Link_Type;
#define Link_Type int

enum TouchType {
    TouchBegin = 0x01,
    TouchUpdate = 0x02,
    TouchEnd = 0x03,
};
#define TouchType int

typedef enum
{
    Unknown = 0x0,
    LinkAndroid = 0x01,
    LinkIPhone = 0x02,
}LinkPhoneType;
#define LinkPhoneType int

typedef enum
{
    LINK_UNSUPPORTED= 0xff,     //手机类型未匹配或手机连接中转后台点击另外一个连接模式
    LINK_CONNECTED = 1,         //暂时未用
    LINK_DISCONNECTED = 2,      //请求连接后没连接上，可能没插入手机
    LINK_STARTING = 3,          //发起连接
    LINK_SUCCESS = 4,           //连接成功
    LINK_FAIL = 5,              //连接失败
    LINK_EXITING = 6,           //退到后台
    LINK_EXITED = 7 ,           //退出carlife，完全是释放解码库，切源的时候调用
    LINK_REMOVED = 8,           //手机连接上carlife后拔出手机
    LINK_INSERTED = 9,          //手机插入包括第一次上电
    LINK_NOT_INSERTED = 10,     //手机未进行连接拔出,暂时可以没用
    LINK_NOT_INSTALL = 11,      //未安装APK
    LINK_CALL_PHONE = 12,       //carlife拨打电话，呼入电话和通话中
    LINK_CALL_PHONE_EXITED = 13,//carlife拒绝来电或者挂断电话
    LINK_MUTE =14,              //禁掉声音
    LINK_UNMUTE  = 15,          //恢复声音
    LINK_NODATA = 16,           //苹果手机锁屏或者在后针对iphone5，iphone5s,iphone6等机型
    LINK_VIDEOREADY = 17,       //苹果手机解锁并且在前台针对iphone5，iphone5s,iphone6等机型
    LINK_BT_DISCONNECT = 18,    //断掉蓝牙
    LINK_FAILED_EAP = 19,       //苹果手机eap失败,提示用户要重启手机端carlife
    LINK_FAILED_UNSTART = 20,   //手机端还未拉起完成就通信,可建议用户手机端手动启动carlife
    LINK_AUTO_BT_UNPAIRED = 21, //return the status of vehicle bluetooth with target android phone
    LINK_AUTO_BT_PAIRED = 22,
    LINK_AUTO_BT_REQUEST = 23,
}Link_STATUS;
#define Link_STATUS int

enum WheelButtonstatus{
    WheelButtonCheck = 1,
    WheelButtonNormalAndEnter = 0
};
#define Wheelstatus int

enum WheelStep{
    WheelNext = 1,
    WheelPreviou = -1,
    WheelCurrent = 0
};
#define WheelStep int

typedef enum
{
    KEYCODE_Home = 0x01,				//相当于点击了手机的 Home键
    KEYCODE_Phone_Call = 0x02,				//使用移动设备端 CarLife 应用拨打或者接听电话
    KEYCODE_Phone_End,					//使用移动设备端 CarLife 应用挂断电话
    KEYCODE_HFP = 0x05,					//切换免ᨀ电话和移动设备电话
    KEYCODE_Selector_Next,				//在当前小区域中切换到下一个焦点
    KEYCODE_Selector_Previous = 0x07,			//在当前小区域中切换到上一个焦点
    KEYCODE_Media = 0x09,				//进入移动设备端 CarLife 应用的音乐播放界面
    KEYCODE_Navi = 0x0B,				//进入移动设备端 CarLife 应用的导航界面
    KEYCODE_Back = 0x0E,				//返回到移动设备端 CarLife 应用的上一个界面
    KEYCODE_Seek_Sub = 0x0F,				//播放上一首音乐
    KEYCODE_Seek_Add = 0x10,				//播放下一首音乐
    KEYCODE_Mute = 0x13,				//电话过程中静音
    KEYCODE_OK = 0x14,					//选择焦点以后，按下该键确认点击
    KEYCODE_Move_Left = 0x15,				//焦 点 区 域 左移
    KEYCODE_Move_Right = 0x16,				//焦 点 区 域 右移
    KEYCODE_Move_Up = 0x17,				//焦 点 区 域 上移
    KEYCODE_Move_Down = 0x18,				//焦 点 区 域 下移
    KEYCODE_Move_Up_Left = 0x19,			//左上移动------只在地图底图界面支持
    KEYCODE_Move_Up_Right = 0x1A,			//右上移动------只在地图底图界面支持
    KEYCODE_Move_Down_Left = 0x1B,			//左下移动------只在地图底图界面支持
    KEYCODE_Move_Down_Right = 0x1C,			//右下移动------只在地图底图界面支持
    KEYCODE_TEL = 0x1D,					//切 换 到 电 话
    KEYCODE_MAIN = 0x1E,				//切 换 到 主 页
    KEYCODE_Media_Start = 0x1F,				//播放音乐
    KEYCODE_Media_Stop = 0x20,				//暂停音乐
    KEYCODE_VR_Start = 0x21,				//打开语音
    KEYCODE_VR_Stop = 0x22				//关闭语音
}WheelKeyValue;
#define WheelKeyValue int

struct TouchEvent {
    enum TouchPointState {
        TouchPointPressed    = 0x01,
        TouchPointMoved      = 0x02,
        TouchPointStationary = 0x04,
        TouchPointReleased   = 0x08,
    };
    TouchEvent(){}
    explicit TouchEvent(TouchEvent::TouchPointState state, int x, int y)
        : state(state)
        , y(y)
        , x(x) {}
    ~TouchEvent() {}
    int x;
    int y;
    TouchEvent::TouchPointState state;
};

class LinkPrivate;
class Link : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(Link)
#ifdef g_Link
#undef g_Link
#endif
#define g_Link (Link::instance())
public:
    inline static Link* instance() {
        static Link* link(new Link(qApp));
        return link;
    }
    void startCarlife();
    void startCarplay();
    void startAuto();
    void reset();
    void requestLinkStatus(const int type, const int status);
    void requestTouchStatus(const int deviceType, const int touchType, const QString &touchPointXml);
    void requestKeyValue(const int deviceType, const int type);
    void requestWheelStatus(const int deviceType, const int select_pressed, const int wheelStep);
    void reqeustBTInfo(const int type, const int status);
    Link_Type getLinkType();
    Link_STATUS getLinkStatus();
signals:
    void onLinkStatusChange(const int type, const int status);
private slots:
    void onServiceUnregistered(const QString& service);
    void linkStatusChange(const int type, const int status);
    void onHolderChange(const int oldHolder, const int newHolder);
private:
    explicit Link(QObject* parent = NULL);
    ~Link();
    void initializePrivate();
    friend class LinkPrivate;
    QScopedPointer<LinkPrivate> m_Private;
};

#endif // LINK_H
