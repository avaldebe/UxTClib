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

#include <SSD1306.h>            // alias for `#include "SSD1306Wire.h"`
#include "images.h"             // WiFi logo
SSD1306  display(0x3c, D2, D1); // Initialize the OLED display using Wire library

#include <NTPtimeESP.h>
#include "config.h"
NTPtime NTP(NTP_POOL); // see config.h

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("Booted");

  display.init();
  display.flipScreenVertically();
  oled_wifi();

  Serial.print("Connecting to ");Serial.println(WIFI_SSID);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID,WIFI_PASS); // see config.h
  while (WiFi.status() != WL_CONNECTED) {
    switch (WiFi.status()) {
      case WL_NO_SSID_AVAIL:
        Serial.println("SSID not found");
        break;
      case WL_CONNECT_FAILED:
        Serial.println("Failed to connect");
        break;
      default:
        Serial.print(".");
    }
    delay(500);
  }
  Serial.println("WiFi connected");
}

void loop() {
  // query NTP server, see config.h
  strDateTime ntpNow = NTP.getNTPtime(TIME_ZONE);

  // check ntpNow.valid before using the returned time
  if(ntpNow.valid){
    oled_time(ntpNow);

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

void oled_wifi(){
  display.clear();

  // see http://blog.squix.org/2015/05/esp8266-nodemcu-how-to-create-xbm.html
  // on how to create xbm files
  display.drawXbm(34, 16, WiFi_Logo_width, WiFi_Logo_height, WiFi_Logo_bits);

  // The coordinates define the center of the text
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(64, 50, "Connecting");

  // write the buffer to the display
  display.display();
}

void oled_time(strDateTime ntpNow) {
  static char buffer[24];
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.setFont(ArialMT_Plain_24);
  display.drawString(64, 12, "NTP");

  // date and time
  display.setFont(ArialMT_Plain_10);
  sprintf(buffer, "%04d-%02d-%02d", ntpNow.year, ntpNow.month, ntpNow.day);
  display.drawString(64, 42, buffer);
  sprintf(buffer, "%02d:%02d:%02d", ntpNow.hour, ntpNow.minute, ntpNow.second);
  display.drawString(64, 54, buffer);

  // write the buffer to the display
  display.display();

}
