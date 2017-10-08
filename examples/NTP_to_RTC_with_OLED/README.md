# ntc2rtc

Some popular RTC can miss up to several seconds on a month,
so it must be updated once in a while. Even most accurate
RTC has to be set to the correct time at least one.

On this example, we update/setup an RTC module from a [NTP][] service with a
ESP8266/ESP32 development board and an OLED module.

## Hardware

The [WEMOS D1 mini][d1_mini] is a popular ESP8266 development board,
with its own [ecosystem of shields][d1_shields].
The [MH ET LIVE ESP32MiniKit][mhetesp32minikit] is an ESP32 development board,
compatible with the d1 mini shields. 
Pick one of this boards, an [64x48 oled shield][oled_shield], some proto board ahd headers,
and get soldering.

## Firmware

We use the arduino framework for the ESP8266 or the ESP32 depending of the board of your choice.
For easy of use, this example is set up as a platformio project.

### Libraries in PlatformIO CLI
For the OLED, we use the [esp8266-oled-ssd1306][OLEDlib] library.
NTP syncronized time is provided by the core/HAL time libraries included on the ESP8266/ESP32 frameworks.
Different RTC ICs are read and updated using the RTC class from the [UxTClib][] library.  

```bash
# install the libraries (one time setup)
pio lib install ESP8266_SSD1306 
```

### Compile in PlatformIO CLI
```bash
# set wifi credentials (on each new terminal session)
export WIFI_SSID=myWIFIssid WIFI_PASS=myWIFIpass

# compile & upload for d1_mini (esp8266)
pio run -e d1_mini
pio run -e d1_mini -t upload

# compile & upload for mhetesp32minikit (esp32)
pio run -e mhetesp32minikit
pio run -e mhetesp32minikit -t upload
```

[d1_mini]:     https://wiki.wemos.cc/products:d1:d1_mini
[d1_shields]:  https://wiki.wemos.cc/products:d1_mini_shields
[oled_shield]: https://wiki.wemos.cc/products:d1_mini_shields:oled_shield

[mhetesp32minikit]: http://forum.mhetlive.com/topic/8/new-mh-et-live-minikit-for-esp32

[OLEDlib]: https://github.com/squix78/esp8266-oled-ssd1306
[UxTClib]: https://github.com/avaldebe/UxTClib

[NTP]: https://en.wikipedia.org/wiki/Network_Time_Protocol
[epoch]: https://en.wikipedia.org/wiki/Unix_time
