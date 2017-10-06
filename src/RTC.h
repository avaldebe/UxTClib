#ifndef _RTC_H_
#define _RTC_H_

#include <Arduino.h>
#include <time.h>

enum class rtc_t {DS1307, DS3231, PCF8523, PCF8563};

class RTC {
public:
    RTC(const rtc_t &rtc);
    boolean isrunning();
    void adjust(const time_t &now);
    time_t now();

    // utility functions
    static uint8_t bcd2bin (uint8_t val) { return val - 6 * (val >> 4); }
    static uint8_t bin2bcd (uint8_t val) { return val + 6 * (val / 10); }
    static void printftime (const struct tm *timeinfo, const char *title=""){
      const uint8_t chlen = 24;
      static char buffer[chlen];
      strftime(buffer, chlen, "%F %T", timeinfo);
      Serial.printf("%s %s\n", title, buffer);
    }
    static void printftime (const time_t &now, const char *title=""){
      struct tm *timeinfo = localtime(&now);
      printftime(timeinfo, title);
    }

  protected:
    rtc_t rtc_id;
    uint8_t address, sec_reg;
};



#endif // _RTC_H_
