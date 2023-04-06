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

These modules provide PWM outputs that can be controlled.

PWM settings

* pin count
* pin
* frequency
* current value
* minimal possible PWM value ( min: 0 )
* maximal possible PWM value ( max: 255 )

mqtt section

* a checkbox which activates the status sending via mqtt. The interval is activated and set in the mqtt settings.

# web interface

![pwm interface](/images/pwm.png)

# mqtt interface

## mqtt stat

```Json
{
  "id": "defaultIoT_d1fd28",
  "ip": "192.168.2.26",
  "time": "2023-04-05 19:53.40",
  "uptime": 536,
  "version": "2023033001; firmware: Apr  5 2023 19:27:16; GCC-Version: 5.2.0",
  "reset_reason": "ESP_RST_POWERON",
  "reset_time": "1970-01-01 00:00.00",
  "interval": 5,
  "pwm": {
    "count": 1,
    "pin": [
      {
        "pin": 17,
        "value": 0,
        "frequency": 8000,
        "min": 0,
        "max": 255
      }
    ]
  }
}
```

## mqtt cmnd

```Json
change pwm to 128 and check min/max
{"pwm_channel":0,"value":128}

change pwm frequency to 8000Hz
{"pwm_channel":0,"frequency":8000}

set pwm min value to 0
{"pwm_channel":0,"min":0}

set pwm min value to 255
{"pwm_channel":0,"max":255}
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
