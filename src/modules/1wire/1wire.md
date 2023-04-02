<p align="center">
<img src="https://img.shields.io/github/last-commit/sharandac/defaultIoT.svg?style=for-the-badge" />
&nbsp;
<img src="https://img.shields.io/github/license/sharandac/defaultIoT.svg?style=for-the-badge" />
&nbsp;
<a href="https://www.buymeacoffee.com/sharandac" target="_blank"><img src="https://img.shields.io/badge/Buy%20me%20a%20coffee-%E2%82%AC5-orange?style=for-the-badge&logo=buy-me-a-coffee" /></a>
</p>
<hr/>


# defaultIoT

Let's make the Ikea Foernuftig air filter a little smarter. Here is the matching completely over-engineered firmware.

# Install

## first step: program the esp32

Clone this repository and open it with platformIO. Remember, the SPIFF must also be flashed. On a terminal in vscode you can do it with
```bash
pio run -t uploadfs
pio run -t upload
```

After that, take a look at your monitorport ...

```text
[I][main.cpp:43] setup(): start defaultIoT
[I][modul_mgmt.cpp:162] module_mgmt_register(): register known module analog
[I][modul_mgmt.cpp:162] module_mgmt_register(): register known module input
[I][modul_mgmt.cpp:162] module_mgmt_register(): register known module output
[I][modul_mgmt.cpp:162] module_mgmt_register(): register known module servo
[I][modul_mgmt.cpp:162] module_mgmt_register(): register known module pwm
[I][modul_mgmt.cpp:162] module_mgmt_register(): register known module onewire
[I][modul_mgmt.cpp:162] module_mgmt_register(): register known module vindriktning
[I][modul_mgmt.cpp:162] module_mgmt_register(): register known module foernuftig
[I][core.cpp:81] core_setup(): custom arduino-esp32 framework detected, enable PM/DFS support, 240/80MHz with light sleep enabled
[I][core.cpp:88] c��\Y5����␃F��␀ؽɕ�module initialized
[I][wificlient.cpp:94] wificlient_Task(): Start Wifi-Client on Core: 1
[I][wificlient.cpp:111] wificlient_Task(): WiFi connection lost, restart ... 
[I][ntp.cpp:72] ntp_Task(): Start NTP Task on Core: 1
[E][callback.cpp:281] callback_send(): no callback structure found
[I][ntp.cpp:101] ntp_Task(): NTP-client: renew time
[E][callback.cpp:281] callback_send(): no callback structure found
[I][ntp.cpp:113] ntp_Task(): NTP-client: time is 2023-04-01 09:50.49
[I][mqttclient.cpp:255] Task(): Start MQTT-Client on Core: 1
[I][webserver.cpp:58] asyncwebserver_Task(): Start Webserver on Core: 1
[I][wificlient.cpp:127] wificlient_Task(): connected, IP address: 192.168.2.26
[I][mqttclient.cpp:61] onMqttConnect(): MQTT-Client: connected to [192.168.2.23]
```
When the output look like this, congratulation!

After the first start an access point will be opened with an unique name like
```bash
defaultIoT_XXXXX
```
and an not so unique password
```bash
defaultIoT
```
After that you can configure the foernuftig under the following IP-address with your favorite webbrowser
```bash
http://192.168.4.1
```



# Contributors

Every Contribution to this repository is highly welcome! Don't fear to create pull requests which enhance or fix the project, you are going to help everybody.
<p>
If you want to donate to the author then you can buy me a coffee.
<br/><br/>
<a href="https://www.buymeacoffee.com/sharandac" target="_blank"><img src="https://img.shields.io/badge/Buy%20me%20a%20coffee-%E2%82%AC5-orange?style=for-the-badge&logo=buy-me-a-coffee" /></a>
</p>


