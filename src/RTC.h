#ifndef _RTC_H_
#define _RTC_H_

#include <Arduino.h>
#include <RTClib.h>

enum class rtc_t {DS1307, DS3231, PCF8523, PCF8563};

class RTC {
  public:
    RTC(const rtc_t &rtc);
    boolean isrunning();
    void adjust(const DateTime timeinfo);
    void adjust(const uint32_t now);
    DateTime now();

  protected:
    rtc_t rtc_id;
    uint8_t address, sec_reg;
    
    // utility functions
    static uint8_t bcd2bin (uint8_t val) { return val - 6 * (val >> 4); }
    static uint8_t bin2bcd (uint8_t val) { return val + 6 * (val / 10); }

    // debug printout
    static void printftime (const DateTime now, const char *title="", const char *msg=""){
      const uint8_t chlen = 24;
      static char buffer[chlen];
      sprintf(buffer, "%04d-%02d-%02d %02d:%02d:%02d",
        now.year(), now.month(), now.day(),
        now.hour(), now.minute(), now.second());
      Serial.printf("%s %s %s\n", title, buffer, msg);
    }
    static void printftime (const uint32_t now, const char *title="", const char *msg=""){
      printftime(DateTime(now), title, msg);
    }
};



#endif // _RTC_H_
