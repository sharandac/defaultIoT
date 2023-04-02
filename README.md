<p align="center">
<img src="https://img.shields.io/github/last-commit/sharandac/defaultIoT.svg?style=for-the-badge" />
&nbsp;
<img src="https://img.shields.io/github/license/sharandac/defaultIoT.svg?style=for-the-badge" />
&nbsp;
<a href="https://www.buymeacoffee.com/sharandac" target="_blank"><img src="https://img.shields.io/badge/Buy%20me%20a%20coffee-%E2%82%AC5-orange?style=for-the-badge&logo=buy-me-a-coffee" /></a>
</p>
<hr/>

# defaultIoT

Let's make the Ikea Vindriktning air quality sensor a little smarter. Here is the matching completely over-engineered firmware.

# Install

Clone this repository and open it with platformIO. Remember, the SPIFF must also be flashed. The wifi access credentials can be entered in the config.h file. If no credentials is available, a separate wifi is opened. On a terminal in vscode you can do it with

```bash
pio run -t uploadfs
pio run -t upload
```

After that, take a look at your monitorport ...

```bash
[I][main.cpp:31] setup(): start defaultIoT
[I][core.cpp:88] core_setup(): core module initialized
[I][wificlient.cpp:94] wificlient_Task(): Start Wifi-Client on Core: 1
[I][wificlient.cpp:111] wificlient_Task(): WiFi connection lost, restart ... 
[I][ntp.cpp:73] ntp_Task(): Start NTP Task on Core: 1
[I][ntp.cpp:102] ntp_Task(): NTP-client: renew time
[I][ntp.cpp:114] ntp_Task(): NTP-client: time is 2023-04-01 17:57.25
[I][mqttclient.cpp:255] Task(): Start MQTT-Client on Core: 1
[I][webserver.cpp:58] asyncwebserver_Task(): Start Webserver on Core: 1
[I][wificlient.cpp:127] wificlient_Task(): connected, IP address: 192.168.2.26
[I][mqttclient.cpp:61] onMqttConnect(): MQTT-Client: connected to [192.168.2.23]
```
When the output look like this, congratulation!

If no wifi access data is available an access point will be opened with an unique name like
```bash
defaultIoT_XXXXX
```
and an not so unique password
```bash
defaultIoT
```
After that you can configure the defaultIoT under the following IP-address with your favorite webbrowser
```bash
http://192.168.4.1
```

# module

The following modules are implemented:

* [analog](src/modules/analog/README.md)
* [input](src/modules/input/README.md)
* [foernuftig](src/modules/foernuftig/README.md)
* [servo](src/modules/servo/README.md)
* [pwm](src/modules/pwm/README.md)
* [output](src/modules/output/README.md)
* [oneWire](src/modules/1wire/README.md)
* [vindriktning](src/modules/vindriktning/README.md)

# Contributors

Every Contribution to this repository is highly welcome! Don't fear to create pull requests which enhance or fix the project, you are going to help everybody.
<p>
If you want to donate to the author then you can buy me a coffee.
<br/><br/>
<a href="https://www.buymeacoffee.com/sharandac" target="_blank"><img src="https://img.shields.io/badge/Buy%20me%20a%20coffee-%E2%82%AC5-orange?style=for-the-badge&logo=buy-me-a-coffee" /></a>
</p>
