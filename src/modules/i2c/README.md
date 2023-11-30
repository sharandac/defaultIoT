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

Reading OneWire temperature sensors. A maximum of 16 sensors can be read out. These are provided if correctly set via Mqtt with the appropriate interval.

OneWire settings

* OneWire data bus pin
* report in °C or °F
* internal sensor read interval

sensor data

* out each found sensor

mqtt section

* a checkbox which activates the status sending via mqtt. The interval is activated and set in the mqtt settings.

# web interface

![OneWire interface](/images/onewire.png)

# mqtt interface

## mqtt stat

```Json
{
  "id": "sensor",
  "ip": "192.168.2.34",
  "time": "2023-04-03 11:30.22",
  "uptime": 154981,
  "version": "2023033001; firmware: Apr  1 2023 15:49:17; GCC-Version: 5.2.0",
  "reset_reason": "ESP_RST_SW",
  "reset_time": "2023-04-01 16:27.23",
  "interval": 15,
  "sensor": [
    {
      "addr": "21:12:a9:6e:e3:28",
      "temp": 33.1875,
      "temp_str": "33.19°C"
    },
    {
      "addr": "21:12:b1:02:13:28",
      "temp": 18.5625,
      "temp_str": "18.56°C"
    }
  ]
}
```

## mqtt cmnd

```json
Not available
```

## mqtt tele

On each connect to a mqtt server a telemetry information is send like this:

```json
{
  "id": "sensor",
  "ip": "192.168.2.34",
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