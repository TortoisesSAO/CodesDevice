/*
 * TinyGPS++ modified by Andres Oliva Trevisan
 *
*/
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

#include <ctype.h>
#include <gps_parser/TinyGPS++.h>
#include <stdlib.h>
#include <math.h>

#ifdef CC1312R
//this is used by CC1312R only!
//to manage time count
#define byte uint8_t
#define M_PI		3.14159265358979323846
#define M_1_PI		0.31830988618379067154
//uint32_t millis(void){return(round(Clock_getTicks()*Clock_tickPeriod/1000));}//period is in microseconds
#define   millis()         0
#endif

static uint16_t i_c;//for array


TinyGPSPlus::TinyGPSPlus()
  :  parity(0)
  ,	 commas(0)
  ,  isChecksumTerm(false)
  ,  curSentenceType(GPS_SENTENCE_OTHER)
  ,  curFieldNumber(1)
  ,  curTermOffset(0)
  ,  sentenceHasFix(false)
  ,  customElts(0)
  ,  customCandidates(0)
  ,  parsedCharCount(0)
  ,  sentencesWithFixCount(0)
  ,  failedCommaCount(0)
  ,  failedChecksumCount(0)
  ,  passedChecksumCount(0)
{
  term[0] = '\0';
}

//
// public methods
//
//resets the parse
void TinyGPSPlus::parse_reset(void)
{
     #ifdef  _GPS_STORE_COMPLETE_MESSAGE
	 complete_message_offset=0;
     #endif
	//parse variables
	  term[0] = '\0';
	  parity=0;
	  commas=0;
	  parsedCharCount=0;
	  curTermOffset=0;
	  curFieldNumber=1;
	  isChecksumTerm=false;
	  sentenceHasFix=false;
	  curSentenceType=GPS_SENTENCE_OTHER;
	  //endfotermhandler
	  customElts=0;
	  customCandidates=0;
}
void TinyGPSPlus::parse_chararray(char* char_pr,uint16_t bytes_to_parse)
{//parses an character array (=! string)
	//char_pr=character array pointer
	for (i_c = 0; i_c <bytes_to_parse; i_c++) {
   	 parse(*(char_pr+i_c));}//parse each character of the array
}

bool TinyGPSPlus::parse(char c)
{
bool isValidSentence = false;
 #ifdef  _GPS_STORE_COMPLETE_MESSAGE
       //overflow protecttion: sets complete_message_offset to 0 to disable message store
       if (complete_message_offset==_GPS_MESSAGE_FIELD_SIZE){complete_message_offset=0;}
       //writting mode
       if (complete_message_offset>0){
    	   complete_message_to_eval[complete_message_offset]=c;//store the character
    	   ++complete_message_offset;}//increase the counter
 #endif

++parsedCharCount;
 switch(c)
   {
  case ',': // term terminators
    {parity ^= (uint8_t)c;
     ++commas;}
  case '*':
     {
      if (curTermOffset < sizeof(term))
      {
          term[curTermOffset] = 0;
          isValidSentence = endOfTermHandler();
      }
      ++curFieldNumber;
      curTermOffset = 0;
      isChecksumTerm = c == '*';
      return isValidSentence;
    }
     break;

  case '$': // sentence begin
	 #ifdef  _GPS_STORE_COMPLETE_MESSAGE
	 complete_message_to_eval[0]='$';//store first character of GPS message, which is always '$'
     complete_message_offset=1;      //set the offset counter to enable store of other characters
    #endif
    curFieldNumber = 1;
    curTermOffset = 0;
    parity = 0;
	commas=0;
    curSentenceType = GPS_SENTENCE_OTHER;
    isChecksumTerm = false;
    sentenceHasFix = false;
    return false;

  default: // ordinary characters
	    if (curTermOffset < sizeof(term) - 1)
	      term[curTermOffset++] = c;
	    if (!isChecksumTerm)
	      parity ^= c;
	    if ((isChecksumTerm)and(curTermOffset==2)){
	        //if cheksum term was detected and 2 char where
	    	//detected after this
	    	//so we proceed to perform a check sum and reset the parser
	    if (curTermOffset < sizeof(term))
	      {term[curTermOffset] = 0;
	       isValidSentence = endOfTermHandler();
	       }
	      //at this point, an parser reset is called
	      //this is independent if t
	      parse_reset();
	      return isValidSentence;
	    }
	    return false;
	  }
//out of switch
  return isValidSentence;
}

//
// internal utilities
//
int TinyGPSPlus::fromHex(char a)
{
  if (a >= 'A' && a <= 'F')
    return a - 'A' + 10;
  else if (a >= 'a' && a <= 'f')
    return a - 'a' + 10;
  else
    return a - '0';
}

// static
// Parse a (potentially negative) number with up to 2 decimal digits -xxxx.yy
int32_t TinyGPSPlus::parseDecimal(const char *term)
{
  bool negative = *term == '-';
  if (negative) ++term;
  int32_t ret = 100 * (int32_t)atol(term);
  while (isdigit(*term)) ++term;
  if (*term == '.' && isdigit(term[1]))
  {
    ret += 10 * (term[1] - '0');
    if (isdigit(term[2]))
      ret += term[2] - '0';
  }
  return negative ? -ret : ret;
}

// static
// Parse degrees in that funny NMEA format DDMM.MMMM
void TinyGPSPlus::parseDegrees(const char *term, RawDegrees &deg)
{
  uint32_t leftOfDecimal = (uint32_t)atol(term);
  uint16_t minutes = (uint16_t)(leftOfDecimal % 100);
  uint32_t multiplier = 10000000UL;
  uint32_t tenMillionthsOfMinutes = minutes * multiplier;

  deg.deg = (int16_t)(leftOfDecimal / 100);

  while (isdigit(*term))
    ++term;

  if (*term == '.')
    while (isdigit(*++term))
    {
      multiplier /= 10;
      tenMillionthsOfMinutes += (*term - '0') * multiplier;
    }

  deg.billionths = (5 * tenMillionthsOfMinutes + 1) / 3;
  deg.negative = false;
}

#define COMBINE(sentence_type, term_number) (((unsigned)(sentence_type) << 5) | term_number)

// Processes a just-completed term
// Returns true if new sentence has just passed checksum test and is validated
bool TinyGPSPlus::endOfTermHandler()
{uint8_t i;
uint8_t bool_message_valid=false;
#ifdef  _GPS_STORE_COMPLETE_MESSAGE
	 //valid=false;  //last message recieved is not valid
     //updated=false;//last message recieved is not valid
#endif
   //If it's the checksum term, and the checksum checks out, commit
	//Added function to check number of commas to verify and complete message storage
  if (isChecksumTerm)
  {
    byte checksum = 16 * fromHex(term[0]) + fromHex(term[1]);
    if (checksum == parity)
    {
       passedChecksumCount++;
     if (sentenceHasFix)
            ++sentencesWithFixCount;
      //up to this point, the sentence has been checked an its valid, so we copy it to the proper store point
      //we now proceed to parce for information
        switch(curSentenceType)
        {
         case GPS_SENTENCE_GPRMC:
    	  if ((commas==_GPRMC_commas)and(sentenceHasFix)){
    		 bool_message_valid=true;
    		 complete_message_type=GPS_SENTENCE_GPRMC;
             date.commit();
             time.commit();
            if (sentenceHasFix)
              {
               location.commit();
               speed.commit();
               course.commit();
              }
    	    }
    	   else{++failedCommaCount;}
          break;
         case GPS_SENTENCE_GPGGA:
           if ((commas==_GPGGA_commas)and(sentenceHasFix)){
  		     bool_message_valid=true;
  		     complete_message_type=GPS_SENTENCE_GPGGA;
             time.commit();
              location.commit();
              altitude.commit();
              hdop_only.commit();
              satellites.commit();
             }
           else{++failedCommaCount;}
          break;
         case GPS_SENTENCE_GPGSA: //need to develop properly
           if ((commas==_GPGSA_commas)and(sentenceHasFix)){
      		 complete_message_type=GPS_SENTENCE_GPGSA;
  		     bool_message_valid=true;
               dop.commit();
             }
           else{++failedCommaCount;}
          break;
       }
//------------------------------------------------------
//------------Finish Switch Analisys--------------------
//------------------------------------------------------
#ifdef  _GPS_STORE_COMPLETE_MESSAGE
      if (bool_message_valid==true){
		//sets variables if message is valid and stores the variables
          strncpy(complete_message_ok, complete_message_to_eval,complete_message_offset);//copy all the message evaluated to ok buffer
          for (i = complete_message_offset; i < _GPS_MESSAGE_FIELD_SIZE; i++) {//-1 IS MANDARORY
      	       complete_message_ok[i]=_CHAR_WHITEPACE_NUMBER;}    //fill the rest of the buffer with white space to ensure consistency
          complete_message_offset=0;//This is to tell the parser (TinyGPSPlus::parse) that the message is complete
    						  //The parser will not write complete_message buffer until a new message comes
         valid=true;  //message is valid
         updated=true;//message was updated, this will allow the user to get the value of the last message stored
         }
#endif

      /*
     // Commit all custom listeners of this sentence type
      for (TinyGPSCustom *p = customCandidates; p != NULL && strcmp(p->sentenceName, customCandidates->sentenceName) == 0; p = p->next)
         p->commit();
      */
      return true;
    }
 else//if cheksum fails
    {++failedChecksumCount;}


    return false;
  }

  // the first term determines the sentence type
  if (curFieldNumber == 1)
  {
    if (!strcmp(term, _GPRMCterm) || !strcmp(term, _GNRMCterm)) //strcmp("AB", "AB")=0
      curSentenceType = GPS_SENTENCE_GPRMC;
    else if (!strcmp(term, _GPGGAterm) || !strcmp(term, _GNGGAterm))
      curSentenceType = GPS_SENTENCE_GPGGA;
    else if(!strcmp(term,  _GPGSAterm) || !strcmp(term, _GNGSAterm))
      curSentenceType = GPS_SENTENCE_GPGSA;
    else
      curSentenceType = GPS_SENTENCE_OTHER;
/*  //commented: we are not using the "custom candidates" engine in this proyect
    // Any custom candidates of this sentence type?
    for (customCandidates = customElts; customCandidates != NULL && strcmp(customCandidates->sentenceName, term) < 0; customCandidates = customCandidates->next);
    if (customCandidates != NULL && strcmp(customCandidates->sentenceName, term) > 0)
       customCandidates = NULL;
*/
    return false;
  }
  //------------------------------------------------
  if (curSentenceType != GPS_SENTENCE_OTHER && term[0])
  {
	if (curSentenceType==GPS_SENTENCE_GPRMC){
		switch(curFieldNumber){
		  case 2:time.setTime(term);
		    break;
		  case 3: sentenceHasFix = (term[0] == 'A');//if the term 0 is an A, sentence has fix
			break;
		  case 4:location.setLatitude(term);
		    break;
		  case 5:location.rawNewLatData.negative = term[0] == 'S';
		    break;
		  case 6: location.setLongitude(term);// Longitude
			break;
		  case 7:location.rawNewLngData.negative = term[0] == 'W';
			break;
		  case 8:speed.set(term);// Speed (GPRMC)
		    break;
		  case 9: course.set(term);// Course (GPRMC)
			break;
		  case 10:date.setDate(term); // Date (GPRMC)
			break;
		  }//end of switch
	  }//end of if
	if (curSentenceType==GPS_SENTENCE_GPGGA){
		switch(curFieldNumber){
		  case 2:time.setTime(term);
		    break;
		  case 3:location.setLatitude(term);
			break;
		  case 4:location.rawNewLatData.negative = term[0] == 'S';
			break;
		  case 5:location.setLongitude(term);// Longitude
			break;
		  case 6:location.rawNewLngData.negative = term[0] == 'W';
			break;
		  case 7:sentenceHasFix = term[0] > '0';// Fix data (GPGGA)
			break;
		  case 8: satellites.set(term);// Satellites used (GPGGA)
		    break;
		  case 9: hdop_only.set(term);// updates only hdop. Not suggested to use this value
			break;
		  case 10: altitude.set(term);// Altitude (GPGGA)
			break;
		  }//end of switch
	  }//end of if
	if (curSentenceType==GPS_SENTENCE_GPGSA){
		switch(curFieldNumber){
		  case 3:sentenceHasFix = (term[0] >'1');//if the term 0 > char '1', has  2 = 2D, 3=3D fix.
	        break;
		  case 16: dop.setPDOP(term);// GPGSA pdop
	        break;
		  case 17: dop.setHDOP(term); // GPGSA hdop
	        break;
		  case 18: dop.setVDOP(term);// GPGSA vdop
	        break;
		  }//end of switch
	  }//end of if
  }//end of sentece parsing
  //------------------------------------------------
 /*
  // Set custom values as needed
  for (TinyGPSCustom *p = customCandidates; p != NULL && strcmp(p->sentenceName, customCandidates->sentenceName) == 0 && p->termNumber <= curFieldNumber; p = p->next)
    if (p->termNumber == curFieldNumber)
         p->set(term);
*/
  return false;
}

/* static */
double TinyGPSPlus::distanceBetween(double lat1, double long1, double lat2, double long2)
{
  // returns distance in meters between two positions, both specified
  // as signed decimal-degrees latitude and longitude. Uses great-circle
  // distance computation for hypothetical sphere of radius 6372795 meters.
  // Because Earth is no exact sphere, rounding errors may be up to 0.5%.
  // Courtesy of Maarten Lamers
  double delta = (long1-long2)*M_PI/180;//to convert a radians units
  double sdlong = sin(delta);
  double cdlong = cos(delta);
  lat1 = (lat1)*M_PI/180;//to convert a radians units
  lat2 = (lat2)*M_PI/180;//to convert a radians units
  double slat1 = sin(lat1);
  double clat1 = cos(lat1);
  double slat2 = sin(lat2);
  double clat2 = cos(lat2);
  delta = (clat1 * slat2) - (slat1 * clat2 * cdlong);
  delta = sqrt(delta);
  delta += sqrt(clat2 * sdlong);
  delta = sqrt(delta);
  double denom = (slat1 * slat2) + (clat1 * clat2 * cdlong);
  delta = atan2(delta, denom);
  return delta * 6372795;
}

double TinyGPSPlus::courseTo(double lat1, double long1, double lat2, double long2)
{
  // returns course in degrees (North=0, West=270) from position 1 to position 2,
  // both specified as signed decimal-degrees latitude and longitude.
  // Because Earth is no exact sphere, calculated course may be off by a tiny fraction.
  // Courtesy of Maarten Lamers
  double dlon = (long2-long1)*M_PI/180;//convert to radians values
  lat1 = (lat1)*M_PI/180;
  lat2 = (lat2)*M_PI/180;
  double a1 = sin(dlon) * cos(lat2);
  double a2 = sin(lat1) * cos(lat2) * cos(dlon);
  a2 = cos(lat1) * sin(lat2) - a2;
  a2 = atan2(a1, a2);
  if (a2 < 0.0)
  {
    a2 += 2*M_PI;//original is TWO_PI, which is equal to 2*pi= 6.283...
  }
  return (a2*(180*M_1_PI));//convert to degrees
}

const char *TinyGPSPlus::cardinal(double course)
{
  static const char* directions[] = {"N", "NNE", "NE", "ENE", "E", "ESE", "SE", "SSE", "S", "SSW", "SW", "WSW", "W", "WNW", "NW", "NNW"};
  int direction = (int)((course + 11.25f) / 22.5f);
  return directions[direction % 16];
}

void TinyGPSLocation::commit()
{
   rawLatData = rawNewLatData;
   rawLngData = rawNewLngData;
   lastCommitTime = millis();
   valid = updated = true;
}

void TinyGPSLocation::setLatitude(const char *term)
{
   TinyGPSPlus::parseDegrees(term, rawNewLatData);
}

void TinyGPSLocation::setLongitude(const char *term)
{
   TinyGPSPlus::parseDegrees(term, rawNewLngData);
}

double TinyGPSLocation::lat()
{
   updated = false;
   double ret = rawLatData.deg + rawLatData.billionths / 1000000000.0;
   return rawLatData.negative ? -ret : ret;
}

double TinyGPSLocation::lng()
{
   updated = false;
   double ret = rawLngData.deg + rawLngData.billionths / 1000000000.0;
   return rawLngData.negative ? -ret : ret;
}

void TinyGPSDate::commit()
{
   date = newDate;
   lastCommitTime = millis();
   valid = updated = true;
}

void TinyGPSTime::commit()
{
   time = newTime;
   lastCommitTime = millis();
   valid = updated = true;
}

void TinyGPSTime::setTime(const char *term)
{
   newTime = (uint32_t)TinyGPSPlus::parseDecimal(term);
}

void TinyGPSDate::setDate(const char *term)
{
   newDate = atol(term);
}
uint16_t TinyGPSDate::year()
{
   updated = false;
   uint16_t year = date % 100;
   return year + 2000;
}

uint8_t TinyGPSDate::month()
{
   updated = false;
   return (date / 100) % 100;
}

uint8_t TinyGPSDate::day()
{
   updated = false;
   return date / 10000;
}

uint8_t TinyGPSTime::hour()
{
   updated = false;
   return time / 1000000;
}

void TinyGPSDate::date_str_inverted(char* str)
{
	   updated = false;
	   uint8_t  year = date % 100;
	   uint8_t  month= (date / 100) % 100;
	   uint8_t  day  =  date / 10000 ;
	   str[0]='2';//the date is 20 for the following 77 years
	   str[1]='0';
	   str[2]=(year / 10  +48);
	   str[3]=(year % 10  +48);
	   str[4]=(month / 10 +48);
	   str[5]=(month % 10 +48);
	   str[6]=(day / 10   +48);
	   str[7]=(day % 10   +48);
}

uint8_t TinyGPSTime::minute()
{
   updated = false;
   return (time / 10000) % 100;
}

uint8_t TinyGPSTime::second()
{
   updated = false;
   return (time / 100) % 100;
}

uint8_t TinyGPSTime::centisecond()
{
   updated = false;
   return time % 100;
}

void TinyGPSDecimal::commit()
{
   val = newval;
   lastCommitTime = millis();
   valid = updated = true;
}

void TinyGPSDecimal::set(const char *term)
{
   newval = TinyGPSPlus::parseDecimal(term);
}

void TinyGPSInteger::commit()
{
   val = newval;
   lastCommitTime = millis();
   valid = updated = true;
}

void TinyGPSInteger::set(const char *term)
{
   newval = atol(term);
}

void TinyGPSDOP::setHDOP(const char *term)
{
	updated = false;
	newhdop=  TinyGPSPlus::parseDecimal(term);
}
void TinyGPSDOP::setVDOP(const char *term)
{
	updated = false;
	newvdop=  TinyGPSPlus::parseDecimal(term);
}
void TinyGPSDOP::setPDOP(const char *term)
{
	updated = false;
	newpdop=  TinyGPSPlus::parseDecimal(term);
}
void TinyGPSDOP::commit()
{
	hdop_ok=newhdop / 100.0;//for scale
	vdop_ok= newvdop / 100.0;//for scale
	pdop_ok=newpdop / 100.0;//for scale
    lastCommitTime = millis();
    valid = updated = true;
}

TinyGPSCustom::TinyGPSCustom(TinyGPSPlus &gps, const char *_sentenceName, int _termNumber)
{
   begin(gps, _sentenceName, _termNumber);
}

void TinyGPSCustom::begin(TinyGPSPlus &gps, const char *_sentenceName, int _termNumber)
{
   lastCommitTime = 0;
   updated = valid = false;
   sentenceName = _sentenceName;
   termNumber = _termNumber;
   memset(stagingBuffer, '\0', sizeof(stagingBuffer));
   memset(buffer, '\0', sizeof(buffer));

   // Insert this item into the GPS tree
   gps.insertCustom(this, _sentenceName, _termNumber);
}

void TinyGPSCustom::commit()
{
   strcpy(this->buffer, this->stagingBuffer);
   lastCommitTime = millis();
   valid = updated = true;
}

void TinyGPSCustom::set(const char *term)
{
   strncpy(this->stagingBuffer, term, sizeof(this->stagingBuffer));
}

void TinyGPSPlus::insertCustom(TinyGPSCustom *pElt, const char *sentenceName, int termNumber)
{
   TinyGPSCustom **ppelt;

   for (ppelt = &this->customElts; *ppelt != NULL; ppelt = &(*ppelt)->next)
   {
      int cmp = strcmp(sentenceName, (*ppelt)->sentenceName);
      if (cmp < 0 || (cmp == 0 && termNumber < (*ppelt)->termNumber))
         break;
   }

   pElt->next = *ppelt;
   *ppelt = pElt;
}

