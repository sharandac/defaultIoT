<p align="center">
<img src="https://img.shields.io/github/last-commit/sharandac/defaultIoT.svg?style=for-the-badge" />
&nbsp;
<img src="https://img.shields.io/github/license/sharandac/defaultIoT.svg?style=for-the-badge" />
&nbsp;
<a href="https://www.buymeacoffee.com/sharandac" target="_blank"><img src="https://img.shields.io/badge/Buy%20me%20a%20coffee-%E2%82%AC5-orange?style=for-the-badge&logo=buy-me-a-coffee" /></a>
</p>
<hr/>

# defaultIoT

Again and again, the wish came up that all the little helpers don't have to reinvent the wheel over and over again. Here is the appropriate completely overdone firmware.
It is neatly divided into modules that can be activated or deactivated individually. The configuration is also saved dutifully.
So it is possible for all my little helpers always one and the same firmware to use and everything in one pass to update if necessary.

# Install

Clone this repository and open it with platformIO. Remember, the SPIFF must also be flashed. The wifi access credentials can be entered in the config.h file. If no credentials is available, a separate wifi is opened. On a terminal in vscode you can do it with

```bash
pio run -t uploadfs
pio run -t upload
```

After that, take a look at your monitorport ...

```
[I][core.cpp:79] core_setup(): core start defaultIoT
[I][core.cpp:126] core_setup(): corecustom arduino-esp32 framework detected, enable PM/DFS support, 240/80MHz with light sleep enabled
[I][wificlient.cpp:109] Task(): Start Wifi-Client on Core: 1
[I][wificlient.cpp:126] Task(): WiFi connection lost, restart ... 
[I][ntp.cpp:80] Task(): Start NTP Task on Core: 1
[I][webserver.cpp:73] asyncwebserver_Task(): Start Webserver on Core: 1
[I][mqttclient.cpp:287] Task(): Start MQTT-Client on Core: 1
[I][core.cpp:153] core_setup(): core module initialized
[I][ntp.cpp:105] Task(): NTP-client: renew time
[I][ntp.cpp:116] Task(): NTP-client: time is 2023-04-05 20:05.41
[I][wificlient.cpp:142] Task(): connected, IP address: 192.168.2.26
[I][mqttclient.cpp:93] onMqttConnect(): MQTT-Client: connected to [192.168.2.23]
```
When the output look like this, congratulation!

If no wifi credentials is available an access point will be opened with an unique name like
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

And for to ptogrammers, here is a small and easy to use template:

* [module template](src/modules/README.md)

# Contributors

Every Contribution to this repository is highly welcome! Don't fear to create pull requests which enhance or fix the project, you are going to help everybody.
<p>
If you want to donate to the author then you can buy me a coffee.
<br/><br/>
<a href="https://www.buymeacoffee.com/sharandac" target="_blank"><img src="https://img.shields.io/badge/Buy%20me%20a%20coffee-%E2%82%AC5-orange?style=for-the-badge&logo=buy-me-a-coffee" /></a>
</p>
