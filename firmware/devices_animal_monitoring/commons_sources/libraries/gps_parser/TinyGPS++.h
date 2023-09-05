/*
TinyGPS++ - a small GPS library for Arduino providing universal NMEA parsing
Based on work by and "distanceBetween" and "courseTo" courtesy of Maarten Lamers.
Suggestion to add satellites, courseTo(), and cardinal() by Matt Monson.
Location precision improvements suggested by Wayne Holder.
Copyright (C) 2008-2013 Mikal Hart
All rights reserved.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef __TinyGPSPlus_h
#define __TinyGPSPlus_h
#include <gps_parser/TinyGPS_DEFINES.h>
#include <limits.h>//set the max value of types of data
#include <stdint.h>//allows the use of Stdint
#include <string.h>//allows the use of string commannds
#ifdef CC1312R
#include <ti/sysbios/knl/Clock.h> //TI RTOSto acces task gettick
uint32_t millis(void);
#endif

#define _GPS_VERSION "1.1" // software version of this library

enum GPS_senstence_types{GPS_SENTENCE_GPGGA, GPS_SENTENCE_GPRMC,GPS_SENTENCE_GPGSA, GPS_SENTENCE_OTHER};


#define _GPS_MPH_PER_KNOT 1.15077945
#define _GPS_MPS_PER_KNOT 0.51444444
#define _GPS_KMPH_PER_KNOT 1.852
#define _GPS_MILES_PER_METER 0.00062137112
#define _GPS_KM_PER_METER 0.001
#define _GPS_FEET_PER_METER 3.2808399

#define _GPS_MAX_FIELD_SIZE 15// 15 IS ORIGINAL SIZE, its works ok with that

struct RawDegrees
{
   uint16_t deg;
   uint32_t billionths;
   bool negative;
public:
   RawDegrees() : deg(0), billionths(0), negative(false)
   {}
};

struct TinyGPSLocation
{
   friend class TinyGPSPlus;
public:
   bool isValid()     const  { return valid; }
   bool isUpdated() const  { return updated; }
   uint32_t age() const    { return valid ? millis() - lastCommitTime : (uint32_t)ULONG_MAX; }
   const RawDegrees &rawLat()     { updated = false; return rawLatData; }
   const RawDegrees &rawLng()     { updated = false; return rawLngData; }
   double lat();
   double lng();

   TinyGPSLocation() : valid(false), updated(false)
   {}
private:
   bool valid, updated;
   RawDegrees rawLatData, rawLngData, rawNewLatData, rawNewLngData;
   uint32_t lastCommitTime;
   void commit();
   void setLatitude(const char *term);
   void setLongitude(const char *term);
};

struct TinyGPSDate
{
   friend class TinyGPSPlus;
public:
   bool isValid() const       { return valid; }
   bool isUpdated() const     { return updated; }
   uint32_t age() const       { return valid ? millis() - lastCommitTime : (uint32_t)ULONG_MAX; }

   uint32_t value()           { updated = false; return date; }
   uint16_t year();
   uint8_t month();
   uint8_t day();
   void date_str_inverted(char* str);
   TinyGPSDate() : valid(false), updated(false), date(0)
   {}

private:
   bool valid, updated;
   uint32_t date, newDate;
   uint32_t lastCommitTime;
   void commit();
   void setDate(const char *term);
};

struct TinyGPSTime
{
   friend class TinyGPSPlus;
public:
   bool isValid() const       { return valid; }
   bool isUpdated() const     { return updated; }
   uint32_t age() const       { return valid ? millis() - lastCommitTime : (uint32_t)ULONG_MAX; }

   uint32_t value()           { updated = false; return time; }
   uint8_t hour();
   uint8_t minute();
   uint8_t second();
   uint8_t centisecond();

   TinyGPSTime() : valid(false), updated(false), time(0)
   {}

private:
   bool valid, updated;
   uint32_t time, newTime;
   uint32_t lastCommitTime;
   void commit();
   void setTime(const char *term);
};

struct TinyGPSDecimal
{
   friend class TinyGPSPlus;
public:
   bool isValid() const    { return valid; }
   bool isUpdated() const  { return updated; }
   uint32_t age() const    { return valid ? millis() - lastCommitTime : (uint32_t)ULONG_MAX; }
   int32_t value()         { updated = false; return val; }

   TinyGPSDecimal() : valid(false), updated(false), val(0)
   {}

private:
   bool valid, updated;
   uint32_t lastCommitTime;
   int32_t val, newval;
   void commit();
   void set(const char *term);
};

struct TinyGPSInteger
{
   friend class TinyGPSPlus;
public:
   bool isValid() const    { return valid; }
   bool isUpdated() const  { return updated; }
   uint32_t age() const    { return valid ? millis() - lastCommitTime : (uint32_t)ULONG_MAX; }
   uint32_t value()        { updated = false; return val; }

   TinyGPSInteger() : valid(false), updated(false), val(0)
   {}

private:
   bool valid, updated;
   uint32_t lastCommitTime;
   uint32_t val, newval;
   void commit();
   void set(const char *term);
};

struct TinyGPSDOP
{
   friend class TinyGPSPlus;
public:
   bool isValid()     const  { return valid; }
   bool isUpdated() const  { return updated; }
   uint32_t age() const    { return valid ? millis() - lastCommitTime : (uint32_t)ULONG_MAX; }
   double hdop() { updated = false; return hdop_ok; }
   double vdop(){ updated = false; return vdop_ok; }
   double pdop(){ updated = false; return pdop_ok; }

   TinyGPSDOP() : valid(false), updated(false)
   {}
private:
   bool valid, updated;
   uint32_t lastCommitTime;
   double newhdop,newvdop,newpdop,hdop_ok,vdop_ok,pdop_ok;
   void commit();
   void setHDOP(const char *term);
   void setVDOP(const char *term);
   void setPDOP(const char *term);
};


struct TinyGPSSpeed : TinyGPSDecimal
{
   double knots()    { return value() / 100.0; }
   double mph()      { return _GPS_MPH_PER_KNOT * value() / 100.0; }
   double mps()      { return _GPS_MPS_PER_KNOT * value() / 100.0; }
   double kmph()     { return _GPS_KMPH_PER_KNOT * value() / 100.0; }
};

struct TinyGPSCourse : public TinyGPSDecimal
{
   double deg()      { return value() / 100.0; }
};

struct TinyGPSAltitude : TinyGPSDecimal
{
   double meters()       { return value() / 100.0; }
   double miles()        { return _GPS_MILES_PER_METER * value() / 100.0; }
   double kilometers()   { return _GPS_KM_PER_METER * value() / 100.0; }
   double feet()         { return _GPS_FEET_PER_METER * value() / 100.0; }
};

struct TinyGPSHDOP : TinyGPSDecimal
{
   double val() { return value() / 100.00; }
};



class TinyGPSPlus;
class TinyGPSCustom
{
public:
   TinyGPSCustom() {};
   TinyGPSCustom(TinyGPSPlus &gps, const char *sentenceName, int termNumber);
   void begin(TinyGPSPlus &gps, const char *_sentenceName, int _termNumber);
   bool isUpdated() const  { return updated; }
   bool isValid()   const  { return valid; }
   uint32_t age() const    { return valid ? millis() - lastCommitTime : (uint32_t)ULONG_MAX; }
   const char *value()     { updated = false; return buffer; }

private:
   void commit();
   void set(const char *term);

   char stagingBuffer[_GPS_MAX_FIELD_SIZE + 1];
   char buffer[_GPS_MAX_FIELD_SIZE + 1];
   unsigned long lastCommitTime;
   bool valid, updated;
   const char *sentenceName;
   int termNumber;
   friend class TinyGPSPlus;
   TinyGPSCustom *next;
};



class TinyGPSPlus
{
public:
	TinyGPSPlus();
#ifdef  _GPS_STORE_COMPLETE_MESSAGE
   bool isValid() const          { return valid; }
   bool isUpdated()    const      { return updated; } //{ return updated; }
   void get_last_message_ok(char* dest_char_p) { updated = false; strncpy(dest_char_p, complete_message_ok,_GPS_MESSAGE_FIELD_SIZE); }//complete_message_ok
   uint8_t get_last_message_ok_type()       { return complete_message_type; }

#endif

  void parse_reset(void);
  void parse_chararray(char* char_pr,uint16_t bytes_to_parse);
  bool parse(char c); // process one character received from GPS
  TinyGPSPlus &operator << (char c) {parse(c); return *this;}
  uint8_t getcommas()     const{ return commas; }
  TinyGPSLocation location;
  TinyGPSDate date;
  TinyGPSTime time;
  TinyGPSSpeed speed;
  TinyGPSCourse course;
  TinyGPSAltitude altitude;
  TinyGPSInteger satellites;
  TinyGPSDOP dop;
  TinyGPSHDOP hdop_only;//original:Horizontal Dilution of precision (DOP)
  static const char *libraryVersion() { return _GPS_VERSION; }

  static double distanceBetween(double lat1, double long1, double lat2, double long2);
  static double courseTo(double lat1, double long1, double lat2, double long2);
  static const char *cardinal(double course);

  static int32_t parseDecimal(const char *term);
  static void parseDegrees(const char *term, RawDegrees &deg);

  uint32_t charsProcessed()   const { return parsedCharCount; }
  uint32_t sentencesWithFix() const { return sentencesWithFixCount; }
  uint32_t failedChecksum()   const { return failedChecksumCount; }
  uint32_t passedChecksum()   const { return passedChecksumCount; }

private:

#define _GPRMCterm      "GPRMC"
#define _GPRMC_commas      12
#define _GPGGAterm      "GPGGA"
#define _GPGGA_commas      14
#define _GPGSAterm      "GPGSA" //added to get position resolution
#define _GPGSA_commas      17
//Non parced, but leaved available
#define _GNRMCterm      "GNRMC"
#define _GNGGAterm      "GNGGA"
#define _GNGSAterm      "GNGSA"
  // parsing state variables
  uint8_t parity;
  uint8_t commas;
  bool isChecksumTerm;
  char term[_GPS_MAX_FIELD_SIZE];
  uint8_t curSentenceType;
  uint8_t curFieldNumber;
  uint8_t curTermOffset;
  bool sentenceHasFix;

  // custom element support
  friend class TinyGPSCustom;
  TinyGPSCustom *customElts;
  TinyGPSCustom *customCandidates;
  void insertCustom(TinyGPSCustom *pElt, const char *sentenceName, int index);

  // statistics
  uint32_t parsedCharCount;
  uint32_t sentencesWithFixCount;
  uint32_t failedCommaCount;
  uint32_t failedChecksumCount;
  uint32_t passedChecksumCount;
#ifdef  _GPS_STORE_COMPLETE_MESSAGE
  uint8_t complete_message_type;
  char complete_message_ok[_GPS_MESSAGE_FIELD_SIZE];//to store the message being evaluated
  char complete_message_to_eval[_GPS_MESSAGE_FIELD_SIZE];//to store the message evaluated and ok
  uint8_t complete_message_offset;
#endif
  //status
  bool valid, updated;
  // internal utilities
  int fromHex(char a);
  bool endOfTermHandler();
};

#endif // def(__TinyGPSPlus_h)
