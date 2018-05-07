#ifndef DBUSSERVER_H
#define DBUSSERVER_H

#include <QCoreApplication>
#include <QObject>
#include <QTimer>
#include <QDomDocument>
#include <QList>
#include "icarplayer.h"
#include "carlife.h"

#define ArkMicroLinkService     "com.arkmicro.carlife"
#define ArkMicroLinkPath        "/com/arkmicro/carlife"
#define ArkMicroLinkInterface   "Local.DbusServer.Carlife"

//Signals
//#define ArkMicroLinkSignals     "arkLinkProtocolSignal"


typedef enum
{
    EV_UNSUPPORTED=0XFF,
    EV_DOWN = 0x01,
    EV_UP,
    EV_MOVE ,
    EV_MULTIMOVE,
}TouchStatus;
#define TouchStatus int


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

}Link_Type;
#define Link_Type int



//#define LinkMode int

typedef enum
{
    LINK_UNSUPPORTED=0XFF,
    LINK_CONNECTED = 1,
    LINK_DISCONNECTED= 2,
    LINK_STARTING = 3,
    LINK_SUCCESS = 4,
    LINK_FAIL = 5,
    LINK_EXITING = 6,
    LINK_EXITED = 7 ,
    LINK_REMOVED = 8,
    LINK_INSERTED = 9,
    LINK_NOT_INSERTED = 10,
    LINK_NOT_INSTALL = 11,
    LINK_CALL_PHONE = 12,
    LINK_CALL_PHONE_EXITED = 13,
}Link_STATUS;

#define Link_STATUS int

enum TouchType {
    TouchBegin = 0x01,
    TouchUpdate = 0x02,
    TouchEnd = 0x03,
};
#define TouchType int

struct TouchEvent
{
    enum TouchPointState {
        TouchPointPressed    = 0x01,
        TouchPointMoved      = 0x02,
        TouchPointStationary = 0x04,
        TouchPointReleased   = 0x08,
    };
    TouchEvent(){}
    TouchEvent(TouchEvent::TouchPointState state, int x, int y)
        : state(state)
        , y(y)
        , x(x) {}
    ~TouchEvent() {}
    int x;
    int y;
    TouchEvent::TouchPointState state;
};


extern int g_bDoExit;
extern bool g_bSecond;
extern int g_srcWidth;
extern int g_srcHeight;

class DbusServer : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(DbusServer)
    Q_CLASSINFO("D-Bus Interface", "Local.DbusServer.Carlife")
#define g_Link (DbusServer::instance())
public:
    explicit DbusServer(QObject *parent = NULL);
    ~DbusServer();
    inline static DbusServer& instance() {
        static DbusServer* dbusServer(new DbusServer(qApp));
        return *dbusServer;
    }
    void initialize();
    void set_player(ICarPlayer *player);

    void sendLinkStatus(const Link_Type type, const Link_STATUS status);

    void sendLinkBTStatus(const Link_Type type, int status);

    ICarPlayer *m_player;

    int m_src_x[2];
    int m_src_y[2];
    bool m_bInit;
    int  m_linktype;

    pthread_t m_play_thread;
    bool playthread();
    static void* playthreadfunc(void *me);

public slots:
    void onExitSignal();
    void onSendSignal(int type, int status);
    void onSendBTSignal(int status);

signals:
    void onLinkPhone(const int phone, const int status);
    void onLinkStatusChange(const int type, const int status);
    void onLinkBTStatusChange(const int type, const int status);
//  void onTouchStatusChange(int type, int x_src, int y_src, int x_dst, int y_dst);
//  void onTouchStatusChange(TouchStatus type, int x_src, int y_src, int x_dst, int y_dst);

public slots:
    void requestLinkStatus(const int type, const int status);

    //void requestLinkMode(const int type);

//    void requestTouchStatus(int type, int x_src, int y_src, int x_dst, int y_dst);

    void requestTouchStatus(const int deviceType, const int touchType, const QString &touchPointXml);
//    void requestTouchStatus(const Link_Type deviceType, const TouchType touchType, const QString &touchPointXml);

    void requestKeyValue(int type);

    void requestWheelStatus(const Link_Type deviceType, const int select_pressed, const int wheelStep);
//private slots:
//    void onLinkStatusChangePrivate(const int type, const int status);
};

#endif // DBUSSERVER_H
