#include "Utility.h"
#include <sys/fcntl.h>
#include <string.h>
#include <iostream>
#include <string>
#include <sstream>
#include <unistd.h>
#include <cstdlib>
#include <fcntl.h>
#include <stdio.h>
#include <time.h>
#include <QDateTime>
#include <sys/utsname.h>
#include <time.h>
#include <QStringList>

using std::string;
using std::stringstream;
using std::cout;
using std::endl;

static string appFilePathSuffix()
{
    static const unsigned short s_MaxCount = 1024;
    char buf[s_MaxCount] = {'\0'};
    int ret = readlink("/proc/self/exe", buf, s_MaxCount);
    string suffix("");
    if ((ret < 0) || (ret > s_MaxCount)) {
        suffix = string("");
    } else {
        string tmp(buf);
        int pos = tmp.find_last_of(char('/'));
        if (-1 == pos) {
            suffix = string("");
        }
        suffix = tmp.substr(pos + 1, tmp.length() - 1);
    }
    return suffix;
}

bool singleApplication(const bool acquire, const std::string &key, const bool block)
{
    string app;
    if (0 == key.size()) {
        app = appFilePathSuffix();
    } else {
        app = key;
    }
    bool ret = true;
    string error;
    if (acquire) {
        error = "acquire";
    } else {
        error = "release";
    }
    if (app.empty()) {
        error = string("app is Empty!");
        ret = false;
    } else {
        string tmp = app;
        //        stringstream pidStr;
        //        pidStr << getpid();
        //        string pid;
        //        pidStr >> pid;
        tmp = string("/tmp/") + tmp;
        int fd = open(tmp.c_str(), O_RDWR | O_CREAT, 0666);
        if (fd < 0) {
            error =  string("open ") + tmp + string(" error!");
            ret = false;
        } else {
            struct flock lock;
            if (!acquire) {
                lock.l_type = F_RDLCK;
            }
            lock.l_whence = SEEK_SET;
            lock.l_start = 0;
            lock.l_len = 0;
            if ((!acquire)
                    && (0 == fcntl(fd, F_GETLK, &lock))) {
                if (F_WRLCK == lock.l_type) {
                    ret = false;
                }
            } else {
                if (acquire) {
                    lock.l_type = F_WRLCK;
                } else {
                    lock.l_type = F_UNLCK;
                }
                lock.l_pid = getpid();
                int cmd = F_SETLKW;
                if (!block) {
                    cmd = F_SETLK;
                }
                if(fcntl(fd, cmd, &lock) < 0) {
                    close(fd);
                    fd = -1;
                    error = string("application already acquire!");
                    ret = false;
                }
            }
        }
    }
    if (ret) {
        error += " success!";
    } else {
        error += " failed!";
    }
    cout << " singleApplication: " << app << " " << error << endl;
    return ret;
}

bool acquireApplication(const std::string &key, const bool block)
{
    return singleApplication(true, key, block);
}

bool releaseApplication(const std::string &key)
{
    return singleApplication(false, key, false);
}


static HANDLER m_Callback(NULL);
static void* m_Parameter(NULL);

HANDLER acquirePreemptiveResource(HANDLER callback, void *parameter)
{
    HANDLER lastHandler = m_Callback;
    if ((NULL != m_Callback) && (callback != m_Callback)) {
        m_Callback(m_Parameter);
    }
    m_Callback = callback;
    m_Parameter = parameter;
    return lastHandler;
}

void clearOwner()
{
    m_Callback = NULL;
    m_Parameter = NULL;
}

bool initializeArkVideoResources()
{
    static bool flag(false);
    if (!flag) {
        int ret = 0;
#ifndef gcc
        ret = system("/etc/memalloc_load.sh");
        ret = (0 == ret) && system("/etc/driver_load.sh");
        ret = (0 == ret) && system("/etc/itu656_load.sh");
#endif
        flag = (0 == ret);
    }
    return flag;
}
#include <fcntl.h>
bool hideArkStartupLogo()
{
    static bool flag(false);
    if (!flag) {
        int ret = 0;
#ifndef gcc
        int fd = open("/proc/display", O_WRONLY);
        if (-1 != fd) {
            const char str[] = "23";
            ret = write(fd, str, sizeof(str));
            close(fd);
            fd = -1;
        }
        //        ret = system("echo 23 > /proc/display");
#endif
        flag = (3 == ret);
    }
    return flag;
}

QString compilerDate()
{
    char month[4] = {0};
    int MM, dd, yyyy;
    struct tm t = {0};
    static const char monthNames[] = "JanFebMarAprMayJunJulAugSepOctNovDec";
    sscanf(__DATE__, "%s %d %d", month, &dd, &yyyy);
    MM = (strstr(monthNames, month) - monthNames) / 3;
    t.tm_year = yyyy - 1900;
    t.tm_mday = dd;
    t.tm_mon = MM;
    return QDateTime::fromTime_t((uint)mktime(&t)).toString("yyyyMMdd");
}

QString osVersion()
{
    QString version/*("#210 Mon Jun 12 11:40:05 CST 2017")*/;
    struct utsname  kernelInfo;
    if (uname(&kernelInfo) != -1) {
        version = QString(kernelInfo.version);
    }
    QString year, month, day("%1");
    QStringList stringList = version.split(QChar(' '));
    for (int i = 0; i < stringList.size(); ++i) {
#ifndef gcc
        if (2 == i) {
#else
        if (3 == i) {
#endif
            month = stringList.at(i);
            if (month.contains("Jan")) {
                month = QString("01");
            } else if (month.contains("Feb")) {
                month = QString("02");
            } else if (month.contains("Mar")) {
                month = QString("03");
            } else if (month.contains("Apr")) {
                month = QString("04");
            } else if (month.contains("May")) {
                month = QString("04");
            } else if (month.contains("Jun")) {
                month = QString("06");
            } else if (month.contains("Jul")) {
                month = QString("07");
            } else if (month.contains("Aug")) {
                month = QString("08");
            } else if (month.contains("Sep")) {
                month = QString("09");
            } else if (month.contains("Oct")) {
                month = QString("10");
            } else if (month.contains("Nov")) {
                month = QString("11");
            } else /*if (month.contains("Dec"))*/ {
                month = QString("12");
            }
#ifndef gcc
        } else if (3 == i) {
#else
        } else if (4 == i) {
#endif
//            day = stringList.at(i);
            day = day.arg(stringList.at(i).toInt(), 2, 10, QChar('0'));
        } else if (i == stringList.size() - 1) {
            year = stringList.at(i);
        }
    }
    return year + month + day;
}

bool setDateTime(const int year, const int month, const int day, const int hour, const int minute, const int second)
{
    struct tm tm;
    tm.tm_year = year - 1900;
    tm.tm_mon = month;
    tm.tm_mday = day;
    tm.tm_hour = hour;
    tm.tm_min = minute;
    tm.tm_sec = second;
    tm.tm_isdst = -1;
    time_t time = mktime(&tm);
    bool flag(false);
    if (0 == stime(&time)) {
        if (0 == system("hwclock -w")) {
            flag = true;
        }
    }
    return flag;
}

bool memallocload()
{
    static bool load = false;
    if (!load) {
        if (0 != system("/etc/memalloc_load.sh")) {
            printf("execute hw decoder memory /etc/memalloc_load.sh failed!");
        } else {
            load = true;
        }
    }
    return load;
}

bool driverload()
{
    static bool load = false;
    if (!load) {
        if (0 != system("/etc/driver_load.sh")) {
            printf("execute hw decoder /etc/driver_load.sh failed!");
        } else {
            load = true;
        }
    }
    return load;
}

bool capacitiveScreen()
{
    bool flag = true;
    if (NULL == getenv("QWS_ARK_MT_DEVICE")) {
        flag = false;
    }
    return flag;
}
