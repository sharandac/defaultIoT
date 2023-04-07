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

Easy and simple interface to a 8x32 RGB LED Matrix with touch ( +/- and on/off ).

* [Amazon 8x32 RGB LED Matrix with WS2812B](https://amzn.eu/d/8GkIm0a)

# web interface

![irsta interface](/images/irsta.png)

# mqtt interface

## mqtt stat

```Json
{
  "id": "defaultIoT_d1fd28",
  "ip": "192.168.2.26",
  "time": "2023-04-07 18:28.27",
  "uptime": 71,
  "version": "2023033001; firmware: Apr  7 2023 18:25:46; GCC-Version: 5.2.0",
  "reset_reason": "ESP_RST_POWERON",
  "reset_time": "1970-01-01 00:00.00",
  "interval": 5,
  "irsta": {
    "state": 0,
    "max": 112,
    "max_led": 192,
    "red": 208,
    "green": 112,
    "blue": 32,
    "button": 0
  }
}
```

## mqtt cmnd

```Json
{"set_level":192}
{"set_led":true}
```

## mqtt tele

```Json
{
  "id": "sensor_f62ca8",
  "ip": "192.168.2.85",
  "time": "2023-04-03 11:23.55",
  "uptime": 1,
  "reset_reason": "ESP_RST_SW",
  "reset_time": "2023-04-03 11:23.54",
  "mac": "24:62:AB:F6:2C:A8",
  "channel": 10,
  "ssid": "karlshorst_BGN",
  "rssi": -60
}
```

