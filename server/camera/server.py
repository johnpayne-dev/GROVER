import sqlite3
import datetime
import requests

db = "/var/jail/home/aponce/grover_test.db"


def create_database():
    conn = sqlite3.connect(db)  # connect to that database (will create if it doesn't already exist)
    c = conn.cursor()  # move cursor into database (allows us to execute commands)
    c.execute('''CREATE TABLE IF NOT EXISTS image_table (image text, time timestamp);''')  # run a CREATE TABLE command
    conn.commit()  # commit commands (VERY IMPORTANT!!)
    conn.close()  # close connection to database


def insert_into_database(image_data):
    conn = sqlite3.connect(db)
    c = conn.cursor()
    # sql_command = '''INSERT into test_table VALUES ('{}',{}, {}, {});'''.format(user,lat, lon, area, datetime.datetime.now())
    # c.execute(sql_command)
    c.execute('''INSERT into image_table VALUES (?,?);''', (image_data, datetime.datetime.now()))
    conn.commit()
    conn.close()


def get_info():
    conn = sqlite3.connect(db)
    c = conn.cursor()
    # things = c.execute('''SELECT * FROM user_table WHERE time > ;''').fetchall()
    ex = c.execute('''SELECT * FROM image_table ORDER BY time DESC;''').fetchone()
    return ex
    conn.commit()
    conn.close()



def request_handler(request):
    if request["method"] == "GET":

        create_database()
        image = get_info()

        if not image:
            return 0
        else:
            return image[0]

    if request["method"] == "POST":
        if request["content-type"] == "application/x-www-form-urlencoded":
            image_data = ""
            try:
                image_data = request["form"]["data"]
            except Exception as e:
                # return e here or use your own custom return message for error catch
                # be careful just copy-pasting the try except as it stands since it will catch *all* Exceptions not just ones related to number conversion.
                return e

            create_database()

            insert_into_database(image_data)

            return "Succesffully saved image"
        else:

            return "Check incorrect content-type"





























