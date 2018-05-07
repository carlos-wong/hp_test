#ifndef SINGLEAPPLICATION_H
#define SINGLEAPPLICATION_H

#include <QString>
#include <string>

bool memallocload();
bool driverload();
bool acquireApplication(const std::string &key = std::string("ArkMicro"), const bool block = true);
bool releaseApplication(const std::string &key = std::string("ArkMicro"));
bool initializeArkVideoResources();
bool hideArkStartupLogo();
bool capacitiveScreen();

QString compilerDate();
QString osVersion();

bool setDateTime(const int year,
                 const int month,
                 const int day,
                 const int hour,
                 const int minute,
                 const int second);

typedef void (*HANDLER) (void*);
HANDLER acquirePreemptiveResource(HANDLER callback, void *parameter);
void clearOwner();

#endif // SINGLEAPPLICATION_H
