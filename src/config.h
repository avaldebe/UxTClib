// Local WiFi
#ifndef WIFI_SSID
#define WIFI_SSID "wifi_ssid"
#endif
#ifndef WIFI_PASS
#define WIFI_PASS "wifi_pass"
#endif

// EasyNTPClient
#define NTP_POOL "pool.ntp.org"
#define TIME_UTC     0 // UTC+00:00
#define TIME_CET  3600 // UTC+01:00
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
#define DS1307_BUS  D2,D1
#define DS3231_BUS  D5,D1
#define PCF8523_BUS D6,D1
#endif
