#ifndef __GEWITTER_MONITOR__
#define __GEWITTER_MONITOR__

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <ArduinoNvs.h>

#define PIN_LCD_PWR          25  
#define PIN_BT_RIGHT         16
#define PIN_BT_UP            17
#define PIN_BT_LEFT           4
#define PIN_BT_DOWN          32
#define PIN_BT_CENTER        33
#define PIN_ADC_LEFT         35
#define PIN_ADC_RIGHT        34
#define PIN_VOLTAGE_A        36
#define PIN_VOLTAGE_B        39
#define PIN_FLASH_LED_LEFT   26
#define PIN_FLASH_LED_RIGHT  27
#define PIN_LED_GREEN        14
#define PIN_LED_YELLOW       12
#define PIN_LED_RED          13

#define DATA_MAX            180

// Job flags
#define JF_UPDATE_SECOND      0
#define JF_UPDATE_MINUTE      1
#define JF_FLASH_LEFT         2
#define JF_FLASH_RIGHT        3

// default parameter
#define DEFAULT_HPA_OFFSET           4000
#define DEFAULT_DETECTION_THRESHOLD   150
#define DEFAULT_FLASH_DURATION        100
#define DEFAULT_ALARM_LEVEL          8000
#define DEFAULT_ALARM_WINDOW            5
#define DEFAULT_DISPLAY_TIMEOUT        10
#define DEFAULT_FLASH_ON                1
#define DEFAULT_SCALE_MIN             500

#define VOLTAGE_CONVERSION 3797

#define DEMO_MODE                       0

// title message
const char title_msg1[] = "Gewitter Monitor v3";
const char title_msg2[] = "SLW 23-09-14";
const char title_msg3[] = "laagewitt.de";

// global parameter names
const char nvs_flash_on[] = "flash_status";
const char nvs_hpa_offset[] = "hpa_offset";
const char nvs_detection_threshold[] = "detection_th";
const char nvs_flash_duration[] = "flash_duration";
const char nvs_alarm_level[] = "alarm_level";
const char nvs_alarm_window[] = "alarm_window";
const char nvs_display_timeout[] = "disp_timeout";
const char nvs_scale_min[] = "scale_min";

#endif
