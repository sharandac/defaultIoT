<p align="center">
<img src="https://img.shields.io/github/last-commit/sharandac/defaultIoT.svg?style=for-the-badge" />
&nbsp;
<img src="https://img.shields.io/github/license/sharandac/defaultIoT.svg?style=for-the-badge" />
&nbsp;
<a href="https://www.buymeacoffee.com/sharandac" target="_blank"><img src="https://img.shields.io/badge/Buy%20me%20a%20coffee-%E2%82%AC5-orange?style=for-the-badge&logo=buy-me-a-coffee" /></a>
</p>
<hr/>

# function

# web interface

![servo interface](/images/servo.png)

# mqtt interface

## mqtt stat

```Json
{
  "id": "defaultIoT_d1fd28",
  "ip": "192.168.2.26",
  "time": "2023-04-05 19:48.15",
  "uptime": 211,
  "version": "2023033001; firmware: Apr  5 2023 19:27:16; GCC-Version: 5.2.0",
  "reset_reason": "ESP_RST_POWERON",
  "reset_time": "1970-01-01 00:00.00",
  "interval": 5,
  "servo": {
    "count": 1,
    "pin": [
      {
        "pin": 17,
        "current_angle": 0,
        "current_value": 90,
        "state": false
      }
    ]
  }
}
```

## mqtt cmnd

```Json
set servo to angle 90°
{"servo_channel":0,"angle":90}

set servo to scaled value 45
{"servo_channel":0,"value":45}

trigger servo to min angle and if detach is enabled move after detach time to neutral
{"servo_channel":0,"min":true}

trigger servo to max angle and if detach is enabled move after detach time to neutral
{"servo_channel":0,"max":true}
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
