#! /bin/sh
QMAKE=/usr/local/Trolltech/Qt-4.7.4/bin/qmake
build()
{
	cd $1 && $QMAKE && make
	if [ $? -eq 0 ]; then
		cd ../
	fi
}
build ArkProtocolWrapper
if [ $? != 0 ]; then
	echo build $1 error!
	exit 1
fi
build Utility
if [ $? != 0 ]; then
	echo build $1 error!
	exit 1
fi
build AutoConnect
if [ $? != 0 ]; then
	echo build $1 error!
	exit 1
fi
build ArkApplication
if [ $? != 0 ]; then
	echo build $1 error!
	exit 1
fi
build RunnableThread
if [ $? != 0 ]; then
	echo build $1 error!
	exit 1
fi
build UserInterface
if [ $? != 0 ]; then
	echo build $1 error!
	exit 1
fi
build DbusService
if [ $? != 0 ]; then
	echo build $1 error!
	exit 1
fi
build AudioService
if [ $? != 0 ]; then
	echo build $1 error!
	exit 1
fi
build MultimediaService
if [ $? != 0 ]; then
	echo build $1 error!
	exit 1
fi
build SettingService
if [ $? != 0 ]; then
	echo build $1 error!
	exit 1
fi
build RadioService
if [ $? != 0 ]; then
	echo build $1 error!
	exit 1
fi
build BluetoothService
if [ $? != 0 ]; then
	echo build $1 error!
	exit 1
fi
echo ------------------------------------------------------------------------------------------------------------------------------------------
echo build sucess!
echo ------------------------------------------------------------------------------------------------------------------------------------------
