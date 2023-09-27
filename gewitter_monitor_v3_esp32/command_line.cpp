#include "command_line.h"

//---------------------------------------------------------------
void CommandLine::decode_set_cmd(uint8_t pnt) {
  extern MyRTC rtc;
  extern Display tft;
  extern Adafruit_BMP280 bmp;
  extern int gl_hpa_offset, gl_detection_threshold, gl_flash_duration, gl_flash_on, gl_alarm_window, gl_display_timeout;
  extern uint32_t gl_alarm_level;
  uint8_t year=0, month=0, day=0, hour=0, minute=0;
  bool success = true;
  int x;

  switch (buffer[pnt]) {
    case 'a':     // set alarm level
    case 'A':
      Serial.println("Set alarm level");
      pnt += 1;
      x = get_int(&pnt);
      if ((x > 500) && (x < 50000)) {
        gl_alarm_level = x;
        calc_alarm_levels();
        NVS.setInt(nvs_alarm_level, gl_alarm_level);
        Serial.print("  alarm level: "); Serial.println(gl_alarm_level);
        Serial.println();
      } else {
        Serial.println("Alarm level out of range!");
      }
      break;

    case 'd':     // set date
    case 'D':
      rtc.set_date(buffer, pnt);
      break;

    case 'f':     // factor reset
    case 'F':
      Serial.println("Configuration reset to default.");
      NVS.setInt(nvs_hpa_offset, DEFAULT_HPA_OFFSET);
      NVS.setInt(nvs_detection_threshold, DEFAULT_DETECTION_THRESHOLD);
      NVS.setInt(nvs_flash_duration, DEFAULT_FLASH_DURATION);
      NVS.setInt(nvs_flash_on, DEFAULT_FLASH_ON);
      NVS.setInt(nvs_alarm_level, DEFAULT_ALARM_LEVEL);
      NVS.setInt(nvs_alarm_window, DEFAULT_ALARM_WINDOW);
      NVS.setInt(nvs_display_timeout, DEFAULT_DISPLAY_TIMEOUT);
      gl_hpa_offset = NVS.getInt(nvs_hpa_offset);
      gl_detection_threshold = NVS.getInt(nvs_detection_threshold);
      gl_flash_duration = NVS.getInt(nvs_flash_duration);
      gl_flash_on = NVS.getInt(nvs_flash_on);
      gl_alarm_level = NVS.getInt(nvs_alarm_level);
      calc_alarm_levels();
      gl_alarm_window = NVS.getInt(nvs_alarm_window);
      gl_display_timeout = NVS.getInt(nvs_display_timeout);
      tft.start();
      print_config();
      break;

    case 'h':     // hpa offset
    case 'H':
      Serial.println("Set hPa offset");
      pnt += 1;
      x = get_int(&pnt);
      if ((x >= 9000) && (x < 12000)) {
        gl_hpa_offset = x * 10 - bmp.readPressure();
        NVS.setInt(nvs_hpa_offset, gl_hpa_offset);
        Serial.print("  hpa offset: "); Serial.println(gl_hpa_offset);
        Serial.println();
      } else {
        Serial.println("Target hPa out of range!");
      }
      break;

    case 'o':     // display timeout
    case 'O':
      Serial.println("Set display timeout");
      pnt += 1;
      x = get_int(&pnt);
      if ((x >= 0) && (x < 60)) {
        gl_display_timeout = x;
        NVS.setInt(nvs_display_timeout, gl_display_timeout);
        Serial.print("  display timeout: "); Serial.println(gl_display_timeout);
        Serial.println();
      } else {
        Serial.println("Display timeout out of range!");
      }
      break;

    case 'r':     // detection threshold
    case 'R':
      Serial.println("Set detection threshold");
      pnt += 1;
      x = get_int(&pnt);
      if ((x >= 25) && (x < 1000)) {
        gl_detection_threshold = x;
        NVS.setInt(nvs_detection_threshold, gl_detection_threshold);
        Serial.print("  detection threshold ('r'): "); Serial.println(gl_detection_threshold);
        Serial.println();
      } else {
        Serial.println("Detection threshold out of range!");
      }
      break;

    case 's':     // flash status
    case 'S':
      Serial.println("Set flash status");
      pnt += 1;
      x = get_int(&pnt);
      if (x > 0) gl_flash_on = 1;
      else gl_flash_on = 0;
      NVS.setInt(nvs_flash_on, gl_flash_on);
      Serial.print("  flash status ('s'): "); if (gl_flash_on == 0) Serial.println("off"); else Serial.println("on");
      break;

    case 't':     // set time
    case 'T':
      rtc.set_time(buffer, pnt);
      break;

    case 'u':     // flash duration
    case 'U':
      Serial.println("Set flash duration");
      pnt += 1;
      x = get_int(&pnt);
      if ((x > 5) && (x < 1000)) {
        gl_flash_duration = x;
        NVS.setInt(nvs_flash_duration, gl_flash_duration);
        Serial.print("  flash duration: "); Serial.println(gl_flash_duration);
        Serial.println();
      } else {
        Serial.println("Flash duration out of range!");
      }
      break;

    case 'w':     // alarm window
    case 'W':
      Serial.println("Set alarm window");
      pnt += 1;
      x = get_int(&pnt);
      if ((x >= 2) && (x < 60)) {
        gl_alarm_window = x;
        NVS.setInt(nvs_alarm_window, gl_alarm_window);
        Serial.print("  alarm window: "); Serial.println(gl_alarm_window);
        Serial.println();
      } else {
        Serial.println("Alarm window out of range!");
      }
      break;

    default:
      Serial.print("Set command not recognized: ");
      Serial.println(buffer);
  }
}

//---------------------------------------------------------------
void CommandLine::decode_get_cmd(uint8_t pnt) {
  extern MyRTC rtc;
  
  char buf[16];

  switch (buffer[pnt]) {
    case 't':           // get time
    case 'T':
      rtc.get_time(buf);
      Serial.println(buf);
      break;

    case 'c':           // get configuration
    case 'C':
      print_config();
      break;

    default:
      Serial.print("Get command not recognized: ");
      Serial.println(buffer);
  }
}

//---------------------------------------------------------------
void CommandLine::decode_cmd(void) {
  uint8_t pnt = 0;

  while (((buffer[pnt] == ' ') || (buffer[pnt] == '\t ')) 
         && (pnt < BUFFER_MAX_LEN - 1)) pnt += 1;
  switch (buffer[pnt]) {
    case 's':
    case 'S':
      decode_set_cmd(pnt + 1);
      break;
    case 'g':
    case 'G':
      decode_get_cmd(pnt + 1);
      break;
    default:
      Serial.print("Command not recognized: ");
      Serial.println(buffer);
  }
}

//---------------------------------------------------------------
void CommandLine::print_config(void) {
  extern MyRTC rtc;
  extern int gl_hpa_offset, gl_detection_threshold, gl_flash_duration, gl_flash_on, gl_alarm_level, gl_alarm_window, gl_display_timeout;
  extern int gl_alarm_1, gl_alarm_2, gl_alarm_3, gl_alarm_4;
  char buf[16];

  Serial.println("Configuration");
  Serial.print("  hpa offset ('h'): "); Serial.println(gl_hpa_offset);
  Serial.print("  detection threshold ('r'): "); Serial.println(gl_detection_threshold);
  Serial.print("  flash duration ('u'): "); Serial.println(gl_flash_duration);
  Serial.print("  flash status ('s'): "); if (gl_flash_on == 0) Serial.println("off"); else Serial.println("on");
  Serial.print("  alarm level ('a'): "); Serial.print(gl_alarm_level);
  Serial.print("  (1: "); Serial.print(gl_alarm_1); Serial.print("  2: "); Serial.print(gl_alarm_2); 
  Serial.print("  3: "); Serial.print(gl_alarm_3); Serial.print("  4: "); Serial.print(gl_alarm_4); Serial.println(")");
  Serial.print("  alarm window ('w'): "); Serial.println(gl_alarm_window); 
  Serial.print("  display timeout ('o'): "); Serial.println(gl_display_timeout);
  Serial.print("  date ('d'): "); 
  rtc.get_date(buf); Serial.println(buf);
  Serial.print("  time ('t'): ");
  rtc.get_time(buf); Serial.println(buf);
  Serial.println();
}

//---------------------------------------------------------------
void CommandLine::check_ser_input(void) {
  char c;

  while (Serial.available() > 0) {
    c = Serial.read();
    if (c == '\n') {
      buffer[buffer_pnt] = '\n';
      decode_cmd();
      buffer_pnt = 0;
    } else {
      if (buffer_pnt < BUFFER_MAX_LEN - 1) {
        buffer[buffer_pnt] = c;
        buffer_pnt += 1;
      }
    }
  }
}

//-------------------------------------------------------------------------
// get_int
// Extracts an integer from the input buffer. 
// Moves the pointer to the next position after the integer
int32_t CommandLine::get_int(uint8_t *pnt) {
  int32_t result = 0;
  bool negative_flag = false, done = false, valid = false;

  while (buffer[*pnt] == ' ') *pnt += 1;     // skip leading blanks
  
  while (!done) {
    if (buffer[*pnt] == '+') {
      negative_flag = false;
    } else if (buffer[*pnt] == '-') {
      negative_flag = true;
    } else if ((buffer[*pnt] >= '0') && (buffer[*pnt] <= '9')) {
      result = result * 10;
      result += buffer[*pnt] - '0';
      valid = true;
    } else {
      done = true;
    }
    if (buffer[*pnt] != '\0')      // don't progress beyond end of string
      *pnt += 1;
  }
  if (negative_flag) {
    result = -result;
  }
  return result;
}
