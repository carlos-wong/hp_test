#ifndef GLOBALCONFIGURE_H
#define GLOBALCONFIGURE_H

enum RegionalVersion{
    Ver_Domestic,      /*< 国内版*/
    Ver_Overseas       /*< 海外版*/
};

// 设置区域版本
/*#######################################*/

RegionalVersion g_Regionnal_Version = Ver_Overseas;

/*#######################################*/

#endif // GLOBALCONFIGURE_H
