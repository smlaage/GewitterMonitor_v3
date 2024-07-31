import mariadb
import matplotlib
import matplotlib.pyplot as plt
import matplotlib.dates as mdates
import pandas as pd
import numpy as np
import time
import os

# Maria DB connection
mdb_host = "127.0.0.1"
mdb_port = 3306
mdb_db = "gwm"
mdb_user = "gwm_client"
mdb_password = "donald12"

# Image folder
img_folder = os.path.join("/home", "stela", "Python", "gwm_webserver", "static", "images") 
matplotlib.use('Agg')

def get_strike_data(year, month, day):
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
    

def plot_strikes(year, month, day):
    date_str = "{:02d}-{:02d}-{:04d}".format(day, month, year)
    no_data = False
    t = time.localtime()
    this_year, this_month, this_day = t[0], t[1], t[2]

    if day == this_day and month == this_month and year == this_year:
        img_file = "GewitterMonitor_today.png"
    else:
        img_file = "GewitterMonitor_{:04d}{:02d}{:02d}.png".format(year, month, day)
        # Check whether the file is already available in the cache 
        if os.path.exists(os.path.join(img_folder, img_file)):
            print("Image available in cache")  
            return img_file
    
    df = get_strike_data(year, month, day)
    if df is None or len(df) < 5:
        no_data = True
    
    linewidth = 0.0007
    fig, ax = plt.subplots(figsize=(12,7))
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

#========================================

if __name__ == "__main__":
    start_time = time.time()
    t = time.localtime();
    year, month, day = t[0], t[1], t[2]
    # day += 1
    print(plot_strikes(year, month, day))
    print("Duration: ", round(time.time() - start_time, 1))
    