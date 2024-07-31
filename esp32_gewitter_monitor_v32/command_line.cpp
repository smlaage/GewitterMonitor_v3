/*
Gewitter Monitor V3.2
Module "command_line"
*/

#include "command_line.h"

//---------------------------------------------------------------
void CommandLine::decode_set_cmd(uint8_t pnt) {
  extern Preferences prefs;
  extern MyRTC rtc;
  extern Display tft;
  extern Adafruit_BMP280 bmp;
  extern int gl_hpa_offset, gl_detection_threshold, gl_flash_duration, gl_flash_on, 
             gl_alarm_window, gl_display_timeout, gl_alarm_level, gl_scale_min;
  extern String gl_network_ssid, gl_network_password, gl_mqtt_host_ip, gl_mqtt_user, 
                gl_mqtt_password, gl_mqtt_topic;
  uint8_t year=0, month=0, day=0, hour=0, minute=0;
  bool success = true;
  int x;

  switch (buffer[pnt]) {
    case 'a':     // set alarm level
    case 'A':
      Serial.println("Set alarm level");
      pnt += 1;
      x = get_int(&pnt);
      if ((x > 500) && (x < 50000)) {
        gl_alarm_level = x;
        calc_alarm_levels();
        prefs.putInt(nvs_alarm_level, gl_alarm_level);
        Serial.print("  alarm level: "); Serial.println(gl_alarm_level);
        Serial.println();
      } else {
        Serial.println("Alarm level out of range!");
      }
      break;

    case 'd':     // set date
    case 'D':
      rtc.set_date(buffer, pnt);
      break;

    case 'f':     // factor reset
    case 'F':
      Serial.println("Resetting configuration to default values ...");
      prefs.putInt(nvs_hpa_offset, DEFAULT_HPA_OFFSET);
      prefs.putInt(nvs_detection_threshold, DEFAULT_DETECTION_THRESHOLD);
      prefs.putInt(nvs_flash_duration, DEFAULT_FLASH_DURATION);
      prefs.putInt(nvs_flash_on, DEFAULT_FLASH_ON);
      prefs.putInt(nvs_alarm_level, DEFAULT_ALARM_LEVEL);
      prefs.putInt(nvs_alarm_window, DEFAULT_ALARM_WINDOW);
      prefs.putInt(nvs_display_timeout, DEFAULT_DISPLAY_TIMEOUT);
      gl_hpa_offset = prefs.getInt(nvs_hpa_offset);
      gl_detection_threshold = prefs.getInt(nvs_detection_threshold);
      gl_flash_duration = prefs.getInt(nvs_flash_duration);
      gl_flash_on = prefs.getInt(nvs_flash_on);
      gl_alarm_level = prefs.getInt(nvs_alarm_level);
      calc_alarm_levels();
      gl_alarm_window = prefs.getInt(nvs_alarm_window);
      gl_display_timeout = prefs.getInt(nvs_display_timeout);
      gl_network_ssid = "";
      prefs.putString(nvs_network_ssid, gl_network_ssid);
      gl_network_password = "";
      prefs.putString(nvs_network_password, gl_network_password);
      gl_mqtt_user = "";
      prefs.putString(nvs_mqtt_user, gl_mqtt_user);
      gl_mqtt_host_ip = "";
      prefs.putString(nvs_mqtt_host_ip, gl_mqtt_host_ip);
      gl_mqtt_password = "";
      prefs.putString(nvs_mqtt_password, gl_mqtt_password);
      gl_mqtt_topic = "";
      prefs.putString(nvs_mqtt_topic, gl_mqtt_topic);
      tft.start();
      print_config();
      break;

    case 'h':     // hpa offset
    case 'H':
      Serial.println("Set hPa (hPa * 10)");
      pnt += 1;
      x = get_int(&pnt);
      if ((x >= 9000) && (x < 12000)) {
        gl_hpa_offset = x * 10 - bmp.readPressure();
        prefs.putInt(nvs_hpa_offset, gl_hpa_offset);
        Serial.print("  hpa offset: "); Serial.println(gl_hpa_offset);
        Serial.println();
      } else {
        Serial.println("Target hPa out of range!");
      }
      break;

    case 'm':     // minimum scale
    case 'M':
      Serial.println("Set minimum scale");
      pnt += 1;
      x = get_int(&pnt);
      if ((x >= 300) && (x < 3000)) {
        gl_scale_min = x;
        prefs.putInt(nvs_scale_min, gl_scale_min);
        Serial.print("  minimum scale: "); Serial.println(gl_scale_min);
        Serial.println();
      } else {
        Serial.println("Minimum scale out of range!");
      }
      break;

    case 'o':     // display timeout
    case 'O':
      Serial.println("Set display timeout");
      pnt += 1;
      x = get_int(&pnt);
      if ((x >= 0) && (x < 60)) {
        gl_display_timeout = x;
        prefs.putInt(nvs_display_timeout, gl_display_timeout);
        Serial.print("  display timeout: "); Serial.println(gl_display_timeout);
        Serial.println();
      } else {
        Serial.println("Display timeout out of range!");
      }
      break;

    case 'r':     // detection threshold
    case 'R':
      Serial.println("Set detection threshold");
      pnt += 1;
      x = get_int(&pnt);
      if ((x >= 25) && (x < 1000)) {
        gl_detection_threshold = x;
        prefs.putInt(nvs_detection_threshold, gl_detection_threshold);
        Serial.print("  detection threshold ('r'): "); Serial.println(gl_detection_threshold);
        Serial.println();
      } else {
        Serial.println("Detection threshold out of range!");
      }
      break;

    case 's':     // flash status
    case 'S':
      Serial.println("Set flash status");
      pnt += 1;
      x = get_int(&pnt);
      if (x > 0) gl_flash_on = 1;
      else gl_flash_on = 0;
      prefs.putInt(nvs_flash_on, gl_flash_on);
      Serial.print("  flash status ('s'): "); if (gl_flash_on == 0) Serial.println("off"); else Serial.println("on");
      break;

    case 't':     // set time
    case 'T':
      rtc.set_time(buffer, pnt);
      break;

    case 'u':     // flash duration
    case 'U':
      Serial.println("Set flash duration");
      pnt += 1;
      x = get_int(&pnt);
      if ((x > 5) && (x < 1000)) {
        gl_flash_duration = x;
        prefs.putInt(nvs_flash_duration, gl_flash_duration);
        Serial.print("  flash duration: "); Serial.println(gl_flash_duration);
        Serial.println();
      } else {
        Serial.println("Flash duration out of range!");
      }
      break;

    case 'w':     // alarm window
    case 'W':
      Serial.println("Set alarm window");
      pnt += 1;
      x = get_int(&pnt);
      if ((x >= 2) && (x < 60)) {
        gl_alarm_window = x;
        prefs.putInt(nvs_alarm_window, gl_alarm_window);
        Serial.print("  alarm window: "); Serial.println(gl_alarm_window);
        Serial.println();
      } else {
        Serial.println("Alarm window out of range!");
      }
      break;

    default:
      Serial.print("Set command not recognized: ");
      Serial.println(buffer);
  }
}

//---------------------------------------------------------------
void CommandLine::decode_get_cmd(uint8_t pnt) {
  extern MyRTC rtc;
  
  char buf[16];

  switch (buffer[pnt]) {
    case 't':           // get time
    case 'T':
      rtc.get_time(buf);
      Serial.println(buf);
      break;

    case 'c':           // get configuration
    case 'C':
      print_config();
      break;

    default:
      Serial.print("Get command not recognized: ");
      Serial.println(buffer);
  }
}

//---------------------------------------------------------------
void CommandLine::decode_network_cmd(uint8_t pnt) {
  extern Preferences prefs;
  extern String gl_network_password, gl_mqtt_host_ip, gl_host_name, gl_mqtt_user, gl_mqtt_password, gl_mqtt_topic;
  extern int gl_mqtt_port;
  char buf[16];
  int x, l;

  switch (buffer[pnt]) {

    case 'c':           // connect to network
    case 'C':
      network_status = network_connect();
      break;

    case 'p':           // set network password
    case 'P':
      pnt += 2;
      while (buffer[pnt] == ' ') pnt += 1;
      buffer[buffer_pnt] = '\0';
      gl_network_password = String(buffer + pnt);
      Serial.print("Network password: >>>"); Serial.print(gl_network_password); Serial.println("<<<");  
      prefs.putString(nvs_network_password, gl_network_password);
      break;

    case 's':           // set network ssid
    case 'S':
      network_scan();
      break;

    case 't':           // set MQTT topic
    case 'T':
      pnt += 2;
      while (buffer[pnt] == ' ') pnt += 1;
      buffer[buffer_pnt] = '\0';
      gl_mqtt_topic = String(buffer + pnt);
      Serial.print("MQTT topic: "); Serial.println(gl_mqtt_topic);  
      prefs.putString(nvs_mqtt_topic, gl_mqtt_topic);
      break;

    case 'h':           // set host name
    case 'H':
      pnt += 2;
      while (buffer[pnt] == ' ') pnt += 1;
      buffer[buffer_pnt] = '\0';
      gl_host_name = String(buffer + pnt);
      Serial.print("Host name: "); Serial.println(gl_host_name);  
      prefs.putString(nvs_host_name, gl_host_name);
      break;

    case 'i':           // set mqtt host ip
    case 'I':
      pnt += 2;
      while (buffer[pnt] == ' ') pnt += 1;
      buffer[buffer_pnt] = '\0';
      gl_mqtt_host_ip = String(buffer + pnt);
      Serial.print("MQTT Host IP: "); Serial.println(gl_mqtt_host_ip);
      prefs.putString(nvs_mqtt_host_ip, gl_mqtt_host_ip);
      break;
 
    case 'o':           // set mqtt port
    case 'O':
      pnt += 1;
      x = get_int(&pnt);
      if (x > 0) {
        gl_mqtt_port = x;
        Serial.print("MQTT port: "); Serial.println(gl_mqtt_port);
        prefs.putInt(nvs_mqtt_port, gl_mqtt_port);
      }
      break;

    case 'u':           // set mqtt user
    case 'U':
      pnt += 2;
      while (buffer[pnt] == ' ') pnt += 1;
      buffer[buffer_pnt] = '\0';
      gl_mqtt_user = String(buffer + pnt);
      Serial.print("MQTT User ID: "); Serial.println(gl_mqtt_user);
      prefs.putString(nvs_mqtt_user, gl_mqtt_user);
      break;
  
    case 'x':           // set mqtt password
    case 'X':
      pnt += 2;
      while (buffer[pnt] == ' ') pnt += 1;
      buffer[buffer_pnt] = '\0';
      gl_mqtt_password = String(buffer + pnt);
      Serial.print("MQTT Password: "); Serial.println(gl_mqtt_password);
      prefs.putString(nvs_mqtt_password, gl_mqtt_password);
      break;

    default:
      Serial.print("Network command not recognized: ");
      if (buffer[pnt] > 0x20) Serial.println(buffer);
      else Serial.println(buffer[pnt-1]);
  }
}

//---------------------------------------------------------------
void CommandLine::decode_cmd(void) {
  uint8_t pnt = 0;

  while (((buffer[pnt] == ' ') || (buffer[pnt] == '\t ')) 
         && (pnt < BUFFER_MAX_LEN - 1)) pnt += 1;
  switch (buffer[pnt]) {
    case 's':
    case 'S':
      decode_set_cmd(pnt + 1);
      break;
    
    case 'g':
    case 'G':
      decode_get_cmd(pnt + 1);
      break;
    
    case 'n':
    case 'N':
      decode_network_cmd(pnt + 1);
      break;
    
    case 'h':   // help
    case 'H':
      show_help();
      break;

    default:
      Serial.print("Command not recognized: ");
      Serial.println(buffer);
  }
}

//---------------------------------------------------------------
// print_config()
// Shows the current configuration via the serial interface
void CommandLine::print_config(void) {
  extern MyRTC rtc;
  extern int gl_hpa_offset, gl_detection_threshold, gl_flash_duration, gl_flash_on, gl_alarm_level, gl_scale_min,
             gl_alarm_window, gl_display_timeout, gl_mqtt_port;
  extern String gl_network_ssid, gl_network_password, gl_host_name, gl_mqtt_host_ip, gl_mqtt_user, 
                gl_mqtt_password, gl_mqtt_topic;
  extern int gl_alarm_1, gl_alarm_2, gl_alarm_3, gl_alarm_4;
  char buf[16];

  Serial.println("Configuration");
  Serial.print("  hpa offset ('h'): "); Serial.println(gl_hpa_offset);
  Serial.print("  detection threshold ('r'): "); Serial.println(gl_detection_threshold);
  Serial.print("  flash duration ('u'): "); Serial.println(gl_flash_duration);
  Serial.print("  flash status ('s'): "); if (gl_flash_on == 0) Serial.println("off"); else Serial.println("on");
  Serial.print("  alarm level ('a'): "); Serial.print(gl_alarm_level);
  Serial.print("  (1: "); Serial.print(gl_alarm_1); Serial.print("  2: "); Serial.print(gl_alarm_2); 
  Serial.print("  3: "); Serial.print(gl_alarm_3); Serial.print("  4: "); Serial.print(gl_alarm_4); Serial.println(")");
  Serial.print("  alarm window ('w'): "); Serial.println(gl_alarm_window); 
  Serial.print("  minimum scale: "); Serial.println(gl_scale_min);
  Serial.print("  display timeout ('o'): "); Serial.println(gl_display_timeout);
  Serial.print("  date ('d'): "); 
  rtc.get_date(buf); Serial.println(buf);
  Serial.print("  time ('t'): ");
  rtc.get_time(buf); Serial.println(buf);
  Serial.print("  network status: "); 
  if (network_status < 0) Serial.println("disabled");
  else if (network_status == 0) Serial.println("no WLAN connection");
  else if (network_status == 1) Serial.println("WLAN okay, MQTT failed");
  else Serial.println("WLAN connected, MQTT connected");
  Serial.print("  network ssid: "); 
  if (gl_network_ssid.length() > 0) Serial.println(gl_network_ssid); else Serial.println("<none>");
  Serial.print("  network password: ");
  if (gl_network_password.length() > 0) Serial.println("<provided>"); else Serial.println("<none>");
  Serial.print("  host name: "); Serial.println(gl_host_name);
  Serial.print("  MQTT host IP: ");
  if (gl_mqtt_host_ip.length() > 0) Serial.println(gl_mqtt_host_ip); else Serial.println("<none>");
  Serial.print("  MQTT port: "); Serial.println(gl_mqtt_port);
  Serial.print("  MQTT user: ");
  if (gl_mqtt_user.length() > 0) Serial.println(gl_mqtt_user); else Serial.println("<none>");
  Serial.print("  MQTT password: ");
  if (gl_mqtt_password.length() > 0) Serial.println("<provided>"); else Serial.println("<none>");
  Serial.print("  MQTT topic: ");
  if (gl_mqtt_topic.length() > 0) Serial.println(gl_mqtt_topic); else Serial.println("<none>");


  Serial.println();
}

//---------------------------------------------------------------
// check_ser_input()
// Checks the serial input for any key stroke. 
// Transfers the characters into the buffer. 
// Executes commands when the <return> key was pressed.
// This function needs to be part of the main loop
void CommandLine::check_ser_input(void) {
  char c;

  while (Serial.available() > 0) {
    c = Serial.read();
    if (c == '\n') {
      buffer[buffer_pnt] = '\n';
      switch (command_line_status) {
        case 0:
          decode_cmd();
          break;
        case 1:
          get_network_ssid();
          break;
        default:
          Serial.println("Comamnd line: internal error!");
          command_line_status = 0;
      }
      buffer_pnt = 0;
    } else {
      if (buffer_pnt < BUFFER_MAX_LEN - 1) {
        buffer[buffer_pnt] = c;
        buffer_pnt += 1;
      }
    }
  }
}

//-------------------------------------------------------------------------
// get_int()
// Extracts an integer from the input buffer. 
// Moves the pointer to the next position after the integer
int32_t CommandLine::get_int(uint8_t *pnt) {
  int32_t result = 0;
  bool negative_flag = false, done = false, valid = false;

  while (buffer[*pnt] == ' ') *pnt += 1;     // skip leading blanks
  
  while (!done) {
    if (buffer[*pnt] == '+') {
      negative_flag = false;
    } else if (buffer[*pnt] == '-') {
      negative_flag = true;
    } else if ((buffer[*pnt] >= '0') && (buffer[*pnt] <= '9')) {
      result = result * 10;
      result += buffer[*pnt] - '0';
      valid = true;
    } else {
      done = true;
    }
    if (buffer[*pnt] != '\0')      // don't progress beyond end of string
      *pnt += 1;
  }
  if (negative_flag) {
    result = -result;
  }
  return result;
}

//-------------------------------------------------------------------------
// get_network_ssid()
void CommandLine::get_network_ssid(void) {
  extern Preferences prefs;
  extern String gl_network_ssid;
  int select;
  uint8_t pnt = 0;
  
  select = get_int(&pnt);
  Serial.println(select);
  if ((select > 0) && (select < 10)) {
    gl_network_ssid = WiFi.SSID(select - 1);
    Serial.print("SSID: ");
    Serial.println(gl_network_ssid);
  } else {
    Serial.println("Cancelled!");
    gl_network_ssid = "";
  }
  prefs.putString(nvs_network_ssid, gl_network_ssid);
  command_line_status = 0;
}

//-------------------------------------------------------------------------
// network_scan()
// Scans the network and lists all available SSIDs
int CommandLine::network_scan(void) {
  int n;

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  Serial.println("Scanning networks ...");
  n = WiFi.scanNetworks();
  if (n == 0) {
    Serial.println("No networks found");
    return 0;
  } else {
    for (int i = 1; i <= n; ++i) {
      // Print SSID and RSSI for each network found
      Serial.print(i);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i-1));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i-1));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " -" : " *");
    }
  }
  Serial.println();
  Serial.print("Enter network number (0 for exit): ");
  command_line_status = 1;
  return n;
}

//-------------------------------------------------------------------------
// network_connect()
// Tries connecting to the WLAN (status -> 1) and to the MQTT broker (status -> 2)
// Updates the network status and refreshes the status on the display
// Returns the resulting network status
int CommandLine::network_connect(void) {
  extern PubSubClient client;
  extern String gl_network_ssid, gl_network_password, gl_mqtt_host_ip;
  extern char gl_host_name_char[], gl_mqtt_user_char[], gl_mqtt_password_char[], gl_mqtt_host_ip_char[];
  extern int gl_mqtt_port;
  extern Display tft;

  // in case the network was disabled, the cancel the action right away
  if (network_status < 0) return network_status;

  // first of all ... reset the network status to 0
  network_status = 0;

  // checking whether ssid and password are given
  tft.show_network_status(network_status);
  Serial.print("Connecting to network ");
  if (gl_network_ssid.length() < 2) {
    Serial.println("\n - SSID missing!");
    network_status = -1;
  }
  if (gl_network_password.length() < 2) {
    Serial.println("\n - password missing!");
    network_status = -1;
  }

  // Switch display on (if ist is not running already)
  tft.start();

  // step 1: connecting to WLAN
  if (network_status >= 0) {
    Serial.print(gl_network_ssid);
    Serial.print(' ');
    WiFi.mode(WIFI_STA);
    WiFi.setHostname(gl_host_name_char);
    WiFi.begin(gl_network_ssid, gl_network_password);
    for (int i = 0; i < 20; ++i) {
      if (WiFi.status() == WL_CONNECTED) break;
      Serial.print('.');
      delay(500);
    }
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("\n - connection failed!");
      tft.show_message("Network failed!");
      network_status = 0;
    } else {
      Serial.print("\n - connected to '");
      Serial.print(gl_network_ssid);
      Serial.println("'");
      Serial.print(" - IP: ");
      Serial.println(WiFi.localIP());
      Serial.println();
      tft.show_message("Network connected");
      network_status = 1;
      delay(5);
    }
    tft.show_network_status(network_status);
  }

  // step 2: connecting to MQTT
  if (network_status >= 1) {
    Serial.println("Connecting to MQTT ...");
    if ((gl_mqtt_host_ip.length() > 5) && (gl_mqtt_port > 0)) {
      Serial.print("MQTT client at "); Serial.print(gl_mqtt_host_ip_char); Serial.print("/"); Serial.println(gl_mqtt_port);
      client.setServer(gl_mqtt_host_ip_char, gl_mqtt_port);
      if (client.connect(gl_host_name_char, gl_mqtt_user_char, gl_mqtt_password_char)) {
        Serial.println("Connected to MQTT broker");
        tft.show_message("MQTT connected");
        network_status = 2;
      } else {
        tft.show_message("MQTT failed!");
        Serial.println("MQTT connection failed!");
        network_status = 1;
      }
    }
    tft.show_network_status(network_status);
    delay(5);
  }

  return network_status;
}

//-------------------------------------------------------------------------
// publish_data
// Tries publishing the data to the MQTT broker
// Updates the network status and refreshes the status on the display
// Returns success (true or false)
bool CommandLine::publish_data(long left_sum, long right_sum, long hpa, int alarm_level) {
  extern PubSubClient client;
  extern char gl_host_name_char[], gl_mqtt_user_char[], gl_mqtt_password_char[], gl_mqtt_topic_char[];
  extern Display tft;
  char buf1[32], buf2[16];

  // is the network connection available? Otherwise cancel immediately
  if (network_status < 2) return false;

  // Try connecting to the MQTT broker
  if (!client.connect(gl_host_name_char, gl_mqtt_user_char, gl_mqtt_password_char)) {
    network_status = 1;
    tft.show_network_status(network_status);
    return false;
  }

  // construct the MQTT message
  delay(5);
  ltoa(left_sum, buf1, 10);
  strcat(buf1, ",");
  ltoa(right_sum, buf2, 10);
  strcat(buf1, buf2);
  strcat(buf1, ",");
  itoa(hpa, buf2, 10);
  strcat(buf1, buf2);
  strcat(buf1, ",");
  itoa(alarm_level, buf2, 10);
  strcat(buf1, buf2);

  // send the message
  if (client.publish(gl_mqtt_topic_char, buf1)) {
    Serial.print("MQTT: "); Serial.print(gl_mqtt_topic_char); 
    Serial.print(' '); Serial.println(buf1);
    return true;
  } else {
    Serial.println("Failed to send data!");
    tft.show_message("MQTT message failed");
    network_status = 1;
    tft.show_network_status(network_status);
    return false;
  }
}

//-------------------------------------------------------------------------
// get_network_status
int CommandLine::get_network_status(void) {
  return network_status;
}

//-------------------------------------------------------------------------
// set_network_status
void CommandLine::set_network_status(int status) {
  network_status = status;
}

//-------------------------------------------------------------------------
// show_help
void CommandLine::show_help(void) {
  Serial.println(title_msg1);
  Serial.println(title_msg2);
  Serial.println("Help");
  Serial.println("Get Commands");
  Serial.println(" - gt <Enter> - get time");
  Serial.println(" - gc <Enter> - get configuration details");
  Serial.println("Set Commands");
  Serial.println(" - sa <int><Enter> - set alaram level (500 ... 50000)");
  Serial.println(" - sd <dd-mm-yy><Enter> - set date");
  Serial.println(" - sf <Enter> - factory reset");
  Serial.println(" - sh <hpa><Enter> - set current air pressure (hPa * 10, 9000 ... 12000");
  Serial.println(" - sm <int><Enter> - set minimum scale for the display (300 ... 3000)");
  Serial.println(" - so <int><Enter> - set display timepout (0 ... 60 minutes)");
  Serial.println(" - sr <int><Enter> - set detetcion threshold (25 ... 1000)");
  Serial.println(" - ss <int><Enter> - set flash status (0 = off, 1 = on)");
  Serial.println(" - st <hh:mm><Enter> - set time");
  Serial.println(" - su <int><Enter> - set flash duration (5 ... 1000 msec)");
  Serial.println(" - sw <int><Enter> - set alarm window (2 ... 60 minutes)");
  Serial.println("Network Commands");
  Serial.println(" - nc <Enter> - connect to network (with given SSID and password)");
  Serial.println(" - nh <host><Enter> - set WLAN host name");
  Serial.println(" - ni <IP><Enter> - set MQTT host IP");
  Serial.println(" - no <int><Enter> - set MQTT port number");
  Serial.println(" - np <password><Enter> - set network password");
  Serial.println(" - ns <Enter> - scan available networks and select one of them");
  Serial.println(" - nt <topic><Enter> - set MQTT topic");
  Serial.println(" - nu <user id><Enter> - set MQTT user ID");
  Serial.println(" - nx <password><Enter> - set MQTT user password");
  Serial.println();
}
