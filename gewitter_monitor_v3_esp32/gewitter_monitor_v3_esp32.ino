/*
Gewitter Monitor V3
Target device: ESP32

SLW 2023-09-27
*/

#include "gewitter_monitor.h"
#include "display.h"
#include "command_line.h"
#include "my_rtc.h"
#include "keyboard.h"
#include "menu.h"
#include "util.h"

// Timer interrupts
#define TIMER0_INTERVAL_MS  1

// Global parameters
int gl_hpa_offset;
int gl_detection_threshold;
int gl_flash_duration;
int gl_flash_on;
int gl_alarm_level; // red, e.g. 5000
int gl_alarm_1,     // display on, e.g. 1000
    gl_alarm_2,     // green_yellow, e.g. 2000
    gl_alarm_3,     // yellow, e.g. 3000
    gl_alarm_4;     // yellow_red, e.g. 4000
int gl_alarm_window;
int gl_display_timeout;
int gl_read_flash_data = true;
int gl_scale_min;
int gl_network_status;

// global variables
hw_timer_t *timer_1 = NULL;   // triggers every second, updates x-axis and time display
portMUX_TYPE timer_1_Mux = portMUX_INITIALIZER_UNLOCKED;
hw_timer_t *timer_3 = NULL;   // triggers with 10 kHz, captures ADC values
portMUX_TYPE timer_3_Mux = portMUX_INITIALIZER_UNLOCKED;
hw_timer_t *timer_2 = NULL;   // keyboard timer
portMUX_TYPE timer_2_Mux = portMUX_INITIALIZER_UNLOCKED;
Display tft;
Menu menu;
Adafruit_BMP280 bmp;
MyRTC rtc;
CommandLine cl;
// Variables managed by timer interrupt routines
volatile uint16_t left_max = 0, right_max = 0;
#if DEMO_MODE
  volatile uint16_t left_cnt = 55, right_cnt = 71;
  volatile uint32_t left_sum = 3027, right_sum = 4652;
#else
  volatile uint16_t left_cnt = 0, right_cnt = 0;
  volatile uint32_t left_sum = 0, right_sum = 0;
#endif
volatile bool left_channel = true;
volatile uint8_t job_flags = 0b00000000;
volatile uint8_t second = 0;
#if DEMO_MODE
  volatile uint32_t data_left[DATA_MAX] = {
      0,    0,    0,    0,    0,    0,    0,    0,    0,  259, 
      0,    0,    0,   32,    0,   91,    0,    0,    0,    0, 
      0,    0,    0,    0,    0,   31,    0,    0,    0,  180, 
      0,    0,    0,    0,    0,    0,    0,    0,    0,    0, 
      0,    0,    0,    0,    0,    0,    0,  329,    0,    0, 
      0,   10,    7,    5,   23,   12,   41,   31,   38,   57, 
      52,    8,   50,   18,   24,   48,   42,   88,   28,   30, 
      25,   65,    4,  124,   87,   24,  104,  130,  112,   29, 
      4,  147,    1,   95,   90,   64,  147,   16,   56,  316, 
    201,   35,   76,   47,  215,  166,  117,   12,  137,  232, 
    322,  322,  101,    6,  182,  292,   94,  294,  130,   24, 
    100,  365,  209,  209,  212,   71,  335,   30,  297,    2, 
    155,  379,  116,  304,  240,  444,  266,  593,  475,  884, 
    883,  755,  954,  640,  981, 1038, 1320,  824,  415,  850, 
    839,  822,  970, 1670, 1790,  623, 1452, 1062, 1209,  577, 
    1733, 1752, 1454,  765, 2202, 2164, 2260,  855,  852, 2468, 
    876, 2271, 2372, 1045, 2596,  828, 2015, 3580, 1021,  931, 
    1641, 2293, 3255, 2585, 3202, 3075, 2868, 1201, 3461, 3113
  };
  volatile uint32_t data_right[DATA_MAX] = {
      0,    0,    0,    0,    0,    0,    0,    0,    0,  116, 
      0,    0,    0,   97,    0,   88,    0,    0,    0,    0, 
      0,    0,    0,    0,    0,   72,    0,    0,    0,  147, 
      0,    0,    0,    0,    0,    0,    0,    0,    0,    0, 
      0,    0,    0,    0,    0,    0,    0,   93,    0,    0, 
      0,   10,    6,    3,   22,   12,   29,   24,   51,   36, 
      52,   10,   33,   21,   18,   64,   29,   78,   18,   27, 
      22,   89,    4,   83,   78,   23,   78,  102,  107,   41, 
      4,  147,    1,   75,  109,   53,  116,   13,   54,  202, 
    165,   47,   53,   42,  224,  219,   82,   11,   92,  242, 
    145,  145,  151,    8,  155,  212,   77,  251,  111,   18, 
    108,  243,  145,  234,  259,   74,  322,   32,  264,    3, 
    159,  327,   83,  340,  151,  420,  176,  528,  424,  450, 
    657,  539,  663,  704,  781,  985,  935,  614,  343,  800, 
    690,  474,  550, 1191,  949,  452, 1217,  891, 1092,  442, 
    1100,  959, 1118,  833, 1526, 2298, 1883,  939,  823, 2555, 
    1118, 2611, 2159, 1302, 2413, 1115, 2101, 2435, 1732, 1507, 
    2032, 2353, 3666, 2376, 3096, 3405, 3222, 2180, 3690, 3282
  };
  volatile uint32_t data_hpa[DATA_MAX] = {
    101223, 101218, 101223, 101219, 101213, 101225, 101226, 101221, 101207, 101211, 
    101209, 101206, 101216, 101212, 101225, 101216, 101222, 101212, 101209, 101212, 
    101211, 101206, 101221, 101208, 101211, 101207, 101223, 101209, 101218, 101208, 
    101219, 101208, 101218, 101211, 101212, 101209, 101207, 101219, 101223, 101211, 
    101221, 101209, 101223, 101209, 101218, 101215, 101209, 101223, 101217, 101210, 
    101211, 101218, 101209, 101221, 101208, 101211, 101208, 101212, 101203, 101215, 
    101212, 101211, 101200, 101202, 101209, 101191, 101200, 101191, 101190, 101190, 
    101201, 101194, 101200, 101201, 101191, 101192, 101200, 101191, 101197, 101183, 
    101184, 101182, 101178, 101179, 101186, 101178, 101184, 101169, 101181, 101184, 
    101186, 101165, 101170, 101173, 101165, 101162, 101164, 101170, 101170, 101167, 
    101170, 101164, 101158, 101159, 101169, 101168, 101162, 101155, 101155, 101152, 
    101157, 101153, 101162, 101159, 101161, 101143, 101143, 101156, 101152, 101139, 
    101147, 101140, 101134, 101139, 101143, 101143, 101143, 101146, 101134, 101133, 
    101143, 101138, 101127, 101138, 101140, 101121, 101125, 101121, 101135, 101131, 
    101128, 101132, 101123, 101113, 101130, 101129, 101110, 101113, 101112, 101114, 
    101126, 101110, 101120, 101117, 101105, 101116, 101124, 101115, 101107, 101119, 
    101113, 101120, 101116, 101109, 101117, 101121, 101113, 101108, 101103, 101109, 
    101103, 101118, 101119, 101118, 101105, 101112, 101107, 101119, 101102, 101114, 
  };
#else
  volatile uint32_t data_left[DATA_MAX], data_right[DATA_MAX], data_hpa[DATA_MAX];
#endif
volatile uint16_t data_pnt = 0;
// Other
bool bmp_found = false;
uint32_t hpa;
bool bt_center_status = LOW;
int y_pos = 0;
int left_flash_cnt = 0, right_flash_cnt = 0;
bool refresh_okay = false;

// timer service handler 1 --------------------------------------------------------------------------------------
void IRAM_ATTR timer_1_isr(void) {
  portENTER_CRITICAL_ISR(&timer_1_Mux);
  job_flags |= (1 << JF_UPDATE_SECOND);
  second += 1;
  if (second >= 60) {
    second = 0;
    job_flags |= (1 << JF_UPDATE_MINUTE);
    data_left[data_pnt] = left_sum;
    data_right[data_pnt] = right_sum;
    data_hpa[data_pnt] = hpa;
    data_pnt += 1;
    if (data_pnt >= DATA_MAX) data_pnt = 0;
    portENTER_CRITICAL(&timer_3_Mux);
#if DEMODATA
  left_cnt = 45; right_cnt = 71;
  left_sum = 3027; right_sum = 4652;
#else    
    left_cnt = 0;
    right_cnt = 0;
    left_sum = 0;
    right_sum = 0;
#endif
    portEXIT_CRITICAL(&timer_3_Mux);
  }
  portEXIT_CRITICAL_ISR(&timer_1_Mux);
}

// timer service handler 3 --------------------------------------------------------------------------------------
void IRAM_ATTR timer_3_isr(void) {
  static uint16_t left_old = 0, right_old = 0;
  static bool left_detect = false, right_detect = false;
  uint16_t left_adc, right_adc;

  if (!gl_read_flash_data) return;

  portENTER_CRITICAL_ISR(&timer_3_Mux);

  if (left_channel) {
    left_adc = analogRead(PIN_ADC_LEFT);
    // search for peaks indicating a flash
    if (left_adc > left_old + gl_detection_threshold) {
      left_detect = true;
      left_max = left_adc;
    }
    // if flash is ongoing, find the maximum
    if (left_detect) {
      if (left_adc > left_old) {
        left_max = left_adc;
      } else {
        job_flags |= (1 << JF_FLASH_LEFT);
        left_cnt += 1;
        left_sum += (left_max - gl_detection_threshold) >> 2;
        // left_sum += (left_max - gl_detection_threshold);
        left_detect = false;
      }
    } 
    left_old = left_adc;
    left_channel = false;
  
  } else {
    right_adc = analogRead(PIN_ADC_RIGHT);
    // search for peaks indicating a flash
    if (right_adc > right_old + gl_detection_threshold) {
      right_detect = true;
      right_max = right_adc;
    }
    // if flash is ongoing, find the maximum
    if (right_detect) {
      if (right_adc > right_old) {
        right_max = right_adc;
      } else {
        job_flags |= (1 << JF_FLASH_RIGHT);
        right_cnt += 1;
        right_sum += (right_max - gl_detection_threshold) >> 2;
        // right_sum += (right_max - gl_detection_threshold);
        right_detect = false;
      }
    }
    right_old = right_adc;
    left_channel = true;
  }

  portEXIT_CRITICAL_ISR(&timer_3_Mux);
}

//---------------------------------------------------------------
void setup() {
  Serial.begin(115200);

  pinMode(PIN_BT_LEFT, INPUT_PULLUP);
  pinMode(PIN_BT_UP, INPUT_PULLUP);
  pinMode(PIN_BT_RIGHT, INPUT_PULLUP);
  pinMode(PIN_BT_DOWN, INPUT_PULLUP);
  pinMode(PIN_BT_CENTER, INPUT_PULLUP);
  pinMode(PIN_LCD_PWR, OUTPUT);
  pinMode(PIN_FLASH_LED_LEFT, OUTPUT);
  pinMode(PIN_FLASH_LED_RIGHT, OUTPUT);
  pinMode(PIN_LED_GREEN, OUTPUT);
  pinMode(PIN_LED_YELLOW, OUTPUT);
  pinMode(PIN_LED_RED, OUTPUT);
  digitalWrite(PIN_LCD_PWR, LOW);

  Serial.println("Gewitter Monitor v3");
  Serial.println("SLW 2023-08 https://laagewitt.de");

  NVS.begin();
  gl_hpa_offset = NVS.getInt(nvs_hpa_offset);
  gl_detection_threshold = NVS.getInt(nvs_detection_threshold);
  gl_flash_duration = NVS.getInt(nvs_flash_duration);
  gl_flash_on = NVS.getInt(nvs_flash_on);
  gl_alarm_level = NVS.getInt(nvs_alarm_level);
  calc_alarm_levels();
  gl_alarm_window = NVS.getInt(nvs_alarm_window);
  gl_display_timeout = NVS.getInt(nvs_display_timeout);
  gl_scale_min = NVS.getInt(nvs_scale_min);
  if (gl_scale_min < 400) {
    gl_scale_min = DEFAULT_SCALE_MIN;
    NVS.setInt(nvs_scale_min, gl_scale_min);
  }
  gl_network_status = NVS.getInt(nvs_network_status);

  Wire.begin();
  Wire.setClock(400000);
  
  // Prepare ADC
  analogSetWidth(10);     // set to 10 bit, values 0 ... 1023
  analogReadResolution(10); 
  adcAttachPin(PIN_ADC_LEFT);
  adcAttachPin(PIN_ADC_RIGHT);

  // Clear buffer
  #if !DEMO_MODE
  for (int i = 0; i < DATA_MAX; ++i) {
    data_left[i] = 0;
    data_right[i] = 0;
    data_hpa[i] = 0;
  }
  #endif

  // Initiate tft
  tft.init();

  // Flash LEDs
  digitalWrite(PIN_FLASH_LED_LEFT, HIGH);
  delay(200);
  digitalWrite(PIN_FLASH_LED_LEFT, LOW);  
  digitalWrite(PIN_FLASH_LED_RIGHT, HIGH);
  delay(200);
  digitalWrite(PIN_FLASH_LED_RIGHT, LOW);
  digitalWrite(PIN_LED_GREEN, HIGH);
  delay(200);
  digitalWrite(PIN_LED_GREEN, LOW);
  digitalWrite(PIN_LED_YELLOW, HIGH);
  delay(200);
  digitalWrite(PIN_LED_YELLOW, LOW);
  digitalWrite(PIN_LED_RED, HIGH);
  delay(200);
  digitalWrite(PIN_LED_RED, LOW);

  // Start RTC
  second = rtc.get_second();

  // Start BMP
  bmp_found = bmp.begin(0x76);

  // Start timer 1
  // Serial.println("Starting timer interrupts on timer 1");
  timer_1 = timerBegin(1, 160, true);          // pre-scaler 160 gives 1 tick per 2 usec
  timerAttachInterrupt(timer_1, &timer_1_isr, true);
  timerAlarmWrite(timer_1, 500010, true);     // 500000 usec -> 1 event per second
  timerAlarmEnable(timer_1);

  // Starting timer 3
  // Serial.println("Starting timer interrupts on timer 3");
  timer_3 = timerBegin(3, 80, true);          // pre-scaler 80 gives 1 tick per 1 usec
  timerAttachInterrupt(timer_3, &timer_3_isr, true);
  timerAlarmWrite(timer_3, 100, true);     // 100 usec -> 0.1 ms -> 10000 Hz
  timerAlarmEnable(timer_3);

  // Starting timer 2 - keyboard
  // Serial.println("Starting timer interrupts on timer 2");
  timer_2 = timerBegin(2, 160, true);          // pre-scaler 160 gives 2 tick per 1 usec
  timerAttachInterrupt(timer_2, &check_keyboard_isr, true);
  timerAlarmWrite(timer_2, 20000, true);     // 25000 usec -> 20 events per second
  timerAlarmEnable(timer_2);

  update_alarm();

  tft.restart();
}

//---------------------------------------------------------------
void loop() {
  bool bt_center;
  char buf[24];
  uint8_t key;

  if (left_flash_cnt > 0) {
    left_flash_cnt -= 1;
    if (left_flash_cnt == 0)
      digitalWrite(PIN_FLASH_LED_LEFT, LOW);
  }

  if (right_flash_cnt > 0) {
    right_flash_cnt -= 1;
    if (right_flash_cnt == 0)
      digitalWrite(PIN_FLASH_LED_RIGHT, LOW);
  }

  if (job_flags & (1 << JF_UPDATE_SECOND)) {
    portENTER_CRITICAL(&timer_1_Mux);
    job_flags &= ~(1 << JF_UPDATE_SECOND);
    portEXIT_CRITICAL(&timer_1_Mux); 
    if (!menu.is_active()) tft.refresh_second();
    if (bmp_found) {
      if (second == 30) {
        #if DEMO_MODE
          hpa = 101114;
        #else
          hpa = (uint32_t) (bmp.readPressure() + gl_hpa_offset);
        #endif        
        if (!menu.is_active()) tft.refresh_hpa(hpa);
      }
    }
    if (second == 59) refresh_okay = true;
  }

  if (job_flags & (1 << JF_UPDATE_MINUTE)) {
    portENTER_CRITICAL(&timer_1_Mux);
    job_flags &= ~(1 << JF_UPDATE_MINUTE);
    portEXIT_CRITICAL(&timer_1_Mux);
    if (refresh_okay) { 
      if (!menu.is_active()) tft.refresh_graph();
      update_alarm();
      refresh_okay = false;
    }
  }

  if (job_flags & (1 << JF_FLASH_LEFT)) {
    portENTER_CRITICAL(&timer_3_Mux);
    job_flags &= ~(1 << JF_FLASH_LEFT);
    portEXIT_CRITICAL(&timer_3_Mux);
    if (gl_flash_on > 0) {
      digitalWrite(PIN_FLASH_LED_LEFT, HIGH);
      left_flash_cnt = gl_flash_duration;
    }
    if (!menu.is_active()) tft.draw_left_cnt_sum(left_cnt, left_sum, right_sum);
  }

  if (job_flags & (1 << JF_FLASH_RIGHT)) {
    portENTER_CRITICAL(&timer_3_Mux);
    job_flags &= ~(1 << JF_FLASH_RIGHT);
    portEXIT_CRITICAL(&timer_3_Mux);
    if (gl_flash_on > 0) {
      digitalWrite(PIN_FLASH_LED_RIGHT, HIGH);
      right_flash_cnt = gl_flash_duration;
    }
    if (!menu.is_active()) tft.draw_right_cnt_sum(right_cnt, left_sum, right_sum);
  }

  // check serial interface
  if (Serial.available() > 0) cl.check_ser_input();
  
  // check keyboard
  key = read_keyboard();
  
  if (key > 0) {
    if (menu.is_active()) menu.process_key(key);
    else if (!tft.get_status()) tft.start();
    else {
      switch (key) {

        case 1:   // left button
          if (gl_flash_on > 0) {
            tft.show_message("Flash off");
            gl_flash_on = 0;
          } else {
            tft.show_message("Flash on");
            gl_flash_on = 1;
          }
          NVS.setInt(nvs_flash_on, gl_flash_on);
          break;

        case 2:   // up button
          switch (tft.toogle_zoom_mode()) {
            case 0: 
              tft.show_message("Zoom off");
              break;
            case 1:
              tft.show_message("Zoom 2 fold");
              break;
            case 2:
              tft.show_message("Zoom 4 fold");
              break;
          }
          break;

        case 4:   // right button
          if (gl_read_flash_data) {
            tft.show_message("Detector off");
            gl_read_flash_data = false;
          } else {
            tft.show_message("Detector on");
            gl_read_flash_data = true;
          }
          break;

        case 8:   // down button
          tft.stop();
          break;

        case 16:  // center button
          menu.init();
          break;
        
        default:
          Serial.print(key);
      }
    }
  }
  
  delay(1);
}
