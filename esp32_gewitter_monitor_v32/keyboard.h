#ifndef __KEYBOARD__
#define __KEYBOARD__

#include <Arduino.h>
#include "gewitter_monitor.h"

#define KEYBOARD_BUFFER_SIZE 32

void check_keyboard_isr(void);
uint8_t read_keyboard(void);

#endif
