1, 在一张TF卡上的根目录下新建一个文件，/test.c（内容可以为空）
2，tf插入IBox并通过adb 输入 reboot recovery 进入工厂模式
3，clone 工程 git@10.173.235.228:projects/Demo/factory.git
4，进入上述工程，用系统cmd运行脚本 factory.bat
5，输入将要写进IBox的设备ID，回车直接写入
6，拔出tf卡，重启（会再进入recovery模式），再通过adb再一次重启
7，完成IBox配置及设备ID的烧录