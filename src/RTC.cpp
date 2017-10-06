#include <Arduino.h>
#include <Wire.h>
#include <RTClib.h>
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

// DateTime.year() = years since 0
const uint16_t y2k = 2000;

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

void RTC::adjust(const DateTime timeinfo) {
  Wire.beginTransmission(address);
  // wipe control/status registers
  Wire.write((uint8_t)0);
  for(uint8_t reg=0x00; reg<sec_reg; reg++){
    Wire.write((uint8_t) 0x0);
  }
  // write time/date registers
  switch (rtc_id) {
    case rtc_t::DS1307:
    case rtc_t::DS3231:
      Wire.write(bin2bcd(timeinfo.second()));
      Wire.write(bin2bcd(timeinfo.minute()));
      Wire.write(bin2bcd(timeinfo.hour()));
      Wire.write(bin2bcd(0)); // skip day of the week
      Wire.write(bin2bcd(timeinfo.day()));
      Wire.write(bin2bcd(timeinfo.month()));
      Wire.write(bin2bcd(timeinfo.year() - y2k));
      break;
    case rtc_t::PCF8523:
    case rtc_t::PCF8563:
      Wire.write(bin2bcd(timeinfo.second()));
      Wire.write(bin2bcd(timeinfo.minute()));
      Wire.write(bin2bcd(timeinfo.hour()));
      Wire.write(bin2bcd(timeinfo.day()));
      Wire.write(bin2bcd(0)); // skip day of the week
      Wire.write(bin2bcd(timeinfo.month()));
      Wire.write(bin2bcd(timeinfo.year() - y2k));
      break;
  }
  Wire.endTransmission();
}
void RTC::adjust(const uint32_t now) {
  adjust(DateTime(now));
}

DateTime RTC::timeinfo() {
  Wire.beginTransmission(address);
  Wire.write(sec_reg);
  Wire.endTransmission();

  Wire.requestFrom(address, (uint8_t)7);
  uint8_t sec, min, hour, mday, mon, year;
  switch (rtc_id) {
    case rtc_t::DS1307:
    case rtc_t::DS3231:
      sec = bcd2bin(Wire.read() & 0x7F);
      min = bcd2bin(Wire.read() & 0x7F);
      hour= bcd2bin(Wire.read() & 0x3F);
      Wire.read(); // skip day of the week
      mday= bcd2bin(Wire.read() & 0x3F);
      mon = bcd2bin(Wire.read() & 0x1F);
      year= bcd2bin(Wire.read()) + y2k;
      break;
    case rtc_t::PCF8523:
    case rtc_t::PCF8563:
      sec = bcd2bin(Wire.read() & 0x7F);
      min = bcd2bin(Wire.read() & 0x7F);
      hour= bcd2bin(Wire.read() & 0x3F);
      mday= bcd2bin(Wire.read() & 0x3F);
      Wire.read(); // skip day of the week
      mon = bcd2bin(Wire.read() & 0x1F);
      year= bcd2bin(Wire.read()) + y2k;
      break;
  }
  return DateTime(year, mon, mday, hour, min, sec);
}
uint32_t RTC::now() {
  return timeinfo().unixtime();
}
