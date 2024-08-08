#ifndef __UTIL__
#define __UTIL__

#include <Arduino.h>
#include "gewitter_monitor.h"
#include "display.h"

// Function prototypes
void trend_to_str(float m, char str_buf[]);
bool calc_lin_reg(uint8_t minute_cnt, float *m, int32_t *b);
void itoaf(int32_t value, char *s, uint8_t digits, const uint8_t dec_point, bool lead_zero);
void calc_alarm_levels(void);
void set_alarm_leds(uint32_t data_mean);
void update_alarm(void);

#endif
