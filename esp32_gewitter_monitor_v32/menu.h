#ifndef __MENU__
#define __MENU__

#include <Arduino.h>
#include "gewitter_monitor.h"
#include "display.h"
#include "my_rtc.h"
#include "keyboard.h"

class Menu {
  private:
    bool is_on = false;
    int item = 10;
    char labels[11][20] = {
      "Hour",                   //  0
      "Minute",                 //  1
      "Detector Threshold",     //  2
      "Flash Duration",         //  3
      "Display Timeout",        //  4
      "Alarm Level",            //  5
      "Alarm Window",           //  6
      "Scale Minimum",          //  7
      "hPa",                    //  8
      "Voltage Adjust",         //  9
      "Exit"                    // 10
    };
    long hpa;
    int hour, minute, detection_threshold, flash_duration, display_timeout, alarm_level, alarm_window, scale_min;
    void reset_item(int item, char buf[]);
    void show_confirmation(void);
    void show_voltage(void);
   
  public:
    void init(void);
    void process_key(uint8_t key);
    bool is_active(void);
};

#endif