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
        things = c.execute('''SELECT * FROM loc_table  ORDER by time_ DESC;''').fetchone()
        conn.commit() # commit commands
        conn.close() # close connection to database
        return things
    if request['method'] == "POST":
        lat = request['values']['lat']
        lng = request['values']['lng']
        now = datetime.datetime.now()
        example_db = '/var/jail/home/askumar2/sensor/settings.db' # name of database from above
        conn = sqlite3.connect(example_db)  # connect to that database
        c = conn.cursor()  # move cursor into database (allows us to execute commands)
        c.execute('''CREATE TABLE IF NOT EXISTS loc_table (time_ timestamp, lat REAL, lng REAL);''')
        c.execute('''INSERT into loc_table  VALUES (?,?,?);''',(now,lat,lng))
        conn.commit() # commit commands
        conn.close() # close connection to database
        return 'done'