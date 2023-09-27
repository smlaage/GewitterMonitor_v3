#include "keyboard.h"

volatile char keyboard_buffer[KEYBOARD_BUFFER_SIZE];
volatile uint8_t keyboard_buffer_wr_pnt = 0;
volatile uint8_t keyboard_buffer_rd_pnt = 0;

//--------------------------------------------------------------
void IRAM_ATTR check_keyboard_isr(void) {
  extern hw_timer_t *timer_2;
  extern portMUX_TYPE timer_2_Mux;
  uint8_t keyboard_value;
  static uint8_t keyboard_value_old = 0;	
  static uint8_t keyboard_status = 0;	

  keyboard_value = 0;
  keyboard_value |= ((digitalRead(PIN_BT_LEFT) == LOW) << 0);
  keyboard_value |= ((digitalRead(PIN_BT_UP) == LOW) << 1);
  keyboard_value |= ((digitalRead(PIN_BT_RIGHT) == LOW) << 2);
  keyboard_value |= ((digitalRead(PIN_BT_DOWN) == LOW) << 3);
  keyboard_value |= ((digitalRead(PIN_BT_CENTER) == LOW) << 4);
  
  portENTER_CRITICAL_ISR(&timer_2_Mux);
  switch (keyboard_status) {
    case 0:												                  // key pressed first time
			keyboard_value_old = keyboard_value;			    // remember new keyboard value
			++keyboard_status;								            // switch keyboard_status to evaluation mode
			break; 
		case 2:												                  // capture key first time
			if (keyboard_value_old == keyboard_value) {		// still the same key?
				keyboard_buffer[keyboard_buffer_wr_pnt] = keyboard_value;
				if (++keyboard_buffer_wr_pnt == KEYBOARD_BUFFER_SIZE) keyboard_buffer_wr_pnt = 0;
				++keyboard_status;							            // keep an eye on how long the key is pressed
			} else keyboard_status = 0;
			break;
		case 15:											                  // capture repeated key
			if (keyboard_value_old == keyboard_value) {		// still the same key?
				keyboard_buffer[keyboard_buffer_wr_pnt] = keyboard_value;
				if (++keyboard_buffer_wr_pnt == KEYBOARD_BUFFER_SIZE) keyboard_buffer_wr_pnt = 0;
				keyboard_status = 12;
			} else keyboard_status = 0;
			break;		
		case 0xff:											                // wait for key to be released	
			break;
		default:
			if (keyboard_value_old == keyboard_value) ++keyboard_status;
			else keyboard_status = 0;						        // different key pressed -> reset keyboard routine
			break;
	}
  
  portEXIT_CRITICAL_ISR(&timer_2_Mux); 
}

//--------------------------------------------------------------
uint8_t read_keyboard(void) {
  uint8_t keyboard_value;
  
  if (keyboard_buffer_wr_pnt == keyboard_buffer_rd_pnt) return 0;

  keyboard_value = keyboard_buffer[keyboard_buffer_rd_pnt];
	if (++keyboard_buffer_rd_pnt == KEYBOARD_BUFFER_SIZE) keyboard_buffer_rd_pnt = 0;
	return keyboard_value;
}
