# !/usr/bin/python # -*- coding: utf-8 -*-
import time
import paho.mqtt.client as paho
import mariadb
import os
import sys

time.sleep(10)

mqtt_topic = "gwm/strikes"
mqtt_host = "127.0.0.1"
mqtt_port = 1883
mqtt_user = "gwmv32"
mqtt_password = "<mqtt password>"

mdb_host = "127.0.0.1"
mdb_port = 3306
mdb_db = "gwm"
mdb_user = "gwm_client"
mdb_password = "<mariadb password>"

logging = True
root_dir = os.path.join('/home', 'stela', 'gwm')

t = time.localtime()
log_file_name = "gwm_log_{:04d}-{:02d}-{:02d}-{:02d}-{:02d}.log".format(
                 t[0], t[1], t[2], t[3], t[4])
if logging:
    log_file_path = os.path.join(root_dir, 'mqtt', log_file_name)
    log_file = open(log_file_path, "w", buffering=1)

def on_message(client, userdata, msg):
    if msg.topic == mqtt_topic:
        payload = msg.payload.decode("UTF-8").split(',')
        t = time.localtime()
        t_key = "{:04d}-{:02d}-{:02d} {:02d}-{:02d}-{:02d}".format(
                 t[0], t[1], t[2], t[3], t[4], t[5])  
        sql_statement = "INSERT INTO gwm.strikes VALUES ('"
        if len(payload) < 4:
            if logging:
                log_file.write("Error: did not receive sufficient data!\n")
                log_file.write(str(payload) + "\n")
        else:
            sql_statement += t_key + "', " + payload[0] + ", " + payload[1] + ", " + payload[2] + ", " + payload[3] + ");"
        if logging:
            log_file.write(sql_statement + '\n')
        cur.execute(sql_statement)
        cur.execute("COMMIT;")
        print(sql_statement)
    
    
if __name__ == "__main__":
    # connect to database
    try:
        conn = mariadb.connect(
            user = mdb_user,
            password = mdb_password,
            host = mdb_host,
            port = mdb_port,
            database = mdb_db
        )
    except mariadb.Error as e:
        print(f"Error connecting to MariaDB platform: {e}")
        sys.exit(1)  
    cur = conn.cursor()
    
    mqttc = paho.Client()
    mqttc.on_message = on_message
    mqttc.username_pw_set(mqtt_user, mqtt_password)
    mqttc.connect(mqtt_host, mqtt_port, 60)
    mqttc.subscribe(mqtt_topic, 0)
    
    mqttc.loop_forever()
    
    
