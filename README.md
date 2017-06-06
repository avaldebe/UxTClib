# ESPntc2rtc

Update/setup an RTC module from a [NTP][] service using with ESP8266 module.

- Hardware
  - [WEMOS D1 mini][d1_mini]
  - 64x48 [OLED shield][oled_shield]
  - Optional
    - [proto][proto] + [base][base2] shields, or [large base][base3] shield
    - [battery shield][battery]
  - TBD
    - I2C bus connectors & pinout
    - enclosure
- Software
  - arduino framework
  - [NTP library][NTPlib], see [video][NTPvid]
  - [RTC library][RTClib]

[d1_mini]: https://wiki.wemos.cc/products:d1:d1_mini
[oled_shield]: https://wiki.wemos.cc/products:d1_mini_shields:oled_shield
[battery]: https://wiki.wemos.cc/products:d1_mini_shields:battery_shield
[proto]: https://wiki.wemos.cc/products:d1_mini_shields:protoboard_shield
[base2]: https://wiki.wemos.cc/products:d1_mini_shields:dual_base
[base3]: https://wiki.wemos.cc/products:d1_mini_shields:tripler_base

[NTPlib]: https://github.com/SensorsIot/NTPtimeESP
[NTPvid]: https://www.youtube.com/watch?v=qzkNXhubWLg
[RTClib]: https://github.com/adafruit/RTClib
[NTP]: https://en.wikipedia.org/wiki/Network_Time_Protocol
[epoch]: https://en.wikipedia.org/wiki/Unix_time
