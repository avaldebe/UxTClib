#include <Arduino.h>
#include <Wire.h>
#include <time.h>
#include <assert.h>
#include "RTC.h"

#define DS1307_ADDRESS    0x68
#define DS1307_SEC_REG    0x00

#define DS3231_ADDRESS    0x68
#define DS3231_SEC_REG    0x00

#define PCF8523_ADDRESS   0x68
#define PCF8523_SEC_REG   0x03

#define PCF8563_ADDRESS   0x51
#define PCF8563_SEC_REG   0x02

// tm_year = years since 1900
const uint16_t epoch = 1900, y2k = 2000 - epoch;

RTC::RTC(const rtc_t& rtc) {
  rtc_id = rtc;
  switch (rtc_id) {
    case rtc_t::DS1307:
      address = DS1307_ADDRESS;
      sec_reg = DS1307_SEC_REG;
      break;
    case rtc_t::DS3231:
      address = DS3231_ADDRESS;
      sec_reg = DS3231_SEC_REG;
      break;
    case rtc_t::PCF8523:
      address = PCF8523_ADDRESS;
      sec_reg = PCF8523_SEC_REG;
      break;
    case rtc_t::PCF8563:
      address = PCF8563_ADDRESS;
      sec_reg = PCF8563_SEC_REG;
      break;
    default:
      Serial.printf("RTC not implemented\n");
      delay(1000);
      assert(false);
  }
}

boolean RTC::isrunning() {
  Wire.beginTransmission(address);
  Wire.write(sec_reg+6);
  Wire.endTransmission();

  Wire.requestFrom(address, (uint8_t)1);
  uint8_t bcd = Wire.read();
#ifdef DEBUG
  Serial.printf("RTC year %4d, bcd %3d, found %s\n",
    bcd2bin(bcd)+2000, bcd, (bcd != 0xFF)?"T":"F");
#endif
  return (bcd != 0xFF);
}

void RTC::adjust(const struct tm *timeinfo) {
  Wire.beginTransmission(address);
  Wire.write(sec_reg);
  switch (rtc_id) {
    case rtc_t::DS1307:
    case rtc_t::DS3231:
      Wire.write(bin2bcd(timeinfo->tm_sec));
      Wire.write(bin2bcd(timeinfo->tm_min));
      Wire.write(bin2bcd(timeinfo->tm_hour));
      Wire.write(bin2bcd(0)); // skip day of the week
      Wire.write(bin2bcd(timeinfo->tm_mday));
      Wire.write(bin2bcd(timeinfo->tm_mon));
      Wire.write(bin2bcd(timeinfo->tm_year - y2k));
      break;
    case rtc_t::PCF8523:
    case rtc_t::PCF8563:
      Wire.write(bin2bcd(timeinfo->tm_sec));
      Wire.write(bin2bcd(timeinfo->tm_min));
      Wire.write(bin2bcd(timeinfo->tm_hour));
      Wire.write(bin2bcd(timeinfo->tm_mday));
      Wire.write(bin2bcd(0)); // skip day of the week
      Wire.write(bin2bcd(timeinfo->tm_mon));
      Wire.write(bin2bcd(timeinfo->tm_year - y2k));
      break;
  }
  Wire.endTransmission();
}
void RTC::adjust(const time_t &now) {
  struct tm *timeinfo = localtime(&now);
  if(mktime(timeinfo)!=now){
    Serial.printf("ERROR: localtime/mktime missmatch");
    printftime(timeinfo, "  localtime");
    printftime(mktime(timeinfo), "  mktime");
    return;
  }
  adjust(timeinfo);
}

struct tm *RTC::timeinfo() {
  time_t now = time(NULL);
  struct tm *timeinfo = localtime(&now);
  if(mktime(timeinfo)!=now){
    Serial.printf("ERROR: localtime/mktime missmatch");
    printftime(timeinfo, "  localtime");
    printftime(mktime(timeinfo), "  mktime");
    return (time_t)0;
  }

  Wire.beginTransmission(address);
  Wire.write(sec_reg);
  Wire.endTransmission();

  Wire.requestFrom(address, (uint8_t)7);
  switch (rtc_id) {
    case rtc_t::DS1307:
    case rtc_t::DS3231:
      timeinfo->tm_sec = bcd2bin(Wire.read() & 0x7F);
      timeinfo->tm_min = bcd2bin(Wire.read() & 0x7F);
      timeinfo->tm_hour= bcd2bin(Wire.read() & 0x3F);
      Wire.read(); // skip day of the week
      timeinfo->tm_mday= bcd2bin(Wire.read() & 0x3F);
      timeinfo->tm_mon = bcd2bin(Wire.read() & 0x1F);
      timeinfo->tm_year= bcd2bin(Wire.read()) + y2k;
      break;
    case rtc_t::PCF8523:
    case rtc_t::PCF8563:
      timeinfo->tm_sec = bcd2bin(Wire.read() & 0x7F);
      timeinfo->tm_min = bcd2bin(Wire.read() & 0x7F);
      timeinfo->tm_hour= bcd2bin(Wire.read() & 0x3F);
      timeinfo->tm_mday= bcd2bin(Wire.read() & 0x3F);
      Wire.read(); // skip day of the week
      timeinfo->tm_mon = bcd2bin(Wire.read() & 0x1F);
      timeinfo->tm_year= bcd2bin(Wire.read()) + y2k;
      break;
  }
#ifdef DEBUG
  printftime(now, "  SYS");
  printftime(timeinfo, "  RTC");
#endif
  return timeinfo;
}
time_t RTC::now() {
  struct tm *timeinfo = RTC::timeinfo();
  return mktime(timeinfo);
}
