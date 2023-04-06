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

As the name suggests, it's about the analog inputs. Up to 4 inputs can be managed with various setting options:

* number of analog inputs

for each input the following options are available:

* analog Pin
* analog value, not editable
* low threshold to count inpulses or events
* high threshold
* a checkbox to control if the counter should be reset after each status report via mqtt

mqtt section

* a checkbox which activates the status sending via mqtt. The interval is activated and set in the mqtt settings.

# web interface

![analog interface](/images/analog.png)

# mqtt interface

## mqtt status

```json
{
  "id": "sensor_f62ca8",
  "ip": "192.168.2.85",
  "time": "2023-04-03 11:18.12",
  "uptime": 152776,
  "version": "2023033001; firmware: Apr  1 2023 16:51:46; GCC-Version: 5.2.0",
  "reset_reason": "ESP_RST_SW",
  "reset_time": "2023-04-01 14:51.59",
  "interval": 75,
  "analog": {
    "count": 1,
    "pin": [
      {
        "value": 634,
        "min": 607,
        "max": 658,
        "triggercount": 0
      }
    ]
  }
}
```

## mqtt cmnd

Not available

## mqtt tele

On each connect to a mqtt server a telemetry information is send like this:

```json
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