// Local WiFi
#ifndef WIFI_SSID
#define WIFI_SSID "wifi_ssid"
#endif
#ifndef WIFI_PASS
#define WIFI_PASS "wifi_pass"
#endif

// NTPtimeESP
#define DEBUG_ON
#define NTP_POOL "pool.ntp.org"
#define TIME_UTC  0.0,0 // UTC+00:00,None
#define TIME_GMT  0.0,1 // UTC+00:00,European summer time
#define TIME_CET  1.0,1 // UTC+01:00,European summer time
#ifndef TIME_ZONE
#define TIME_ZONE TIME_UTC
#endif

// I2C bus(es)
#ifdef SINGLE_I2C_BUS
// all RTCs on the same bus
#define DS1307_BUS  SDA,SCL
#define DS3231_BUS  SDA,SCL
#define PCF8523_BUS SDA,SCL
#else
// each RTC on a different I2C bus
#define DS1307_BUS  D1,D2
#define DS3231_BUS  D1,D5
#define PCF8523_BUS D1,D6
#endif
