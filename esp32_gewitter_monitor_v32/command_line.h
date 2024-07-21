#ifndef __COMMAND_LINE__
#define __COMMAND_LINE__

#include <Arduino.h>
#include <ArduinoNvs.h>
#include "gewitter_monitor.h"
#include "my_rtc.h"
#include "util.h"

#define BUFFER_MAX_LEN       40

class CommandLine {
  private:
    char buffer[BUFFER_MAX_LEN];
    uint8_t buffer_pnt = 0;
    int command_line_status = 0;  // 0 -> normal, 1 -> get ssid
    int network_status = 0; // 0 -> no connection, 1 -> WiFi okay, 2 -> MQTT okay, -1 -> network disabled
    void decode_cmd(void);
    void decode_get_cmd(uint8_t pnt);
    void decode_set_cmd(uint8_t pnt);
    void decode_network_cmd(uint8_t pnt);
    void print_config(void);
    int32_t get_int(uint8_t *pnt);
    void get_network_ssid(void);
    int network_scan(void);
    void show_help(void);

  public:
    void check_ser_input(void);
    int network_connect(void);
    int get_network_status(void);
    void set_network_status(int status);
    bool publish_data(long left_sum, long right_sum, long hpa);

};

#endif
