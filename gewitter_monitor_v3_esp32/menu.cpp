#include "menu.h"

//--------------------------------------------------------------------------------
void Menu::init(void) {
  extern Display tft;
  extern MyRTC rtc;
  extern Adafruit_BMP280 bmp;
  extern int gl_detection_threshold;
  extern int gl_flash_duration;
  extern int gl_display_timeout;
  extern int gl_alarm_level; // red, e.g. 5000
  extern int gl_alarm_window;
  extern int gl_hpa_offset;
  extern bool bmp_found;
  int i;
  char buf[10];

  hour = rtc.get_hour();
  minute = rtc.get_minute();
  detection_threshold = gl_detection_threshold;
  flash_duration = gl_flash_duration;
  display_timeout = gl_display_timeout;
  alarm_level = gl_alarm_level;
  alarm_window = gl_alarm_window;
  if (bmp_found) {
    hpa = (bmp.readPressure() + gl_hpa_offset) / 100;
  } else {
    hpa = 0;
  }
  
  is_on = true;
  tft.enter_menu();
  itoa(hour, buf, 10);
  tft.draw_menu_item(0, labels[0], buf);
  itoa(minute, buf, 10);
  tft.draw_menu_item(1, labels[1], buf);
  itoa(detection_threshold, buf, 10);
  tft.draw_menu_item(2, labels[2], buf);
  itoa(flash_duration, buf, 10);
  tft.draw_menu_item(3, labels[3], buf);
  itoa(display_timeout, buf, 10);
  tft.draw_menu_item(4, labels[4], buf);
  itoa(alarm_level, buf, 10);
  tft.draw_menu_item(5, labels[5], buf);
  itoa(alarm_window, buf, 10);
  tft.draw_menu_item(6, labels[6], buf);
  if (bmp_found) {
    itoa(hpa, buf, 10);
  } else {
    strcpy(buf, "----");
  }
  tft.draw_menu_item(7, labels[7], buf);
  tft.draw_menu_item(8, labels[8], " ");
  tft.set_menu_item(item);
}

//--------------------------------------------------------------------------------
void Menu::process_key(uint8_t key) {
  extern Display tft;
  extern MyRTC rtc;
  extern Adafruit_BMP280 bmp;
  extern int gl_detection_threshold;
  extern int gl_flash_duration;
  extern int gl_display_timeout;
  extern int gl_alarm_level; // red, e.g. 5000
  extern int gl_alarm_window;
  extern int gl_hpa_offset;
  extern bool bmp_found;
  char buf[10];
  uint8_t pnt = 0;

  switch (key) {

    case 1:       // left key
      switch (item) {
        case 0: 
          if (hour <= 0) hour = 23;
          else hour -= 1;
          itoa(hour, buf, 10);
          tft.update_menu_item(item, buf);
          break;
        case 1:
          if (minute <= 0) minute = 59;
          else minute -= 1;
          itoa(minute, buf, 10);
          tft.update_menu_item(item, buf);
          break;
        case 2:
          if (detection_threshold <= 20) detection_threshold = 500;
          else detection_threshold -= 20;
          itoa(detection_threshold, buf, 10);
          tft.update_menu_item(item, buf);
          break;
        case 3:
          if (flash_duration <= 10) flash_duration = 500;
          else flash_duration -= 10;
          itoa(flash_duration, buf, 10);
          tft.update_menu_item(item, buf);
          break;
        case 4:
          if (display_timeout <= 2) display_timeout = 30;
          else display_timeout -= 1;
          itoa(display_timeout, buf, 10);
          tft.update_menu_item(item, buf);
          break;
        case 5:
          if (alarm_level <= 1000) alarm_level = 25000;
          else alarm_level -= 1000;
          itoa(alarm_level, buf, 10);
          tft.update_menu_item(item, buf);
          break;
        case 6:
          if (alarm_window <= 2) alarm_window = 30;
          else alarm_window -= 1;
          itoa(alarm_window, buf, 10);
          tft.update_menu_item(item, buf);
          break;          
        case 7:
          if (bmp_found) {
            if (hpa <= 900) hpa = 1100;
            else hpa -= 1;
            ltoa(hpa, buf, 10);
            tft.update_menu_item(item, buf);
          }
          break;
        default:
          ;
      }
      break;

    case 2:       // up key
      reset_item(item, buf);
      tft.clear_menu_item(item, buf);
      if (item == 0) item = 8;
      else item -= 1;
      tft.set_menu_item(item);
      break;

    case 4:       // right key
      switch (item) {
        case 0: 
          if (hour >= 23) hour = 0;
          else hour += 1;
          itoa(hour, buf, 10);
          tft.update_menu_item(item, buf);
          break;
        case 1:
          if (minute >= 59) minute = 0;
          else minute += 1;
          itoa(minute, buf, 10);
          tft.update_menu_item(item, buf);
          break;
        case 2:
          if (detection_threshold >= 500) detection_threshold = 20;
          else detection_threshold += 20;
          itoa(detection_threshold, buf, 10);
          tft.update_menu_item(item, buf);
          break;
        case 3:
          if (flash_duration >= 500) flash_duration = 10;
          else flash_duration += 10;
          itoa(flash_duration, buf, 10);
          tft.update_menu_item(item, buf);
          break;
        case 4:
          if (display_timeout >= 30) display_timeout = 2;
          else display_timeout += 1;
          itoa(display_timeout, buf, 10);
          tft.update_menu_item(item, buf);
          break;
        case 5:
          if (alarm_level >= 25000) alarm_level = 1000;
          else alarm_level += 1000;
          itoa(alarm_level, buf, 10);
          tft.update_menu_item(item, buf);
          break;
        case 6:
          if (alarm_window >= 30) alarm_window = 2;
          else alarm_window += 1;
          itoa(alarm_window, buf, 10);
          tft.update_menu_item(item, buf);
          break;          
        case 7:
          if (bmp_found) {
            if (hpa >= 1100) hpa = 900;
            else hpa += 1;
            ltoa(hpa, buf, 10);
            tft.update_menu_item(item, buf);
          }
          break;
        default:
          ;
      }
      break;
    
    case 8:       // down key
      reset_item(item, buf);
      tft.clear_menu_item(item, buf);
      if (item == 8) item = 0;
      else item += 1;
      tft.set_menu_item(item);
      break;

    case 16:      // center key
      switch (item) {
        case 0:
          break;

        case 1:
          rtc.set_hour_minute(hour, minute);
          break;

        case 2:
          gl_detection_threshold = detection_threshold;
          NVS.setInt(nvs_detection_threshold, gl_detection_threshold);
          Serial.print("Detection threshold: "); Serial.println(detection_threshold);
          show_confirmation();
          break;

        case 3:
          gl_flash_duration = flash_duration;
          NVS.setInt(nvs_flash_duration, gl_flash_duration);
          Serial.print("Flash duration: "); Serial.println(flash_duration);
          show_confirmation();
          break;

        case 4:
          gl_display_timeout = display_timeout;
          NVS.setInt(nvs_display_timeout, gl_display_timeout);
          Serial.print("Display timeout: "); Serial.println(display_timeout);
          show_confirmation();
          break;

        case 5:
          gl_alarm_level = alarm_level;
          NVS.setInt(nvs_alarm_level, gl_alarm_level);
          Serial.print("Alarm level: "); Serial.println(alarm_level);
          show_confirmation();
          break;

        case 6:
          gl_alarm_window = alarm_window;
          NVS.setInt(nvs_alarm_window, gl_alarm_window);
          Serial.print("Alarm window: "); Serial.println(alarm_window);
          show_confirmation();
          break;

        case 7:
          if (bmp_found) {
            gl_hpa_offset = hpa * 100 - bmp.readPressure();
            NVS.setInt(nvs_hpa_offset, gl_hpa_offset);
            show_confirmation();
          }
          break;

        case 8:
          is_on = false;
          tft.exit_menu();
          break;

        default:
          Serial.print("menu -> process_key -> center key: item error, "); Serial.println(item);
      };
      break;
      
    default:
      ;
  }
}

//--------------------------------------------------------------------------------
bool Menu::is_active(void) {
  return is_on;
}

//--------------------------------------------------------------------------------
void Menu::reset_item(int item, char buf[]) {
  extern MyRTC rtc;
  extern Adafruit_BMP280 bmp;
  extern int gl_detection_threshold;
  extern int gl_flash_duration;
  extern int gl_display_timeout;
  extern int gl_alarm_level; // red, e.g. 5000
  extern int gl_alarm_window;
  extern int gl_hpa_offset;
  extern bool bmp_found;

  switch (item) {
    case 0: 
      itoa(hour, buf, 10); 
      break;

    case 1: 
      minute =  rtc.get_minute(); itoa(minute, buf, 10); 
      break;

    case 2: 
      detection_threshold = gl_detection_threshold; itoa(detection_threshold, buf, 10); 
      break;

    case 3: 
      flash_duration = gl_flash_duration; itoa(flash_duration, buf, 10); 
      break;

    case 4: 
      display_timeout = gl_display_timeout; itoa(display_timeout, buf, 10); 
      break;

    case 5: 
      alarm_level = gl_alarm_level; itoa(alarm_level, buf, 10); 
      break;

    case 6: 
      alarm_window = gl_alarm_window; itoa(alarm_window, buf, 10);
      break;

    case 7: 
      if (bmp_found) {
        hpa = (bmp.readPressure() + gl_hpa_offset) / 100;
      } else {
        hpa = 0;
      };
      if (hpa > 0) ltoa(hpa, buf, 10);
      else strcpy(buf, "----");
      break;

    case 8:
      buf[0] = '\0';
      break;

    default:
      Serial.print("menu -> reset_item: item error, "); Serial.println(item);
      buf[0] = '\0';
  }
}

//--------------------------------------------------------------------------------
void Menu::show_confirmation(void) {
  digitalWrite(PIN_FLASH_LED_LEFT, HIGH);
  digitalWrite(PIN_FLASH_LED_RIGHT, HIGH);
  delay(100);
  digitalWrite(PIN_FLASH_LED_LEFT, LOW);
  digitalWrite(PIN_FLASH_LED_RIGHT, LOW);
}
