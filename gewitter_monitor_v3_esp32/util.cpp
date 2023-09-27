#include "util.h"

/* trend_to_str ------------------------------------------------------------------------------------------------------------------
 *  Converst the trend (m factor of linear regression) to a string  
 *  Factor is divided by 10 and shown with a decimal fraction of 10
 *  '+' or '-' are included, always
 *  
 */
void trend_to_str(float m, char str_buf[]) {
  uint8_t digit;
  uint8_t i = 0;
  int32_t trend;

  if (m > 999.9) {
    strcpy(str_buf, "> +999");
    return;
  }

  if (m < -999.9) {
    strcpy(str_buf, "< -999.9");
    return;
  }
  
  if (m > 0) trend = (uint32_t) (m * 10 + 0.5);
  else trend = (uint32_t) (m * 10 - 0.5);
  if (trend == 0) {
    str_buf[i++] = '0';
    str_buf[i] = '\0';
    return;
  };
  
  if (trend > 0) str_buf[i++] = '+';
  else {
    str_buf[i++] = '-';
    trend = -trend;
  };
  digit = trend / 1000;
  if (digit > 0) {
    str_buf[i++] = '0' + digit;
    trend -= 1000 * digit;
  };
  digit = trend / 100;
  if (digit > 0) {
    str_buf[i++] = '0' + digit;
    trend -= 100 * digit;
  };
  digit = trend / 10;
  str_buf[i++] = '0' + digit;
  if (digit > 0) trend -= 10 * digit;
  str_buf[i++] = '.';
  str_buf[i++] = '0' + trend;
  str_buf[i] = '\0';
}

/*----------------------------------------------------------------------------------------------------------
*  Calculates a linear regression for buffer_values within the specified time window
*  Checks validity and correlation coefficient. 
*  Return value: true -> lin regression successful
*                false -> lin regrssion failed due to lack of correlation
*/
bool calc_lin_reg(uint8_t minute_cnt, float *m, int32_t *b) {
  extern uint32_t data_left[DATA_MAX], data_right[DATA_MAX];
  extern uint16_t data_pnt;
  uint32_t x, y, x_sum = 0, y_sum = 0, x_square_sum = 0;
  float y_square_sum = 0.0, x_y_sum = 0.0;
  float sxx, sxy, syy, x_avg, y_avg, denominator, r;
  uint16_t pnt = data_pnt;

  // Calculate sums and products
  if (pnt == 0) pnt = DATA_MAX - 1;
  else pnt -= 1;
  for (uint8_t i = 0; i < minute_cnt; ++i) {
    x = (minute_cnt - i);
    x_sum += x;
    x_square_sum += x * x;
    y = data_left[pnt] + data_right[pnt];
    y_sum += y;
    y_square_sum += (float) y * y;
    x_y_sum += (float) x * y;
    if (pnt == 0) pnt = DATA_MAX - 1;
    else pnt -= 1;
  }

  // Calculate regression 
  x_avg = (float) x_sum / minute_cnt;
  y_avg = (float) y_sum / minute_cnt;
  sxx = (float) x_square_sum - (minute_cnt * x_avg * x_avg);
  sxy = x_y_sum - (minute_cnt * x_avg * y_avg);
  syy = y_square_sum - (minute_cnt * y_avg * y_avg);
  // Serial.print("Sxx: "); Serial.println(sxx);
  // Serial.print("Syy: "); Serial.println(syy);
  denominator = sxx * syy;
  // Serial.print("Denominator: "); Serial.println(denominator);
  // check validity and calcuate correlation coefficient
  if (denominator <= 0) return(false);                  // stop in case of negative denominator

  r = sxy / sqrt(sxx * syy);
  // Serial.print("R: "); Serial.println(r);
  if (abs(r) < 0.2) return false;                       // stop in case of correlation below 0.25
 
  // calculate regression coefficients
  *m = sxy / sxx;                        
  if ((*m > -0.2) && (*m < +0.2)) return false;         // trend is almost zero - nothing to show               

  *b = (int32_t) y_avg - *m * x_avg;
  // Serial.print("M: "); Serial.println(*m);
  // Serial.print("B: "); Serial.println(*b);
  return(true);      
}

/* itoaf ---------------------------------------------------------------------------------------------------
* Converts an integer to a string including basic formatting information
* positions -> total number of digits to be reserved 
* dec_point -> number of positions from the right for a decimal point. 0 -> no decimal point
*/
void itoaf(int32_t value, char *s, uint8_t digits, const uint8_t dec_point, bool lead_zero) {
  bool neg = false;
  uint8_t count, blank_flag = 0;
  const uint32_t start_denom[] = {0, 10, 100, 1000, 10000, 100000, 1000000};
  uint32_t denominator;

  if (value < 0) {
    neg = true;
    value = -value;
  };
  if (digits > 7) digits = 7;
  if (digits < 1) digits = 1;
  denominator = start_denom[digits - 1];
  while (digits > 0) {
    count = value / denominator;
    value -= count * denominator;
    if ((digits == dec_point + 1) && !blank_flag) {
      if (neg) *s++ = ('-');
      else *s++ = (' ');
      blank_flag = 1;
    }
    if (digits == dec_point) {
      *s++ = ('.');
    }
    if ((count == 0) && (blank_flag == 0)) {
      if (lead_zero) *s++ = ('0');
      else *s++ = (' ');
    } else {
      if (blank_flag == 0) {
        if (neg) *s++ = ('-');
        else *s++ = (' ');
      };
      *s++ = ('0' + count);
      blank_flag = 1;
    };
    --digits;
    denominator /= 10;
  };
  *s = '\0';
}


//---------------------------------------------------------------------
void update_alarm(void) {
  extern uint32_t data_left[], data_right[];
  extern uint16_t data_pnt;
  extern int gl_alarm_window;
  uint8_t pnt;
  uint32_t min_sum;

  if (gl_alarm_window > 0) {
    pnt = data_pnt;
    if (pnt == 0) pnt = DATA_MAX - 1;
    else pnt -= 1;
    // calculate mean over alarm window
    min_sum = 0;
    for (int i = 0; i < gl_alarm_window; ++i) {
      min_sum += (data_left[pnt] + data_right[pnt]);
      if (pnt == 0) pnt = DATA_MAX - 1;
      else pnt -= 1;
    }
    set_alarm_leds(min_sum / gl_alarm_window);
   
  } else {
    set_alarm_leds(0);
  }
}


//---------------------------------------------------------------------
void calc_alarm_levels(void) {
  extern uint32_t gl_alarm_level, gl_alarm_1, gl_alarm_2, gl_alarm_3, gl_alarm_4;

  int step= gl_alarm_level / 20;
  gl_alarm_1 = 3 * step;
  gl_alarm_2 = 8 * step;
  gl_alarm_3 = 12 * step;
  gl_alarm_4 = 16 * step;
}

//---------------------------------------------------------------------
void set_alarm_leds(uint32_t data_mean) {
  extern uint32_t gl_alarm_level, gl_alarm_1, gl_alarm_2, gl_alarm_3, gl_alarm_4;
  extern Display tft;

  // Serial.print("data_mean: "); Serial.println(data_mean);
  // switch display on in case the mean is above 1st alarm level
  if (data_mean > gl_alarm_1) {
    tft.start();
  }

  if (data_mean > gl_alarm_level) {
    digitalWrite(PIN_LED_RED, HIGH);
    digitalWrite(PIN_LED_YELLOW, LOW);
    digitalWrite(PIN_LED_GREEN, LOW);
    return;
  }
  if (data_mean > gl_alarm_4) {
    digitalWrite(PIN_LED_RED, HIGH);
    digitalWrite(PIN_LED_YELLOW, HIGH);
    digitalWrite(PIN_LED_GREEN, LOW);
    return;
  }
  if (data_mean > gl_alarm_3) {
    digitalWrite(PIN_LED_RED, LOW);
    digitalWrite(PIN_LED_YELLOW, HIGH);
    digitalWrite(PIN_LED_GREEN, LOW);
    return;
  }
  if (data_mean > gl_alarm_2) {
    digitalWrite(PIN_LED_RED, LOW);
    digitalWrite(PIN_LED_YELLOW, HIGH);
    digitalWrite(PIN_LED_GREEN, HIGH);
    return;
  }
  digitalWrite(PIN_LED_RED, LOW);
  digitalWrite(PIN_LED_YELLOW, LOW);
  digitalWrite(PIN_LED_GREEN, HIGH);
}
