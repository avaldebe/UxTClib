/*
ESPntp2rtc.ino
  https://github.com/avaldebe/ESPntp2rtc

Based on example code from
  https://github.com/SensorsIot/NTPtimeESP
  https://github.com/squix78/esp8266-oled-ssd1306

NOTE:
  WIFI_SSID, WIFI_PASS and NTP_POOL are defined in platformio.ini
*/

#include <Wire.h>
#include <SSD1306.h>            // alias for `#include "SSD1306Wire.h"`
#include "images.h"             // WiFi logo
SSD1306  display(0x3c, SDA, SCL); // Initialize the OLED display using Wire library

#include <UxTClib.h>
// System cTime Clock
//   set for UTC time, no DST (daylight saving time)
//   local time and DST should be dealth on the target application
//   here we just set the RTC
STC systime(0, 0, NTP_POOL);

// all currently supported RTCs
const uint8_t rtc_tot = 4;
const char *rtcname[rtc_tot] = {"DS1307", "DS3231", "PCF8523", "PCF8563"};
RTC rtc[rtc_tot]={ RTC(rtc_t::DS1307), RTC(rtc_t::DS3231),
                   RTC(rtc_t::PCF8523), RTC(rtc_t::PCF8563)};

#define SECONDS_FROM_1900_TO_1970 2208988800UL
#define SECONDS_FROM_1970_TO_2000  946684800UL
#ifdef ARDUINO_ARCH_ESP32
 #include <WiFi.h>
#elif defined(ESP8266)
 #include <ESP8266WiFi.h>
#endif
#include <time.h>

void wifi_init(boolean verbose=false);

void setup() {
  Serial.begin(115200);
  Serial.printf("\nProgram: %s\n", __FILE__);
  Serial.printf("Build: %s %s\n", __DATE__, __TIME__);

//Wire.begin(SDA, SCL); // not needed, SSD1306Wire->connect() calls Wire.begin(SDA, SCL)
  display.init();
  display.flipScreenVertically();
  oled_wifi();

  // start wify and systime
  wifi_init(true);    // verbose mode
  systime.init(true); // verbose mode
  oled_time("NTP", time(NULL), NTP_POOL);
  delay(1500); // wait 15s until 1st update
}

void loop() {
  // get time for internal clock, which is NTP synced
  time_t unixtime = systime.now();

  // try updating the RTCs one at a time
  static uint8_t n=0;
  n %= rtc_tot; // there is only rtc_tot RTCs to try
  if (update_rtc(rtc[n], unixtime, rtcname[n])){
    oled_time("RTC", rtc[n].now(), rtcname[n]);
  } else {
  // no RTC found/updated, report SYSTEM time
    oled_time("SYS", unixtime, "");
    n++;
  }

  delay(6000); // wait 60s until next try
}

void wifi_init(boolean verbose) {
  if(verbose){
    Serial.printf("Connecting to %s\n", WIFI_SSID);
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASS);
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

  } else {
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    while (WiFi.status() != WL_CONNECTED) { delay(1000); }
  }
}

bool update_rtc(RTC &rtc, time_t unixtime, const char* name){

#ifdef DEBUG
 #define debug(s,d) Serial.printf("RTC %s %s %d\n",name,s,d)
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
  Serial.printf("%s %d %s\n",title, (uint32_t)now, msg);

  display.clear();
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.setFont(ArialMT_Plain_24);
  display.drawString(64, 12, title);

  // date and time
  display.setFont(ArialMT_Plain_10);
  display.drawString(64, 42, systime.date(now,"%F"));
  display.drawString(64, 54, systime.date(now,"%T"));

  // write the buffer to the display
  display.display();
}
