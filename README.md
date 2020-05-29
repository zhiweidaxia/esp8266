以MQTT为主，使用ESP8266连接ONENET云。
本文并非以AT指令完成接入，而是使用Arduino直接将程序烧录进ESP8266。

ArduinoIDE加载ESP8266方法：

1.打开Arduino，点击左侧方“文件”内的“首选项”中的“Additional Board Manager URLs”编辑框中输入：
http://arduino.esp8266.com/stable/package_esp8266com_index.json

2.进入Arduino的“工具”，点击开发板内的“开发板管理器”，并在弹出的界面中选择ESP8266，点击安装即可。

3.进入Arduino的“工具”，点击开发板内的“NodeMCU1.0（根据自己ESP的型号决定）”选项即可下载。

烧录完毕之后，只需要在串口中输入 ：

h"数据"t"数据"f"数据"

格式数据即可。

作者：张大作死

PS:
例程有需要，可以随便修改，出现事故概不负责
