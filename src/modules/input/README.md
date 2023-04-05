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

![input interface](/images/input.png)

# mqtt interface

## mqtt stat

```Json
{
  "id": "defaultIoT_d1fd28",
  "ip": "192.168.2.26",
  "time": "2023-04-05 19:38.27",
  "uptime": 666,
  "version": "2023033001; firmware: Apr  5 2023 19:27:16; GCC-Version: 5.2.0",
  "reset_reason": "ESP_RST_POWERON",
  "reset_time": "2023-04-05 19:27.21",
  "interval": 5,
  "input": {
    "count": 1,
    "bitmask": "?b",
    "pin": [
      {
        "pin": 0,
        "state": true
      }
    ]
  }
}
```

## mqtt cmnd

```Json
Not available
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
