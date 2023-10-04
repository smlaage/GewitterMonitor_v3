#include "display.h"


//--------------------------------------------------------------------------------
void Display::init(void) {

  digitalWrite(PIN_LCD_PWR, HIGH);
  is_on = false;

  delay(30);
  etft.init();
  etft.setRotation(1);
  etft.fillScreen(TFT_BLACK);
  etft.setTextSize(2);
  etft.setTextDatum(MC_DATUM);
  etft.drawString(title_msg1, x_max / 2, 140, 2);
  etft.drawString(title_msg2, x_max / 2, 200, 1);
  etft.drawString(title_msg3, x_max / 2, 230, 1);
  delay(1000);
}


//--------------------------------------------------------------------------------
void Display::draw_menu_item(int i, char label[], char value_str[]) {
  int y_pos = menu_y + menu_y_pos + i * menu_y_delta;

  etft.setTextDatum(TL_DATUM);
  etft.drawString(label, menu_x + menu_x_label, y_pos);
  etft.setTextDatum(TR_DATUM);
  etft.drawString(value_str, menu_x + menu_x_value, y_pos);
}

//--------------------------------------------------------------------------------
void Display::enter_menu(void) {
  if (!is_on) return;

  etft.fillRect(menu_x, menu_y, menu_width, menu_height, TFT_BLACK);
  etft.drawRect(menu_x - 1, menu_y - 1, menu_width + 2, menu_height + 2, axis_color);
}

//--------------------------------------------------------------------------------
void Display::enter_voltage(void) {
  if (!is_on) return;

  etft.setTextDatum(TL_DATUM);
  etft.fillRect(menu_x, menu_y, menu_width, menu_height, TFT_BLACK);
  etft.drawRect(menu_x - 1, menu_y - 1, menu_width + 2, menu_height + 2, axis_color);
  etft.drawString("Voltage Adjustment", menu_x + menu_x_label, menu_y + menu_y_pos + 2 * menu_y_delta);
  etft.drawString("Channel green [V]", menu_x + menu_x_label, menu_y + menu_y_pos + 5 * menu_y_delta);
  etft.drawString("Channel red [v]", menu_x + menu_x_label, menu_y + menu_y_pos + 7 * menu_y_delta);
}

//--------------------------------------------------------------------------------
void Display::update_voltage(char v_green[], char v_red[]) {
  if (!is_on) return;

  etft.setTextDatum(TR_DATUM);
  etft.drawString(v_green, menu_x + menu_x_value, menu_y + menu_y_pos + 5 * menu_y_delta);
  etft.drawString(v_red, menu_x + menu_x_value, menu_y + menu_y_pos + 7 * menu_y_delta);
}

//--------------------------------------------------------------------------------
void Display::exit_menu(void) {
  if (!is_on) return;

  etft.fillRect(menu_x - 1, menu_y - 1, menu_width + 2, menu_height + 2, TFT_BLACK);
  refresh_graph();
  draw_static();
}

//--------------------------------------------------------------------------------
void Display::set_menu_item(int i) {
  if (!is_on) return;

  int y_pos = menu_y + menu_y_pos + i * menu_y_delta;
  etft.drawRect(menu_x + menu_x_label - 11, y_pos - 4, menu_width - 18, 22, TFT_YELLOW);
  etft.drawRect(menu_x + menu_x_label - 10, y_pos - 3, menu_width - 20, 20, TFT_YELLOW);
}

//--------------------------------------------------------------------------------
void Display::clear_menu_item(int i, char value_str[]) {
  if (!is_on) return;

  int y_pos = menu_y + menu_y_pos + i * menu_y_delta;
  char buf[16] = "    ";

  etft.setTextDatum(TR_DATUM);
  strcat(buf, value_str);
  etft.drawString(buf, menu_x + menu_x_value, y_pos);
  etft.drawRect(menu_x + menu_x_label - 11, y_pos - 4, menu_width - 18, 22, TFT_BLACK);
  etft.drawRect(menu_x + menu_x_label - 10, y_pos - 3, menu_width - 20, 20, TFT_BLACK);
}

//--------------------------------------------------------------------------------
void Display::update_menu_item(int i, char value_str[]) {
  if (!is_on) return;

  int y_pos = menu_y + menu_y_pos + i * menu_y_delta;
  char buf[16] = "    ";

  etft.setTextDatum(TR_DATUM);
  strcat(buf, value_str);
  etft.drawString(buf, menu_x + menu_x_value, y_pos);
}

//--------------------------------------------------------------------------------
void Display::draw_static(void) {
  etft.drawFastHLine(0, y_org, x_org, axis_line_color);
  etft.drawFastVLine(x_org + 1, 0, y_org + 1, axis_line_color);

  etft.setTextDatum(BL_DATUM);
  etft.setTextColor(axis_color, TFT_BLACK);
  etft.drawFastHLine(x_scale_max, y_scale_max - 4, x_max - x_scale_max, axis_line_color);
  etft.drawString("Scale", x_scale_max, y_scale_max - 6, 1);
  etft.drawFastHLine(x_cnt, y_cnt - 4, x_max - x_cnt, axis_line_color);
  etft.drawString("Count", x_cnt, y_cnt - 6, 1);
  etft.drawFastHLine(x_sum, y_sum - 4, x_max - x_sum, axis_line_color);
  etft.drawString("Sum", x_sum, y_sum - 6, 1);
  etft.drawFastHLine(x_trend, y_trend - 4, x_max - x_trend, axis_line_color);
  etft.drawString("Trend", x_trend, y_trend - 6, 1);
  etft.drawFastHLine(x_hpa, y_hpa - 4, x_max - x_hpa, axis_line_color);
  etft.drawString("hPa", x_hpa, y_hpa - 6, 1);
  etft.setTextDatum(BL_DATUM);
  etft.drawFastHLine(x_minute, y_time - 4, x_max - x_minute, axis_line_color);
  etft.drawString("Time", x_minute, y_time - 6, 1);
  
  etft.setTextDatum(TC_DATUM);
  etft.drawString(":", x_minute + (x_max - x_minute) / 2, y_time, 1);
  etft.drawFastHLine(x_org + 2, 0, 4, axis_color);
}

//--------------------------------------------------------------------------------
void Display::start(void) {
  extern int gl_display_timeout;
  char buf[10];

  timeout_cnt = gl_display_timeout;

  if (is_on) return;

  digitalWrite(PIN_LCD_PWR, HIGH);
  is_on = true;
  delay(30);
  etft.init();
  etft.setRotation(1);
  etft.fillScreen(TFT_BLACK);
  etft.setTextSize(2);

  refresh_graph();
  draw_static();  
}

//--------------------------------------------------------------------------------
void Display::restart(void) {
  extern int gl_display_timeout;

  digitalWrite(PIN_LCD_PWR, HIGH);
  is_on = true;
  timeout_cnt = gl_display_timeout;
  refresh_graph();
  draw_static();
}

//--------------------------------------------------------------------------------
void Display::stop(void) {
  digitalWrite(PIN_LCD_PWR, LOW);
  is_on = false;
}

//------------------------------------------------------------------------------------------
void Display::refresh_graph(void) {
  extern int gl_scale_min;
  int scale_min;
  extern uint16_t left_cnt, right_cnt;
  extern uint32_t left_sum, right_sum;
  extern uint32_t data_left[], data_right[], data_hpa[];
  extern uint16_t data_pnt;
  extern bool bmp_found;
  uint32_t scale_max = 0;
  uint32_t hpa_sum = 0, hpa_mean;
  uint32_t min_sum = 0;
  uint16_t pnt, hpa_cnt = 0;
  uint16_t y_left, y_right, x;
  uint16_t y_org_2 = y_org / 2;
  uint32_t scale_factor;
  int y_hpa_plot, last_y_hpa_plot;
  int y_hpa, last_y_hpa = -1;
  char buf[10];
  bool trend_minus = true;
  
  if (!is_on) return;

  scale_min = gl_scale_min - 1;

  // Serial.print("Timeout: "); Serial.println(timeout_cnt);
  if (timeout_cnt > 0) {
    timeout_cnt -= 1;
    if (timeout_cnt == 0) {
      stop();
      return;
    }
  }
  
  draw_x_axis();
  // refresh_minute();
  draw_cnt_sum(left_cnt, right_cnt, left_sum, right_sum);
  // find maximum, calculate hpa mean
  for (int i = 0; i < DATA_MAX; ++i) {
    if (data_left[i] + data_right[i] > scale_max) scale_max = data_left[i] + data_right[i];
    if (bmp_found && (data_hpa[i] > 0)) {
      hpa_sum += data_hpa[i];
      hpa_cnt += 1;
    }
  }
  if (hpa_cnt > 0) hpa_mean = hpa_sum / hpa_cnt;
  else hpa_mean = 10133;
  // Serial.print("hPa mean: "); Serial.print(hpa_mean); Serial.print("   hpa_cnt: "); Serial.println(hpa_cnt);
  // Calculate scale
  if (scale_max < scale_min) scale_max = scale_min;
  if (scale_max > 100000) scale_factor = 100000;
  else if (scale_max > 10000) scale_factor = 10000;
  else if (scale_max > 1000) scale_factor = 1000;
  else scale_factor = 100;
  if (zoom_mode == 0) {
    scale_max = (scale_max / scale_factor + 1) * scale_factor;
  } else if (zoom_mode == 1) {
    scale_max = ((scale_max / 2) / scale_factor + 1) * scale_factor;
  } else {
    scale_max = ((scale_max / 4) / scale_factor + 1) * scale_factor;
  }
  draw_y_axis(scale_max);

  etft.fillRect(0, 0, x_org + 1, y_org, TFT_BLACK);

  pnt = data_pnt;
  if (pnt == 0) pnt = DATA_MAX - 1;
  else pnt -= 1;
  x = x_org;
  for (int i = 0; i < DATA_MAX; ++i) {
    // plot y_right
    if (data_right[pnt] > 0) {
      y_right = data_right[pnt] * y_org / scale_max;
      etft.drawFastVLine(x - 1, y_org - y_right, y_right, right_color);
      etft.drawFastVLine(x,     y_org - y_right, y_right, right_color);
    } else {
      y_right = 0;
    }

    // plot y_left
    if (data_left[pnt] > 0) {
      y_left = data_left[pnt] * y_org / scale_max;
      etft.drawFastVLine(x - 1, y_org - y_left - y_right, y_left, left_color);
      etft.drawFastVLine(x,     y_org - y_left - y_right, y_left, left_color);
    }

    // plot hpa (ignore last data point)
    if (bmp_found && (i < DATA_MAX - 1)) {
      switch (hpa_plot_mode) {
        case 0:       // no plot
          break;
        case 1:       // dots
          if (data_hpa[pnt] > 0) {
            y_hpa = y_org_2 - (data_hpa[pnt] - hpa_mean);
            if ((y_hpa <= y_org) && (y_hpa >= 0))
              etft.drawFastHLine(x - 1, y_hpa, 2, hpa_color);
          }
          break;
        case 2:       // line
          if (data_hpa[pnt] > 0) {
            y_hpa = y_org_2 - (data_hpa[pnt] - hpa_mean);
            if (last_y_hpa < 0) {
              etft.drawFastHLine(x - 1, y_hpa, 2, hpa_color);
            } else {
              // plot line at position x 
              if (y_hpa > y_org) y_hpa_plot = y_org;
              else y_hpa_plot = y_hpa;
              if (last_y_hpa > y_org) last_y_hpa_plot = y_org;
              else last_y_hpa_plot = last_y_hpa;
              if (last_y_hpa_plot > y_hpa_plot) 
                etft.drawFastVLine(x, y_hpa_plot, last_y_hpa_plot - y_hpa_plot, hpa_color);
              else if (last_y_hpa_plot == y_hpa_plot) etft.drawPixel(x, y_hpa_plot, hpa_color);
              else etft.drawFastVLine(x, last_y_hpa_plot, y_hpa_plot - last_y_hpa_plot, hpa_color);
              // plot point at position x - 1
              if ((y_hpa <= y_org) && (y_hpa >= 0)) {
                etft.drawPixel(x - 1, y_hpa, hpa_color);
              }
            }
            last_y_hpa = y_hpa; 
          }
          break;
      }
    }
    x -= 2;
    if (pnt == 0) pnt = DATA_MAX - 1;
    else pnt -= 1;
  }

  // Calculate trend
  float m;
  int32_t b;
  if (calc_lin_reg(regression_window, &m, &b)) {
    // Serial.println("LinReg Success");
    // Serial.print("m: "); Serial.print(m); Serial.print("   b: "); Serial.println(b);
    int32_t x1, y1, x2, y2;
    uint16_t y1_scaled, y2_scaled;
    // clipping at left border (left side of regression window)
    x1 = 0;
    y1 = b;
    if (y1 < 0) {
      x1 = (int32_t) -b / m;
      y1 = 0;
    } else if (y1 > scale_max) {
      x1 = (int32_t) (scale_max -b) / m;
      y1 = scale_max;
    }
    // clipping at right border (right side of regression window)
    x2 = regression_window;
    y2 = (int32_t) m * x2 + b;
    if (y2 < 0) {
      x2 = (int32_t) -b / m;
      y2 = 0;
    } else if (y2 > scale_max) {
      x2 = (int32_t) (scale_max - b) / m;
      y2 = scale_max;
    };
    // Serial.print("x1, y1: "); Serial.print(x1); Serial.print(" "); Serial.println(y1);
    // Serial.print("x2, y2: "); Serial.print(x2); Serial.print(" "); Serial.println(y2);
    y1_scaled = (uint32_t) (y1 * y_org / scale_max);
    y2_scaled = (uint32_t) (y2 * y_org / scale_max);
    // Serial.print("y1_scaled: "); Serial.print(y1_scaled); Serial.print("  y2_scaled: "); Serial.print(y2_scaled);  
    etft.drawLine(x_org - 2 * regression_window + 2 * x1, y_org - y1_scaled, 
                  x_org - 2 * regression_window + 2 * x2, y_org - y2_scaled, regression_color);
    // Update trend
    trend_to_str(m, buf);
    if (m > 0) trend_minus = false;
  } else {
    strcpy(buf, "--.-");
  };
  etft.fillRect(x_trend, y_trend, x_delete_area, 16, TFT_BLACK);
  etft.setTextDatum(TR_DATUM);
  if (trend_minus) etft.setTextColor(axis_color, TFT_BLACK);
  else etft.setTextColor(alarm_color, TFT_BLACK);
  etft.drawString(buf, x_max, y_trend, 1);

  
}

//--------------------------------------------------------------------------------
void Display::draw_x_axis(void) {
  extern MyRTC rtc;
  extern uint8_t second;
  uint8_t hour, minute;
  char buf[8];

  if (!is_on) return;

  hour = rtc.get_hour();
  minute = rtc.get_minute();
  second = rtc.get_second();
  refresh_minute(minute);
  
  etft.fillRect(0, y_org + 1, x_org + 10, 320 - y_org - 1, TFT_BLACK);
  etft.setTextDatum(TC_DATUM);
  etft.setTextColor(axis_color, TFT_BLACK);

  buf[0] = '0' + hour / 10;
  buf[1] = '0' + hour % 10;
  buf[2] = '\0';
  etft.drawString(buf, x_org - 2 * minute, y_org + 4, 1);
  if (hour > 0) hour -= 1;
  else hour = 23;
  buf[0] = '0' + hour / 10;
  buf[1] = '0' + hour % 10;
  etft.drawString(buf, x_org - 120 - 2 * minute, y_org + 4, 1);
  if (hour > 0) hour -= 1;
  else hour = 23;
  buf[0] = '0' + hour / 10;
  buf[1] = '0' + hour % 10;
  etft.drawString(buf, x_org - 240 - 2 * minute, y_org + 4, 1);

  if (minute > 30) etft.drawFastVLine(x_org - 2 * (minute - 30), y_org, 8, axis_color);
  etft.drawFastVLine(x_org -  60 - 2 * minute, y_org, large_tick, axis_color);
  etft.drawFastVLine(x_org - 180 - 2 * minute, y_org, large_tick, axis_color);
  etft.drawFastVLine(x_org - 300 - 2 * minute, y_org, large_tick, axis_color);

  if (minute > 15) etft.drawFastVLine(x_org - 2 * (minute - 15), y_org, 4, axis_color);
  if (minute > 45) etft.drawFastVLine(x_org - 2 * (minute - 45), y_org, 4, axis_color);
  etft.drawFastVLine(x_org -  30 - 2 * minute, y_org, small_tick, axis_color);
  etft.drawFastVLine(x_org -  90 - 2 * minute, y_org, small_tick, axis_color);
  etft.drawFastVLine(x_org - 150 - 2 * minute, y_org, small_tick, axis_color);
  etft.drawFastVLine(x_org - 210 - 2 * minute, y_org, small_tick, axis_color);
  etft.drawFastVLine(x_org - 270 - 2 * minute, y_org, small_tick, axis_color);
  etft.drawFastVLine(x_org - 330 - 2 * minute, y_org, small_tick, axis_color);
  etft.drawFastVLine(x_org - 390 - 2 * minute, y_org, small_tick, axis_color);
}

//------------------------------------------------------------------------------------------
void Display::draw_y_axis(uint32_t scale_max) {
  char buf[10];
  const char labels[][4] = {"0", "5", "10", "15", "20", "25", "30", "35", "40", "45"}; 
  int y;
  uint8_t first_digit;

  etft.fillRect(x_scale_max, y_scale_max, x_delete_area, 16, TFT_BLACK);
  etft.setTextDatum(TR_DATUM);
  etft.setTextColor(axis_color, TFT_BLACK);
  ultoa(scale_max, buf, 10);
  etft.drawString(buf, x_max, y_scale_max, 1);

  etft.fillRect(x_org + 2, 1, 38, y_org + 7, TFT_BLACK);
  
  buf[1] = '\0';
  first_digit = buf[0] - '0';
  etft.setTextDatum(CL_DATUM);
  switch (first_digit) {
    case 1:
      for (int i = 0; i < 10; i += 2) {
        y = y_org - i * (y_org / 10);
        etft.drawFastHLine(x_org + 2, y, 4, axis_color);
        buf[0] = '0' + i;
        etft.drawString(buf, x_org + 8, y, 1);
      }
      break;
    case 2:
    case 3:
      for (int i = 0; i < first_digit * 2; ++i) {
        y = y_org - i * (y_org / (first_digit * 2));
        etft.drawFastHLine(x_org + 2, y, 4, axis_color);
        etft.drawString(labels[i], x_org + 8, y, 1);
      }
      break;
    case 4:
    case 5:
    case 6:
      for (int i = 0; i < first_digit * 2; ++i) {
        y = y_org - i * (y_org / (first_digit * 2));
        etft.drawFastHLine(x_org + 2, y, 4, axis_color);
        buf[0] = '0' + (i / 2);
        if (i % 2 == 0) etft.drawString(buf, x_org + 8, y, 1);
      }
      break;
    case 7:
    case 8:
    case 9:
      for (int i = 0; i < first_digit; ++i) {
        y = y_org - i * (y_org / first_digit);
        etft.drawFastHLine(x_org + 2, y, 4, axis_color);
        buf[0] = '0' + i;
        etft.drawString(buf, x_org + 8, y, 1);
      }
      break;
  }
}

//------------------------------------------------------------------------------------------
void Display::draw_cnt_sum(uint16_t left_cnt, uint16_t right_cnt, uint32_t left_sum, uint32_t right_sum) {
  char buf[10];

  if (!is_on) return;

  etft.fillRect(x_cnt, y_cnt, x_delete_area, 36, TFT_BLACK);
  etft.fillRect(x_sum, y_sum, x_delete_area, 36, TFT_BLACK);
  etft.setTextDatum(TR_DATUM);

  etft.setTextColor(left_color, TFT_BLACK);
  itoa(left_cnt, buf, 10);
  etft.drawString(buf, x_max, y_cnt, 1);
  ultoa(left_sum, buf, 10);
  etft.drawString(buf, x_max, y_sum, 1);

  etft.setTextColor(right_color, TFT_BLACK);
  itoa(right_cnt, buf, 10);
  etft.drawString(buf, x_max, y_cnt + 20, 1);
  ultoa(right_sum, buf, 10);
  etft.drawString(buf, x_max, y_sum +  20, 1);


}

//------------------------------------------------------------------------------------------
void Display::draw_left_cnt_sum(uint16_t left_cnt, uint32_t left_sum, uint32_t right_sum) {
  extern int gl_alarm_1, gl_display_timeout;
  char buf[10];
  
  // switch display on in case the current  value is above 1st alarm level
  if (left_sum + right_sum > gl_alarm_1) {
    timeout_cnt = gl_display_timeout;
    if (!is_on) start();
  }

  if (!is_on) return;

  etft.fillRect(x_cnt, y_cnt, x_delete_area, 16, TFT_BLACK);
  etft.setTextDatum(TR_DATUM);
  etft.setTextColor(left_color, TFT_BLACK);
  itoa(left_cnt, buf, 10);
  etft.drawString(buf, x_max, y_cnt, 1);
  etft.fillRect(x_sum, y_sum, x_delete_area, 16, TFT_BLACK);
  ultoa(left_sum, buf, 10);
  etft.drawString(buf, x_max, y_sum, 1);
}

//------------------------------------------------------------------------------------------
void Display::draw_right_cnt_sum(uint16_t right_cnt, uint32_t left_sum, uint32_t right_sum) {
  extern int gl_alarm_1, gl_display_timeout;
  char buf[10];

  // switch display on in case the current  value is above 1st alarm level
  if (left_sum + right_sum > gl_alarm_1) {
    timeout_cnt = gl_display_timeout;
    if (!is_on) start();
  }

  if (!is_on) return;

  etft.fillRect(x_cnt, y_cnt + 20, x_delete_area, 16, TFT_BLACK);
  etft.setTextDatum(TR_DATUM);
  etft.setTextColor(right_color, TFT_BLACK);
  itoa(right_cnt, buf, 10);
  etft.drawString(buf, x_max, y_cnt + 20, 1);
  etft.fillRect(x_sum, y_sum + 20, x_delete_area, 16, TFT_BLACK);
  ultoa(right_sum, buf, 10);
  etft.drawString(buf, x_max, y_sum + 20, 1);
}

//--------------------------------------------------------------------------------
void Display::refresh_hpa(uint32_t hpa) {
  char buf[10];

  itoaf(hpa / 10, buf, 5, 1, false);
  etft.fillRect(x_hpa, y_hpa, x_delete_area, 16, TFT_BLACK);
  etft.setTextDatum(TR_DATUM);
  etft.setTextColor(axis_color, TFT_BLACK);
  etft.drawString(buf, x_max, y_hpa, 1);
}

//--------------------------------------------------------------------------------
void Display::refresh_minute(uint8_t minute) {
  char buf[3];

  if (!is_on) return;

  buf[0] = '0' + minute / 10;
  buf[1] = '0' + minute % 10;
  buf[2] = '\0';
  etft.setTextDatum(TL_DATUM);
  etft.drawString(buf, x_minute, y_time, 1);
}

//--------------------------------------------------------------------------------
void Display::refresh_second(void) {
  extern uint8_t second;
  char buf[3];
  
  if (!is_on) return;

  buf[0] = '0' + second / 10;
  buf[1] = '0' + second % 10;
  buf[2] = '\0';
  etft.setTextDatum(TR_DATUM);
  etft.setTextColor(axis_color, TFT_BLACK);
  etft.drawString(buf, x_max, y_time, 1);
}

//--------------------------------------------------------------------------------
bool Display::get_status(void) {
  return is_on;
}

//--------------------------------------------------------------------------------
int Display::toogle_zoom_mode(void) {
  if (zoom_mode == 2) zoom_mode = 0;
  else zoom_mode += 1;
  return zoom_mode;
}

//---------------------------------------------------------------------------------
void Display::show_message(char msg[]) {
  const int x = 50, y = 130, xw = x_org - 2 * x, yw = y_org - 2 * y;

  etft.drawRect(x - 4, y - 3, xw + 8, yw + 6, TFT_BLUE);
  etft.fillRect(x, y, xw, yw, TFT_BLUE);
  etft.setTextDatum(MC_DATUM);
  etft.setTextColor(axis_color, TFT_BLUE);
  etft.drawString(msg, x + xw / 2, y + yw / 2);
  etft.setTextColor(axis_color, TFT_BLACK);
  delay(2000);
  refresh_graph();
}
