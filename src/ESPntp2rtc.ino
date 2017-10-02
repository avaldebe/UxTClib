/*
ESPntp2rtc.ino
  https://github.com/avaldebe/ESPntp2rtc

Based on example code from
  https://github.com/SensorsIot/NTPtimeESP
  https://github.com/squix78/esp8266-oled-ssd1306
*/

#include <Wire.h>
#ifndef SCL
#define SCL D1
#endif
#ifndef SDA
#define SDA D2
#endif

#include <RTClib.h>
RTC_DS1307  ds1307;
RTC_PCF8523 pcf8523;

#include <SSD1306.h>            // alias for `#include "SSD1306Wire.h"`
#include "images.h"             // WiFi logo
SSD1306  display(0x3c, SDA, SCL); // Initialize the OLED display using Wire library

#ifdef ARDUINO_ARCH_ESP32
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
#endif
#include <EasyNTPClient.h>
#include <WiFiUdp.h>
#include "config.h"
WiFiUDP udp;
EasyNTPClient ntp(udp, NTP_POOL, TIME_ZONE); // see config.h

void setup() {
  Serial.begin(115200);
  Serial.printf("/nBooted");

  Wire.begin(SDA, SCL); // not needed, SSD1306Wire->connect() calls Wire.begin(SDA, SCL)
  display.init();
  display.flipScreenVertically();
  oled_wifi();

  Serial.printf("Connecting to %s\n", WIFI_SSID);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS); // see config.h
  while (WiFi.status() != WL_CONNECTED) {
    switch (WiFi.status()) {
      case WL_NO_SSID_AVAIL:
        Serial.printf("SSID not found\n");
        break;
      case WL_CONNECT_FAILED:
        Serial.printf("Failed to connect\n");
        break;
      default:
        Serial.printf(".");
    }
    delay(500);
  }
  Serial.printf("WiFi connected\n");
}

void loop() {
  // query NTP server
  uint32_t unixtime = (uint32_t) ntp.getUnixTime();
  if(unixtime==0){  // ntp request failed
    Serial.printf("NTP fail\n");
    delay(1500);    // wait 15 secs
    return;         // before retry
  }

  if (update_rtc(ds1307, unixtime)){
    // sucesful update: DS1307/DS3231
    oled_time("RTC", ds1307.now(), "DS1307/DS3231");
  } else if(update_rtc(pcf8523, unixtime)){
    // sucesful update: PCF8523/PCF8563(?)
    oled_time("RTC", pcf8523.now(), "PCF8523/PCF8563");
  } else {
    // no RTC found/updated, report NTC time
    oled_time("NTP", DateTime(unixtime), NTP_POOL);
  }

  delay(6000); // 60 secs
}

bool update_rtc(RTC_DS1307 &rtc, uint32_t unixtime){
  DateTime now = rtc.now();
  if(now.unixtime()==unixtime){
    Serial.printf("RTC OK\n");
    return true;
  }

  // should take way less than 1s to update
  //Serial.printf("RTC sync\n");
  rtc.adjust(DateTime(unixtime));
  now = rtc.now();
  return now.unixtime()==unixtime;
}

bool update_rtc(RTC_PCF8523 &rtc, uint32_t unixtime){
  DateTime now = rtc.now();
  if(now.unixtime()==unixtime){
    return true;
    Serial.printf("RTC OK\n");
  }

  // should take way less than 1s to update
  //Serial.printf("RTC sync\n");
  rtc.adjust(DateTime(unixtime));
  now = rtc.now();
  return now.unixtime()==unixtime;
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

void oled_time(const char* title, DateTime now, const char* msg) {
  // serial message
  Serial.printf("%s %d %s\n",title, now.unixtime(), msg);

  static char buffer[24];
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.setFont(ArialMT_Plain_24);
  display.drawString(64, 12, title);

  // date and time
  display.setFont(ArialMT_Plain_10);
  sprintf(buffer, "%04d-%02d-%02d", now.year(), now.month(), now.day());
  display.drawString(64, 42, buffer);
  sprintf(buffer, "%02d:%02d:%02d", now.hour(), now.minute(), now.second());
  display.drawString(64, 54, buffer);

  // write the buffer to the display
  display.display();
}
