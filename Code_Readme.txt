
本程序的VideoCaptureLib包含绿幕抠像功能，它集成了cuda调用，被HXCameraViewer依赖。要想正常运行VideoCaptureLib，必须满足以下几个条件：
1.显卡要支持cuda GPU编程
2.本机要安装cuda9.2驱动
3.依赖的openCV库版本是3.4， 需要手动编译该库，使之支持cuda编程, 我编译好的版本放在百度网盘上
4. ffmpeg库版本是4.0

5. test