1. 更新的代码实现了快速倒车，AUXIN和倒车显示，AUXIN和多媒体播放时CVBS输出，多媒体播放时切换倒车等功能，
   并提供了相应的API接口和Demo程序。
2. 在uboot过程和kernel启动过程通过拉高拉低GPIO5脚可以实现快速倒车，需要注意的是从uboot过程到kernel启动
   过程中有1~2秒时间是不能正常响应中断的，因而此时拉高拉低GPIO5脚并没有反应。应用起来后通过执行
   "echo 23 > /proc/display"来显示UI framebuffer，并通知系统应用已正常启动，此后，系统底层不再处理倒车
   显示，需要应用层接收MCU倒车消息后调用提供的API接口来显示倒车图像。
3. 支持AUXIN, UI, 和多媒体播放(mplayer)时的TVOUT功能，但是同一时间只能支持一种。暂时只实现了CVBS输出。
   具体使用在下面各个Demo程序说明中指出。
4. auxin-demo程序用来演示AUXIN接口使用，命令行参数为"./auxin-demo source width height tvout", 其中source
   表示CVBS信号源，现有DVR_SOURCE_CAMERA和DVR_SOURCE_AUX，因为是按照我们公板来调试的，DVR_SOURCE_CAMERA
   使用ARK7116通道0，DVR_SOURCE_AUX使用ARK7116通道1，他们都使用ITU656通道1。width和height表示需要显示的
   宽高,tvout表示是否需要同时TVOUT输出。运行起来后也可以通过提供的接口来动态关闭或者打开TVOUT显示。
5. gui-tvout程序提供UI层TVOUT输出接口说明，接口很简单，看程序代码就能明白。
6. mplayer支持CVBS输出，只需在命令行启动添加参数-tvout就行，另外也可以动态开关mplayer的tvout，mplayer在
   slave模式下发送tvout 1(0)命令就可以开关tvout输出。
7. launcher程序模拟应用层接收到倒车消息后通知mplayer放弃显示并调用接口来显示倒车图像以及接收到退出倒车
   后相应的处理。需要注意的是如果mplayer播放时打开了TVOUT功能，此时进入倒车TVOUT输出显示会停止在某个画面
   直到退出倒车后才能继续正常显示。这个是由于芯片资源限制造成的，后续会和芯片研发讨论，可能会完善该问题。
8. 以上所有相关的API接口都通过libarkcmn库提供, 库文件和头文件会在包内arm-lib文件夹给出。