import requests
import sqlite3
import datetime
from bokeh.plotting import figure
from bokeh.embed import components
example_db = '/var/jail/home/askumar2/sensor/settings.db' # name of database from above
def request_handler(request):
    if request['method'] == "GET":
        example_db = '/var/jail/home/askumar2/sensor/settings.db' # name of database from above
        conn = sqlite3.connect(example_db)  # connect to that database
        c = conn.cursor()  # move cursor into database (allows us to execute commands)
        things = c.execute('''SELECT * FROM final_table  ORDER by time_ ASC;''').fetchall()
        thing2 = c.execute('''SELECT * FROM deriv_table  ORDER by time_ ASC;''').fetchall()
        ting2 = thing2[-10:]
        conn.commit() # commit commands
        conn.close() # close connection to database
        thing = things[-10:]
        final = []
        return f"{thing} \n {ting2}"
    if request['method'] == "POST":
        temp = request['values']['temp']
        pressure = request['values']['press']
        x = request['values']['accelx']
        y = request['values']['accely']
        sound = request['values']['sound']
        a = request['values']['a']
        b = request['values']['b']
        sonar = request['values']['dis']
        alt = request['values']['alt']
        vx = request['values']['vx']
        vy = request['values']['vy']
        now = datetime.datetime.now()
        example_db = '/var/jail/home/askumar2/sensor/settings.db' # name of database from above
        conn = sqlite3.connect(example_db)  # connect to that database
        c = conn.cursor()  # move cursor into database (allows us to execute commands)
        c.execute('''CREATE TABLE IF NOT EXISTS deriv_table (time_ timestamp, alt REAL, vx REAL, vy REAL);''')
        c.execute('''INSERT into deriv_table  VALUES (?,?,?,?);''',(now,alt,vx,vy))
        c.execute('''CREATE TABLE IF NOT EXISTS final_table (time_ timestamp, temp REAL,pressure REAL, x REAL, y REAL, sound REAL,gyrox REAL, gyroy REAL, Sonar_distance REAL);''') # run a CREATE TABLE command
        c.execute('''INSERT into final_table  VALUES (?,?,?,?,?,?,?,?,?);''',(now,temp,pressure,x,y,sound,a,b,sonar))
        things = c.execute('''SELECT * FROM final_table''').fetchall()
        conn.commit() # commit commands
        conn.close() # close connection to database
        return 'done'
