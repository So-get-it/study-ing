## 4G 拨号上网

### 简介

上网的方式有很多种，但是一个网络的状态不可能一直很好，所以选择一个好的网络来上网就显得尤为重要。

该项目实现的就是有线、无线和4G拨号上网间的自动切换功能。

该项目使用Raspberry Pi搭载了4G模块EC20进行开发。通过各网卡的网络状态来判断是否切换网络，并通过更改路由的方式切换网卡进行上网；如果有线、无线的网络状态都不好，就会通过pppd程序拨号上网并添加路由。

### 功能实现

1、实现串口通信编程。

2、封装常用的AT指令函数来判断串口和SIM卡是否可用。

3、封装特殊的AT指令获取运营商的mcc、mnc号，并通过xml库解析出运营商的接入点名称。

4、封装ping命令的函数，通过解析输出结果判断网络是否可用。

5、通过封装的函数判断网络状态，在有线、无线、pppd拨号上网之间切换。添加路由，设置为较低的metric值；删除添加的路由。

6、用多线程来分别实现pppd拨号功能、高优先级网络差向低优先级切换、高优先级网络状态恢复向高优先级切换。
