// Local WiFi
#define WIFI_SSID "your_wifi_name"
#define WIFI_PASS "your_wifi_pass"

// NTPtimeESP
#define DEBUG_ON
#define NTP_POOL "pool.ntp.org"

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
