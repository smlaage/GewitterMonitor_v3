"""
gwm_webserver.py

Web server for Gewitter Monitor v3 using Python Flask
Routes:
- /                     system status
- /alarm                plot of alarm levels for recent 60 minutes
- /strikes            	plot of strike sums for the current day
- /strikes/<yyymmdd>	plot of strikes for a given date (year and month may be omitted)

SLW 30-07-2024
"""

from flask import Flask, Response, render_template, request
from markupsafe import escape
import time
import os
import gwm_util

app = Flask(__name__)
IMG_FOLDER = ("images")


@app.route('/')
def index():
    """ Show system status """
    data = gwm_util.get_recent_data(3)
    last_data = data.iloc[-1]
    html = "<t><b>Gewitter-Monitor v3</b></t>"
    html += "<body> <p>System status:</p>"
    html += "<li>Last timestamp: {:s}</li>".format(str(last_data['ts']))
    html += "<li>Strike sum left: {:d}</li>".format(last_data['lstrike'])
    html += "<li>Strike sum right: {:d}</li>".format(last_data['rstrike'])
    html += "<li>hPa: {:0.00f}</li>".format(last_data['hpa'])
    alarm = int(last_data['alarm_level'])
    html += "<li>Alarm status: {:d} ({:s})</li>".format(alarm, gwm_util.alarm_dict.get(alarm, "undefined"))
    html += "<li>Total data count: {:d}</li>".format(gwm_util.get_record_cnt())
    html += "<p>SLW 30-07-2024</p>"
    return html


@app.route('/alarm')
def show_alarm():
    """ Plot alarm data for the last 60 minutes """
    # Detecting device type
    user_agent = request.headers.get('User-Agent')
    user_agent = user_agent.lower()
    mobile_device = False
    if ("iphone" in user_agent) or ("android" in user_agent):
        mobile_device = True
        
    # Plotting alarm data
    img_file = gwm_util.plot_alarm()
    img_path = os.path.join('images', img_file)
    return render_template("index.html", gwm_image=img_path)


@app.route('/strike')
@app.route('/strikes')
def show_strikes_today():
    """ Plot strike data for the current day """
    # Detecting device type
    user_agent = request.headers.get('User-Agent')
    user_agent = user_agent.lower()
    mobile_device = False
    if ("iphone" in user_agent) or ("android" in user_agent):
        mobile_device = True
        
    # Plotting strike data for today        
    t = time.localtime()
    img_file = gwm_util.plot_strikes(t[0], t[1], t[2], mobile_device)
    img_path = os.path.join('images', img_file)
    return render_template("index.html", gwm_image=img_path)
    

@app.route('/strike/<string:date_str>')
@app.route('/strikes/<string:date_str>')
def show_strikes_per_date(date_str):
    """ Plot strike data for a given day """
    # Detecting device type
    user_agent = request.headers.get('User-Agent')
    user_agent = user_agent.lower()
    mobile_device = False
    if ("iphone" in user_agent) or ("android" in user_agent):
        mobile_device = True
    
    error, year, month, day = gwm_util.extract_date(escape(date_str))
    if len(error) > 0:
        html = "<p>" + "=== Error: " + error + " ===</p>"
        return html

    img_file = gwm_util.plot_strikes(year, month, day, mobile_device)  
    img_path = os.path.join('images', img_file)
    return render_template("index.html", gwm_image=img_path)


#=======================================================================
if __name__ == "__main__":
    
    # start the web service
    app.run(debug=False, port=80, host='192.168.1.249')
    