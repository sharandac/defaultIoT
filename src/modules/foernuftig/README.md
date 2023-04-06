<p align="center">
<img src="https://img.shields.io/github/last-commit/sharandac/defaultIoT.svg?style=for-the-badge" />
&nbsp;
<img src="https://img.shields.io/github/license/sharandac/defaultIoT.svg?style=for-the-badge" />
&nbsp;
<a href="https://www.buymeacoffee.com/sharandac" target="_blank"><img src="https://img.shields.io/badge/Buy%20me%20a%20coffee-%E2%82%AC5-orange?style=for-the-badge&logo=buy-me-a-coffee" /></a>
</p>
<hr/>

# Table of Contents

* 1\. [description](#description)
* 2\. [web interface](#web-interface)
* 3\. [mqtt interface](#mqtt-interface)
    * 3.1\. [mqtt stat](#mqtt-stat)
    * 3.2\. [mqtt cmnd](#mqtt-cmnd)
    * 3.3\. [mqtt tele](#mqtt-tele)
    
# description

see [Foernuftig base project](https://github.com/sharandac/foernuftig)

# web interface

![foernuftig interface](/images/foernuftig.png)

# mqtt interface

## mqtt stat

```json
{
  "id": "foernuftig_397904",
  "ip": "192.168.2.107",
  "time": "2023-04-06 11:54.24",
  "uptime": 126,
  "version": "2023033001; firmware: Apr  6 2023 11:52:41; GCC-Version: 5.2.0",
  "reset_reason": "ESP_RST_SW",
  "reset_time": "2023-04-06 11:52.18",
  "interval": 5,
  "rpm": 129,
  "usb_power": true
}
```

## mqtt cmnd

```json

```

## mqtt tele

```json
{
  "id": "foernuftig_397904",
  "ip": "192.168.2.107",
  "time": "2023-04-06 11:52.19",
  "uptime": 1,
  "reset_reason": "ESP_RST_SW",
  "reset_time": "2023-04-06 11:52.18",
  "mac": "08:B6:1F:39:79:04",
  "channel": 10,
  "ssid": "karlshorst_BGN",
  "rssi": -53
}
```