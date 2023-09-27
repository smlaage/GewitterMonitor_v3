#ifndef __MENU__
#define __MENU__

#include <Arduino.h>
#include "gewitter_monitor.h"
#include "display.h"
#include "my_rtc.h"

class Menu {
  private:
    bool is_on = false;
    int item = 8;
    char labels[9][20] = {
      "Hour",
      "Minute",
      "Detector Threshold",
      "Flash Duration",
      "Display Timeout",
      "Alarm Level",
      "Alarm Window",
      "hPa",
      "Exit"
    };
    long hpa;
    int hour, minute, detection_threshold, flash_duration, display_timeout, alarm_level, alarm_window;
    void reset_item(int item, char buf[]);
    void show_confirmation(void);
   
  public:
    void init(void);
    void process_key(uint8_t key);
    bool is_active(void);
};

#endif