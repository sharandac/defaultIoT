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
    * 3.1\. [mqtt stat()](#mqtt-stat)
    * 3.2\. [mqtt cmnd](#mqtt-cmnd)
    * 3.3\. [mqtt tele](#mqtt-tele)
    
# description

see [Vindriktning base project](https://github.com/sharandac/vindriktning)

# web interface

![vindriktning interface](/images/vindriktning.png)

# mqtt interface

## mqtt stat

```Json
{
  "id": "vindriktning_d22534",
  "ip": "192.168.2.108",
  "time": "2023-04-06 11:50.13",
  "uptime": 21406,
  "version": "2023033001; firmware: Apr  1 2023 16:51:46; GCC-Version: 5.2.0",
  "reset_reason": "ESP_RST_BROWNOUT",
  "reset_time": "1970-01-01 01:00.01",
  "interval": 15,
  "sensor": {
    "pm1006": {
      "pm1": 795,
      "pm25": 11,
      "pm10": 273
    }
  }
}
```

## mqtt cmnd

```
```

## mqtt tele

```Json
{
  "id": "vindriktning_d22534",
  "ip": "192.168.2.108",
  "time": "2023-04-06 11:53.41",
  "uptime": 1,
  "reset_reason": "ESP_RST_BROWNOUT",
  "reset_time": "2023-04-06 11:53.40",
  "mac": "C0:49:EF:D2:25:34",
  "channel": 10,
  "ssid": "karlshorst_BGN",
  "rssi": -70
}
```

