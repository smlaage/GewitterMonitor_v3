""" gwm_util.py
    SLW 2024-07-30
"""

# Maria DB connection
mdb_host = "127.0.0.1"
mdb_port = 3306
mdb_db = "gwm"
mdb_user = "gwm_client"
mdb_password = "<mariadb_password>"

# Image folder
root_dir = os.path.join('/home', 'stela', 'gwm')
img_folder = os.path.join(root_dir, "webserver", "static", "images") 
matplotlib.use('Agg')

import mariadb
import matplotlib
import matplotlib.pyplot as plt
import matplotlib.dates as mdates
from matplotlib.collections import LineCollection
import pandas as pd
import numpy as np
import time
import datetime
import os


alarm_dict = {
    0: "no alarm",
    1: "display on",
    2: "green/yellow",
    3: "yellow",
    4: "yellow/red",
    5: "red"
}

alarm_colors = {
    0 : "darkolivegreen",
    1 : "forestgreen",
    2 : "yellowgreen",
    3 : "gold",
    4 : "darkorange",
    5 : "crimson"
}

SIZE_DESKTOP_X, SIZE_DESKTOP_Y = 12.0, 5.5
SIZE_MOBILE_X, SIZE_MOBILE_Y = 8.0, 3.67


def extract_date(date_str):
    """ Analyzes a given string for a date.
        Returns error: str, either error message or empty in case of no errors
                year: int, month: int, day: int
    """
    error = ""
    t = time.localtime()
    year, month, day = t[0], t[1], t[2]
    if date_str.casefold() == "today".casefold():
        pass
    elif date_str.isdecimal():
        if len(date_str) == 8:
            year = int(date_str[0:4])
            month = int(date_str[4:6])
            day = int(date_str[6:8])
        elif len(date_str) == 6:
            year = 2000 + int(date_str[0:2])
            month = int(date_str[2:4])
            day = int(date_str[4:6])
        elif len(date_str) == 4:
            month = int(date_str[0:2])
            day = int(date_str[2:4])
        elif len(date_str) == 2:
            day = int(date_str)
        else:
            error = "invalid length of date"
    else:
        error = "invalid date format"           
            
    if len(error) == 0:
        # Check whether the date is valid
        try:
            datetime.datetime(year, month, day)
            okay = True
        except ValueError:
            error = "invalid date"
    return error, year, month, day


def get_record_cnt():
    """ Returns the total number of records in the database """
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
    sql = "SELECT count(*) FROM gwm.strikes;"
    cur.execute(sql)
    for x in cur:
        cnt = x[0]
    conn.close()
    return cnt


def get_recent_data(cnt=120):
    """ Retrieves the most recent data from the database """
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
        print("Error connecting to MariaDB platform: {:s}".format(e))
        return None
    # Retrieve data
    cur = conn.cursor()
    # Get data for most recent data
    sql = "SELECT * FROM gwm.strikes ORDER BY ts DESC LIMIT {:d};".format(cnt)
    # print(sql)
    cur.execute(sql)
    table = cur.fetchall()
    if table:
        df = pd.DataFrame(table)
        df.columns = ["ts", "lstrike", "rstrike", "hpa", "alarm_level"]
        df['ts'] = pd.to_datetime(df['ts'], format="%Y-%M-%D %H:%M:%S")
        df['hpa'] = df['hpa'].replace(0, np.nan)
        df['hpa'] = df['hpa'] / 100
        return df
    else:
        return None
    

def get_data(year, month, day):
    """ Retrieves strike data from database for a given day """
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
        print("Error connecting to MariaDB platform: {:s}".format(e))
        return None
    # Retrieve data
    cur = conn.cursor()
    sql = "SELECT * FROM gwm.strikes WHERE DATE(ts) = '{:04d}-{:02d}-{:02d}';".format(year, month, day)
    cur.execute(sql)
    table = cur.fetchall()
    if table:
        df = pd.DataFrame(table)
        df.columns = ["ts", "lstrike", "rstrike", "hpa", "alarm_level"]
        df['ts'] = pd.to_datetime(df['ts'], format="%Y-%M-%D %H:%M:%S")
        df['hpa'] = df['hpa'].replace(0, np.nan)
        df['hpa'] = df['hpa'] / 100
        return df
    else:
        return None
    

def plot_strikes(year, month, day, mobile = False):
    date_str = "{:02d}-{:02d}-{:04d}".format(day, month, year)
    no_data = False
    t = time.localtime()
    this_year, this_month, this_day = t[0], t[1], t[2]

    if day == this_day and month == this_month and year == this_year:
        img_file = "gwm_strikes_today.png"
    else:
        img_file = "gwm_strikes_{:s}_{:04d}{:02d}{:02d}.png".format("small" if mobile else "big", year, month, day)
        # Check whether the file is already available in the cache 
        if os.path.exists(os.path.join(img_folder, img_file)):
            print("Image available in cache")  
            return img_file
    
    df = get_data(year, month, day)
    if df is None or len(df) < 5:
        no_data = True
    
    linewidth = 0.0007
    if mobile:
        width, height = SIZE_MOBILE_X, SIZE_MOBILE_Y
    else:
        width, height = SIZE_DESKTOP_X, SIZE_DESKTOP_Y
    fig, ax = plt.subplots(figsize=(width, height))
    ax.xaxis.set_major_formatter(mdates.DateFormatter('%H:%M'))
    if not no_data:
        ax.bar(df['ts'], df['lstrike'], width=linewidth, color='red')
        ax.bar(df['ts'], df['rstrike'], bottom=df['lstrike'], width=linewidth, color='green')
    else:
        ax.set_ylim(0, 1000)
    ax.set_ylabel("Strike Sum / min")
    ax.grid(color='grey', linestyle='dotted', linewidth=0.5)
    start = "{:04d}-{:02d}-{:02d} 00:00:00".format(year, month, day)
    end = "{:04d}-{:02d}-{:02d} 23:59:59".format(year, month, day)
    ax.set_xlim(pd.to_datetime(start), pd.to_datetime(end))

    ax2 = ax.twinx()
    if not no_data:
        ax2.plot(df['ts'], df['hpa'], color='blue', linewidth=0.5)
        hpa_min, hpa_max = df['hpa'].min(), df['hpa'].max()
        hpa_mid = round((hpa_max + hpa_min) / 2)
        if hpa_max - hpa_min < 4:
            ax2.set_ylim(hpa_mid - 2, hpa_mid + 2)
    else:
        ax2.set_ylim(1010, 1014)
    ax2.set_ylabel("hPa")
    
    ax.text(0.01, 0.98, "Date: " + date_str, ha='left', va='top', transform=ax.transAxes) 
    
    fig.tight_layout()
    plt.savefig(os.path.join(img_folder, img_file))
    return img_file


def plot_alarm(mobile = False):
    # define operating values
    no_data = False
    img_file = "alarm_plot.png"
    df = get_recent_data()
    if df is None or len(df) < 2:
        no_data = True
    if mobile:
        width, height = SIZE_MOBILE_X, SIZE_MOBILE_Y
    else:
        width, height = SIZE_DESKTOP_X, SIZE_DESKTOP_Y
    alarm_linewidth = 6
    # plot frame and axis
    fig, ax = plt.subplots(figsize=(width, height))
    ax.xaxis.set_major_formatter(mdates.DateFormatter('%H:%M'))
    ax.set_ylabel("Strike Sum / min")
    ax2 = ax.twinx()
    ax2.set_ylim(-0.5, 5.5)
    ax2.grid(linewidth=0.5, color='lightgrey')
    ax2.set_ylabel("Alarm Level")
    # plot data
    if not no_data:
        # plot strike data
        ax.bar(df['ts'], df['lstrike'], width=0.0005, color='red', alpha=0.3)
        ax.bar(df['ts'], df['rstrike'], bottom=df['lstrike'], width=0.0005, color='green', alpha=0.4)
        # plot alarm level data with varying colors
        df['color'] = df['alarm_level'].map(alarm_colors)
        alarm_level = df.iloc[0]['alarm_level']
        start_ts = df.iloc[0]['ts']
        for idx, row in df.iloc[1:].iterrows():
            if row['alarm_level'] != alarm_level:
                ax2.plot((start_ts, start_ts), (alarm_level, row['alarm_level']),
                         color = 'grey', linewidth=1)
                ax2.plot((start_ts, row['ts']), (row['alarm_level'], row['alarm_level']),
                        color=alarm_colors[row['alarm_level']], linewidth=alarm_linewidth)
                alarm_level = row['alarm_level']
                start_ts = row['ts']
        ax2.plot((start_ts, row['ts']), (alarm_level, alarm_level),
                 color=alarm_colors[alarm_level], linewidth=alarm_linewidth)
    else:
        ax.set_ylim(0, 1000)
        now = pd.Timestamp.now()
        ax2.plot((now - pd.Timedelta(hours=1), now), (0, 0), color='grey', linewidth=1)
        ax2.text(0.5, 0.2, "No data!", ha='center', va='center', transform=ax.transAxes) 
    # write annotations
    t = time.localtime()
    date_str = "{:02d}-{:02d}-{:04d}".format(t[2], t[1], t[0])
    ax.text(0.01, 0.98, "Date: " + date_str, ha='left', va='top', transform=ax.transAxes)
    # finish picture
    fig.tight_layout()
    plt.savefig(os.path.join(img_folder, img_file))
    return img_file
    

#========================================

if __name__ == "__main__":
    start_time = time.time()
    plot_alarm()
    print("Duration: ", round(time.time() - start_time, 1))
    
    """
    
    t = time.localtime();
    year, month, day = t[0], t[1], t[2]
    day -= 1
    print(plot_strikes(year, month, day, False))
    
    """
