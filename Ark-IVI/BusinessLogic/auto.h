#ifndef __ANDROID_AUTO_H
#define __ANDROID_AUTO_H

#define PHONEBTADDR  "/tmp/phonebtaddr"

typedef enum
{
    LINK_UNSUPPORTED= 0xff,     //\u624b\u673a\u7c7b\u578b\u672a\u5339\u914d\u6216\u624b\u673a\u8fde\u63a5\u4e2d\u8f6c\u540e\u53f0\u70b9\u51fb\u53e6\u5916\u4e00\u4e2a\u8fde\u63a5\u6a21\u5f0f
    LINK_CONNECTED = 1,         //\u6682\u65f6\u672a\u7528
    LINK_DISCONNECTED = 2,      //\u8bf7\u6c42\u8fde\u63a5\u540e\u6ca1\u8fde\u63a5\u4e0a\uff0c\u53ef\u80fd\u6ca1\u63d2\u5165\u624b\u673a
    LINK_STARTING = 3,          //\u53d1\u8d77\u8fde\u63a5
    LINK_SUCCESS = 4,           //\u8fde\u63a5\u6210\u529f
    LINK_FAIL = 5,              //\u8fde\u63a5\u5931\u8d25
    LINK_EXITING = 6,           //\u9000\u5230\u540e\u53f0
    LINK_EXITED = 7 ,           //\u9000\u51facarlife\uff0c\u5b8c\u5168\u662f\u91ca\u653e\u89e3\u7801\u5e93\uff0c\u5207\u6e90\u7684\u65f6\u5019\u8c03\u7528
    LINK_REMOVED = 8,           //\u624b\u673a\u8fde\u63a5\u4e0acarlife\u540e\u62d4\u51fa\u624b\u673a
    LINK_INSERTED = 9,          //\u624b\u673a\u63d2\u5165\u5305\u62ec\u7b2c\u4e00\u6b21\u4e0a\u7535
    LINK_NOT_INSERTED = 10,     //\u624b\u673a\u672a\u8fdb\u884c\u8fde\u63a5\u62d4\u51fa,\u6682\u65f6\u53ef\u4ee5\u6ca1\u7528
    LINK_NOT_INSTALL = 11,      //\u672a\u5b89\u88c5APK
    LINK_CALL_PHONE = 12,       //carlife\u62e8\u6253\u7535\u8bdd\uff0c\u547c\u5165\u7535\u8bdd\u548c\u901a\u8bdd\u4e2d
    LINK_CALL_PHONE_EXITED = 13,//carlife\u62d2\u7edd\u6765\u7535\u6216\u8005\u6302\u65ad\u7535\u8bdd
    LINK_MUTE =14,              //\u7981\u6389\u58f0\u97f3
    LINK_UNMUTE  = 15,          //\u6062\u590d\u58f0\u97f3
    LINK_NODATA = 16,           //\u82f9\u679c\u624b\u673a\u9501\u5c4f\u6216\u8005\u5728\u540e\u9488\u5bf9iphone5\uff0ciphone5s,iphone6\u7b49\u673a\u578b
    LINK_VIDEOREADY = 17,       //\u82f9\u679c\u624b\u673a\u89e3\u9501\u5e76\u4e14\u5728\u524d\u53f0\u9488\u5bf9iphone5\uff0ciphone5s,iphone6\u7b49\u673a\u578b
    LINK_BT_DISCONNECT = 18,    //\u65ad\u6389\u84dd\u7259
    LINK_FAILED_EAP = 19,       //\u82f9\u679c\u624b\u673aeap\u5931\u8d25,\u63d0\u793a\u7528\u6237\u8981\u91cd\u542f\u624b\u673a\u7aefcarlife
    LINK_FAILED_UNSTART = 20,   //\u624b\u673a\u7aef\u8fd8\u672a\u62c9\u8d77\u5b8c\u6210\u5c31\u901a\u4fe1,\u53ef\u5efa\u8bae\u7528\u6237\u624b\u673a\u7aef\u624b\u52a8\u542f\u52a8carlife
	LINK_AUTO_BT_UNPAIRED = 21, //return the status of vehicle bluetooth with target android phone
	LINK_AUTO_BT_PAIRED = 22,
}Link_STATUS;
#endif
