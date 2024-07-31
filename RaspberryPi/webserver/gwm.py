from flask import Flask, Response, render_template, request
from markupsafe import escape
import mariadb
import time
import os
import pandas as pd
import plot_gwm_data
import datetime

mdb_host = "127.0.0.1"
mdb_port = 3306
mdb_db = "gwm"
mdb_user = "gwm_client"
mdb_password = "donald12"

app = Flask(__name__)
IMG_FOLDER = ("images")


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


@app.route('/', methods=['GET', 'POST'])
def index():
    # Plotting strike data for today
    t = time.localtime()
    img_file = plot_gwm_data.plot_strikes(t[0], t[1], t[2])
    img_path = os.path.join('images', img_file)
    return render_template("index.html", gwm_image=img_path)


@app.route('/date/<string:date_str>')
def show_date(date_str):
    error, year, month, day = extract_date(escape(date_str))
    if len(error) > 0:
        html = "<p>" + "=== Error: " + error + " ===</p>"
        return html

    img_file = plot_gwm_data.plot_strikes(year, month, day)  
    img_path = os.path.join('images', img_file)
    return render_template("index.html", gwm_image=img_path)


#=======================================================================
if __name__ == "__main__":
    
    # start the web service
    # app.run(debug=True, port=80, host='192.168.1.252')
    app.run(debug=False, port=80, host='192.168.1.252')
    