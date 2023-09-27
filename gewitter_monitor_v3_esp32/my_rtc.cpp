#include "my_rtc.h"
#include "display.h"

//---------------------------------------------------------------
void MyRTC::get_time(char buf[]) {
  DateTime now = RTClib::now();
  
  uint8_t hour = now.hour();
  buf[0] = '0' + hour / 10;
  buf[1] = '0' + hour % 10; 
  buf[2] = ':';
   
  uint8_t minute = now.minute();
  buf[3] = '0' + minute / 10;
  buf[4] = '0' + minute % 10;
  buf[5] = ':';
  
  uint8_t second = now.second();
  buf[6] = '0' + second / 10;
  buf[7] = '0' + second % 10;
  buf[8] = '\0';
}

//---------------------------------------------------------------
void MyRTC::get_date(char buf[]) {
  DateTime now = RTClib::now();
  
  uint8_t day = now.day();
  buf[0] = '0' + day / 10;
  buf[1] = '0' + day % 10; 
  buf[2] = '/';
   
  uint8_t month = now.month();
  buf[3] = '0' + month / 10;
  buf[4] = '0' + month % 10;
  buf[5] = '/';
  
  uint8_t year = now.year();
  year = year / 10;
  buf[6] = '0' + year / 10;
  buf[7] = '0' + year % 10;
  buf[8] = '\0';
}

//----------------------------------------------------------------------------------------
void MyRTC::set_date(char input_buffer[], uint8_t pnt) {
	uint8_t year=0, month=0, day=0;
	bool success = true;

  if ((input_buffer[pnt + 1] >= '0') && (input_buffer[pnt + 1] <= '9')) {
    day += (input_buffer[pnt + 1] - '0') * 10;
    if ((input_buffer[pnt + 2] >= '0') && (input_buffer[pnt + 2] <= '9')) {
      day += input_buffer[pnt + 2] - '0';
      if ((day >= 1) && (day <= 31)) {
        ;
      } else success = false;
    } else success = false;
  }
      
  if (success) {
    if ((input_buffer[pnt + 3] == '-') || (input_buffer[pnt + 3] == '.') || (input_buffer[pnt + 3] == '/')) {
      if ((input_buffer[pnt + 4] >= '0') && (input_buffer[pnt + 4] <= '9')) {
        month += (input_buffer[pnt + 4] - '0') * 10;
        if ((input_buffer[pnt + 5] >= '0') && (input_buffer[pnt + 5] <= '9')) {
          month += input_buffer[pnt + 5] - '0';
          if ((month >= 1) && (month <= 12)) {
            ;
          } else success = false;
        } else success = false;
      } else success = false;
    } else success = false;
  }

  if (success) {
    if ((input_buffer[pnt + 6] == '-') || (input_buffer[pnt + 6] == '.') || (input_buffer[pnt + 3] == '/')) {
      if ((input_buffer[pnt + 7] >= '0') && (input_buffer[pnt + 7] <= '9')) {
        year += (input_buffer[pnt + 7] - '0') * 10;
        if ((input_buffer[pnt + 8] >= '0') && (input_buffer[pnt + 8] <= '9')) {
          year += input_buffer[pnt + 8] - '0';
          if (year >= 20) {
            ;
          } else success = false;
        } else success = false;
      } else success = false;
    } else success = false;
  }  
   
  if (success) {
    ds_rtc.setDate(day);
    ds_rtc.setMonth(month);
    ds_rtc.setYear(year);
    Serial.println("Date set!");
    Serial.print("day: "); Serial.print(day);
    Serial.print(" month: "); Serial.print(month);
    Serial.print(" year: "); Serial.println(year);
  } else {
    Serial.print("Set date command not recognized: ");
    Serial.println(input_buffer + 2);
    Serial.println("Required format: sd<year>-<month>-<day>, e.g. 'sd29-07-23'");
	}
}

//----------------------------------------------------------------------------------------
void MyRTC::set_hour_minute(uint8_t hour, uint8_t minute) {
  extern uint8_t second;
  extern portMUX_TYPE timer_1_Mux;

  ds_rtc.setSecond(0);
  portENTER_CRITICAL(&timer_1_Mux);
  second = 0;
  portEXIT_CRITICAL(&timer_1_Mux); 
  ds_rtc.setHour(hour);
  ds_rtc.setMinute(minute);
  Serial.print("hour: "); Serial.print(hour);
  Serial.print(" minute: "); Serial.print(minute);
  Serial.println(" Time set!");
}

//----------------------------------------------------------------------------------------
void MyRTC::set_time(char input_buffer[], uint8_t pnt) {
  extern Display tft;
  extern uint8_t second;
  extern portMUX_TYPE timer_1_Mux;
	uint8_t hour=0, minute=0;
	bool success = true;
	
	if ((input_buffer[pnt + 1] >= '0') && (input_buffer[pnt + 1] <= '9'))
      hour += (input_buffer[pnt + 1] - '0') * 10;
    else success = false;
  if (success && ((input_buffer[pnt + 2] >= '0') && (input_buffer[pnt + 2] <= '9')))
    hour += input_buffer[pnt + 2] - '0';
  else success = false;
  if (hour > 23) success = false;
      
  if (success && (input_buffer[pnt + 3] != ':')) success = false;

  if ((input_buffer[pnt + 4] >= '0') && (input_buffer[pnt + 4] <= '9'))
    minute += (input_buffer[pnt + 4] - '0') * 10;
  else success = false;
  if (success && ((input_buffer[pnt + 5] >= '0') && (input_buffer[pnt + 5] <= '9')))
    minute += input_buffer[pnt + 5] - '0';
  else success = false;
  if (minute > 59) success = false;
    
  if (success) {
    ds_rtc.setSecond(0);
    portENTER_CRITICAL(&timer_1_Mux);
    second = 0;
    portEXIT_CRITICAL(&timer_1_Mux); 
    ds_rtc.setHour(hour);
    ds_rtc.setMinute(minute);
    Serial.println("Time set!");
    Serial.print("hour: "); Serial.print(hour);
    Serial.print(" minute: "); Serial.print(minute);
    tft.refresh_minute(minute);
  } else {
    Serial.print("Set time command not recognized: ");
    Serial.println(input_buffer + 2);
    Serial.println("Required format: st<hour>-<minute>, e.g. 'st16:42'");
  }
}

//----------------------------------------------------------------------------------------
uint8_t MyRTC::get_hour(void) {
	return ds_rtc.getHour(mode_flag, pm_flag);
}

//----------------------------------------------------------------------------------------
uint8_t MyRTC::get_minute(void) {
	return ds_rtc.getMinute();
}

//----------------------------------------------------------------------------------------
uint8_t MyRTC::get_second(void) {
	return ds_rtc.getSecond();
}


  