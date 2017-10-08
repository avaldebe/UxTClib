#include <Arduino.h>
#include <Wire.h>
#include <time.h>
#include <assert.h>
#include "UxTClib.h"

const uint32_t
  SECONDS_FROM_1900_TO_1970 = 2208988800,
  SECONDS_FROM_1970_TO_2000 =  946684800;

STC::STC(uint8_t time_zone, uint8_t time_dst,
   const char *ntp_server0,
   const char *ntp_server1,
   const char *ntp_server2) {
   zone = time_zone;
   dst = time_dst;
   ntp0 = ntp_server0;
   ntp1 = ntp_server1;
   ntp2 = ntp_server2;
 }

void STC::init(boolean verbose){
  if(verbose){
    Serial.printf("NTP sync to %s %s %s\n", ntp0, ntp1, ntp2);
    configTime(zone, dst, ntp0, ntp1, ntp2);
    while (time(NULL)<SECONDS_FROM_1970_TO_2000){
      Serial.printf(".");
      delay(1000);
    }
    Serial.printf("\nNTP synced\n");
  } else {
    configTime(zone, dst, ntp0, ntp1, ntp2);
    while (time(NULL)<SECONDS_FROM_1970_TO_2000){ delay(1000); }
  }
}

// tm_year = years since 1900
const uint16_t epoch = 1900, y2k = 2000 - epoch;

RTC::RTC(const rtc_t& rtc) {
  rtc_id = rtc;
  switch (rtc_id) {
    case rtc_t::DS1307:
    case rtc_t::DS3231:
      address = 0x68;
      sec_reg = 0x00;
      break;
    case rtc_t::PCF8523:
      address = 0x68;
      sec_reg = 0x03;
      break;
    case rtc_t::PCF8563:
      address = 0x51;
      sec_reg = 0x02;
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
  Serial.printf("  RTC year %4d, bcd %3d, found %s\n",
    bcd2bin(bcd)+2000, bcd, (bcd != 0xFF)?"T":"F");
#endif
  return (bcd != 0xFF);
}

void RTC::adjust(const struct tm *timeinfo) {
#ifdef DEBUG
  printftime(timeinfo, " ", "write RTC");
#endif
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
#ifdef DEBUG
  printftime(now, " ", "SYS");
#endif
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
  printftime(timeinfo, " ", "read RTC");
#endif
  return timeinfo;
}
time_t RTC::now() {
  struct tm *timeinfo = RTC::timeinfo();
  return mktime(timeinfo);
}
