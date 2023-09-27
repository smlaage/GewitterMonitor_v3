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
    void decode_cmd(void);
    void decode_get_cmd(uint8_t pnt);
    void decode_set_cmd(uint8_t pnt);
    void print_config(void);
    int32_t get_int(uint8_t *pnt);

  public:
    void check_ser_input(void);

};

#endif
