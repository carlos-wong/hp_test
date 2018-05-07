#include "UserInterface/Launcher.h"
#include "Utility.h"
#include "BusinessLogic/Widget.h"
#include "BusinessLogic/Setting.h"
#include "BusinessLogic/Multimedia.h"
#include "ArkApplication.h"
#include "SettingService.h"
#include "MultimediaService.h"
#include "AudioService.h"
#include "UserInterfaceUtility.h"
#include <QTextCodec>
#include <unistd.h>
#include <stdio.h>
#include <QApplication>
#include <stdlib.h>
static void setTextCodec();
//execute sync when reboot
//::shutdown:/bin/sync
//volume
//kmem 0xe4000008 0x7878
//kernel message
//echo 8 4 1 3> /proc/sys/kernel/printk
//bandwidth
//kmem 0xe4900054 0xb94095
//kmem 0x9000000

int main(int argc, char *argv[])
{
    SettingPersistent::restoreBrightness();
    SettingPersistent::restoreContrast();
    SettingPersistent::restoreSaturation();
    
    UserInterfaceUtility::startTime();
    int result = getopt(argc, argv, "t:");
    if (-1 != result) {
        QString argument(optarg);
        if (!argument.isEmpty()) {
            switch(result) {
            case 't': {
                if (MultimediaApplication == argument) {
                    QCoreApplication app(argc, argv);
                    setTextCodec();
                    MultimediaService multimediaService;
                    (void)multimediaService;
                    return app.exec();
                } else if (AudioApplication == argument) {
                    nice(15);
                    QCoreApplication app(argc, argv);
                    AudioService audioService;
                    (void)audioService;
                    return app.exec();
                } else {
                    return EXIT_FAILURE;
                }
                break;
            }
            default: {
                return EXIT_FAILURE;
                break;
            }
            }
        }
    }
    nice(-20);
    ArkApplication app(argc, argv, QApplication::GuiServer);
    setTextCodec();
    g_Widget;
    Launcher launcher;
    (void)launcher;
    return app.exec();
}

static void setTextCodec()
{
    QTextCodec* textCodec = QTextCodec::codecForName("UTF-8");
    if (NULL != textCodec) {
        QTextCodec::setCodecForCStrings(textCodec);
        QTextCodec::setCodecForTr(textCodec);
        QTextCodec::setCodecForLocale(textCodec);
    } else {
        qDebug() << "get codecForName UTF-8 failed!";
    }
}
