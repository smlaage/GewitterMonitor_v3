#ifndef __DISPLAY__
#define __DISPLAY__

#include <Arduino.h>
#include <TFT_eSPI.h>
#include <SPI.h>
#include "gewitter_monitor.h"
#include "my_rtc.h"
#include "util.h"

class Display {
  private:
    TFT_eSPI etft = TFT_eSPI();
    bool is_on = false;
    uint8_t hpa_plot_mode = 2;                      // 0 -> no plot, 1-> dots, 2 -> line
    int last_y_hpa;
    void draw_x_axis(void);
    void draw_y_axis(uint32_t scale_max);
    
    int timeout_cnt = 0;
    int zoom_mode = 0;
  
    const uint16_t x_org = 360, y_org = 300;
    const uint16_t x_max = 479, y_max = 319;
    const uint16_t x_scale_max = 406, y_scale_max = 22;
    const uint16_t x_cnt = x_scale_max, y_cnt = y_scale_max + 51;                  // 72
    const uint16_t x_sum = x_scale_max, y_sum = y_cnt + 65;                        // 122
    const uint16_t x_trend = x_scale_max, y_trend = y_sum + 65;                    // 174
    const uint16_t x_hpa = x_scale_max, y_hpa = y_trend + 51;                      // 224
    const uint16_t x_minute = x_scale_max, x_second = 455, y_time = y_hpa + 50;    // 274
    const uint16_t x_delete_area = 59;
    const uint32_t axis_color = TFT_WHITE, left_color = TFT_GREEN, right_color = TFT_RED, alarm_color = TFT_ORANGE,
                   hpa_color = TFT_CYAN, regression_color = TFT_WHITE, axis_line_color = TFT_DARKGREY;
    const uint8_t small_tick = 6, large_tick = 12;
    const uint8_t regression_window = 60;
    const uint8_t scale_window = 120;
    // const uint32_t scale_min;
    const uint16_t menu_x = 65, menu_y = 17;
    const uint16_t menu_width = 350, menu_height = 286;
    const uint16_t menu_x_label = 20;
    const uint16_t menu_x_value = 330;
    const uint16_t menu_y_pos = 12;
    const uint16_t menu_y_delta = 25;


  public:
    void init(void);
    void enter_menu(void);
    void exit_menu(void);
    void draw_static(void); 
    void start(void);
    void restart(void);
    void stop(void);
    void draw_menu_item(int i, char label[], char value_str[]);
    void set_menu_item(int i);
    void clear_menu_item(int i, char value_str[]);
    void update_menu_item(int i, char value_str[]);
    bool get_status(void);
    void refresh_graph(void);
    void refresh_second(void);
    void refresh_minute(uint8_t minute);
    void refresh_hpa(uint32_t hpa);
    void draw_cnt_sum(uint16_t left_cnt, uint16_t right_cnt, uint32_t left_sum, uint32_t right_sum);
    void draw_left_cnt_sum(uint16_t left_cnt, uint32_t left_sum, uint32_t right_sum);
    void draw_right_cnt_sum(uint16_t right_cnt, uint32_t left_sum, uint32_t right_sum);
    void show_message(char msg[]);
    int toogle_zoom_mode(void);
    void enter_voltage(void);
    void update_voltage(char v_green[], char v_red[]); 
};

#endif