/*
ESPntp2rtc.ino
  https://github.com/avaldebe/ESPntp2rtc

Based on example code from
  https://github.com/SensorsIot/NTPtimeESP
  https://github.com/squix78/esp8266-oled-ssd1306
*/

#include <Wire.h>
#include <SSD1306.h>            // alias for `#include "SSD1306Wire.h"`
#include "images.h"             // WiFi logo
SSD1306  display(0x3c, SDA, SCL); // Initialize the OLED display using Wire library

#include <RTC.h>
RTC ds1307(rtc_t::DS1307);
RTC ds3231(rtc_t::DS3231);
RTC pcf8523(rtc_t::PCF8523);
RTC pcf8563(rtc_t::PCF8563);

#define SECONDS_FROM_1900_TO_1970 2208988800L
#define SECONDS_FROM_1970_TO_2000  946684800L
#ifdef ARDUINO_ARCH_ESP32
 #include <WiFi.h>
#elif defined(ESP8266)
 #include <ESP8266WiFi.h>
#endif
#include <time.h>
#include "config.h"

void setup() {
  Serial.begin(115200);
  Serial.printf("\nBooted\n");

//Wire.begin(SDA, SCL); // not needed, SSD1306Wire->connect() calls Wire.begin(SDA, SCL)
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
    delay(1000);
  }
  Serial.printf("\nWiFi connected\n");

  Serial.printf("NTP sync to %s\n", NTP_POOL);
  configTime(TIME_ZONE, TIME_DST, NTP_POOL); // see config.h
  while (time(NULL)<SECONDS_FROM_1970_TO_2000){
    Serial.print(".");
    delay(1000);
  }
  Serial.printf("\nNTP synced\n");
  oled_time("NTP", time(NULL), NTP_POOL);
}

void loop() {
  // get time for internal clock, which is NTP synced
  time_t unixtime = time(NULL);

  if (update_rtc(ds1307, unixtime, "DS1307")){
    // sucesful update: DS1307
    oled_time("RTC", ds1307.now(), "DS1307");
  } else if (update_rtc(ds3231, unixtime, "DS3231")){
      // sucesful update: DS3231
      oled_time("RTC", ds3231.now(), "DS3231");
  } else if(update_rtc(pcf8523, unixtime, "PCF8523")){
    // sucesful update: PCF8523
    oled_time("RTC", pcf8523.now(), "PCF8523");
  } else if(update_rtc(pcf8563, unixtime, "PCF8563")){
    // sucesful update: PCF8563
    oled_time("RTC", pcf8563.now(), "PCF8563");
  } else {
    // no RTC found/updated, report SYSTEM time
    oled_time("SYS", unixtime, "");
  }

  delay(6000); // 60 secs
}

bool update_rtc(RTC &rtc, time_t unixtime, const char* name){
  const char fmt[] = "RTC %s %s %d\n";
#ifdef DEBUG
 #define debug(s,d) Serial.printf(fmt,name,s,d)
#else
 #define debug(s,d) //skip this line
#endif

  if(!rtc.isrunning()){
    debug("not found", -1);
    return false;
  }

  time_t now = rtc.now();
  if(now==unixtime){
    debug("on sync", 0);
    return true;
  }

  debug("now", now);
  debug("off by", now-unixtime);
  rtc.adjust(unixtime);
  now = rtc.now();
  debug("off by", now-unixtime);

  // should take way less than 1s to update
  return now==unixtime;
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

void oled_time(const char* title, time_t now, const char* msg) {
  // serial message
  Serial.printf("%s %d %s\n",title, now, msg);

  display.clear();
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.setFont(ArialMT_Plain_24);
  display.drawString(64, 12, title);

  // date and time
  display.setFont(ArialMT_Plain_10);
  const uint8_t chlen = 24;
  static char buffer[chlen];
  struct tm *timeinfo = localtime(&now);
  strftime(buffer, chlen, "%F", timeinfo);
  display.drawString(64, 42, buffer);
  strftime(buffer, chlen, "%T", timeinfo);
  display.drawString(64, 54, buffer);

  // write the buffer to the display
  display.display();
}
