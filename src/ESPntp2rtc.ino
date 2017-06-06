/*
ESPntp2rtc.ino
  https://github.com/avaldebe/ESPntp2rtc

Based on example code from
  https://github.com/SensorsIot/NTPtimeESP
*/

#include <Wire.h>
#include <RTClib.h>

RTC_DS1307  ds1307;
RTC_DS3231  ds3231;
RTC_PCF8523 pcf8523;
////RTC_PCF8563 pcf8563;

#include <NTPtimeESP.h>
#include "config.h"
NTPtime NTP(NTP_POOL); // see config.h

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("Booted");
  Serial.println("Connecting to Wi-Fi");

  WiFi.mode(WIFI_STA);
  WiFi.begin (WIFI_SSID, WIFI_PASS); // see config.h
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("WiFi connected");
}

void loop() {
  // query NTP server, see config.h
  strDateTime ntpNow = NTP.getNTPtime(TIME_ZONE);

  // check ntpNow.valid before using the returned time
  if(ntpNow.valid){
    // convert NTP date/time strcuture to RTC date/time class
    DateTime rtcNow (           \
      (uint16_t) ntpNow.year,   \
      (uint8_t)  ntpNow.month,  \
      (uint8_t)  ntpNow.day,    \
      (uint8_t)  ntpNow.hour,   \
      (uint8_t)  ntpNow.minute, \
      (uint8_t)  ntpNow.second);
      // set/update RTSs; each on a separate I2C bus, see config.h

    Wire.begin(DS1307_BUS);  ds1307.adjust(rtcNow);
    Wire.begin(DS3231_BUS);  ds3231.adjust(rtcNow);
    Wire.begin(PCF8523_BUS); pcf8523.adjust(rtcNow);

    NTP.printDateTime(ntpNow);
    Serial.println("RTC updated");
  }
  delay(10000); // 10 secs
}
