#ifndef __MY_RTC__ 
#define __MY_RTC__

#include <DS3231.h>

class MyRTC {
  private:
    DS3231 ds_rtc;
    bool mode_flag, pm_flag;
   
  public:
    void get_time(char buf[]);
    void get_date(char buf[]);
    void set_date(char input_buffer[], uint8_t pnt);
    void set_time(char input_buffer[], uint8_t pnt);
    void set_hour_minute(uint8_t hour, uint8_t minute); 
    uint8_t get_hour(void);
    uint8_t get_minute(void);
    uint8_t get_second(void);
};

#endif
