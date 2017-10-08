#ifndef _UXTC_H_
#define _UXTC_H_

#include <Arduino.h>
#include <Wire.h>
#include <time.h>
#include <assert.h>

class STC { // System cTime Clock
protected:
  uint8_t zone, dst;
  const char *ntp0, *ntp1, *ntp2;

public:
  STC(uint8_t time_zone=0, uint8_t time_dst=0,
     const char *ntp_server0="pool.ntp.org",
     const char *ntp_server1="time.nist.gov",
     const char *ntp_server2="");

  void init(boolean verbose=false);

  static time_t now() { return time(NULL); }

  // formatted string from a ctime type, like the linux date command
  static char *date (const struct tm *timeinfo, const char *fmt="%F %T"){
    // wrap strftime as a function with a pre-deffined buffer
    const uint8_t chlen = 32;
    static char buffer[chlen];
    strftime(buffer, chlen, fmt, timeinfo);
    return buffer;
  }
  static char *date (const time_t &now, const char *fmt="%F %T") {
    struct tm *timeinfo = localtime(&now);
    return date(timeinfo, fmt);
  }
  static char *date (const char *fmt="%F %T"){
    return date(now(), fmt);
  }
};

enum class rtc_t {DS1307, DS3231, PCF8523, PCF8563};

class RTC { // real ctime clocks
public:
  RTC(const rtc_t &rtc);

  // check if can found a RTC on its i2c address
  boolean isrunning();

  // adjust the RTC from a ctime type
  void adjust(const time_t &now);
  void adjust(const struct tm *timeinfo);

  // read the RTC and return an ctime type
  time_t now();
  struct tm *timeinfo();

protected:
  rtc_t rtc_id;
  uint8_t address, sec_reg;

  // utility functions
  static uint8_t bcd2bin (uint8_t val) { return val - 6 * (val >> 4); }
  static uint8_t bin2bcd (uint8_t val) { return val + 6 * (val / 10); }

  // debug printout
  static void printftime (const struct tm *timeinfo, const char *title="", const char *msg=""){
    Serial.printf("%s %s %s\n", title, STC::date(timeinfo), msg);
  }
  static void printftime (const time_t &now, const char *title="", const char *msg=""){
    Serial.printf("%s %s %s\n", title, STC::date(now), msg);
  }
};

#endif // _UXTC_H_
