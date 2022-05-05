import sqlite3
import datetime
import requests
import binascii
import PIL.Image
import io

db = "/var/jail/home/team24/GROVER/server/camera/grover.db"


def create_database():
    conn = sqlite3.connect(db)  # connect to that database (will create if it doesn't already exist)
    c = conn.cursor()  # move cursor into database (allows us to execute commands)
    c.execute('''CREATE TABLE IF NOT EXISTS image_table (image text, time timestamp);''') # run a CREATE TABLE command
    conn.commit() # commit commands (VERY IMPORTANT!!)
    conn.close() # close connection to database

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

def get_picture(data = None):
    if data is None:
        image_data = requests.get('''http://www.608dev-2.net/sandbox/sc/team24/GROVER/server/camera/server.py''')
        image_data = image_data.text.strip()
    else:
        print("Given data")
        image_data = data

    try:
        
        picture_bytes = binascii.unhexlify(image_data)
        picture_stream = io.BytesIO(picture_bytes)
        picture = PIL.Image.open(picture_stream)
        # print(list(picture.getdata()))
        return picture
        picture.show()
    except:
        print("Incorrect image format")
        

def request_handler(request):
    if request["method"] == "GET":
        # try:
        #     request['values']
        # except Exception as e:
        #     return e

        create_database()
        image = get_info()

        if not image:
            return 0
        else:
            return image[0]
            image = get_picture(image[0])
            image.save("/var/jail/home/team24/GROVER/server/camera/grover_image.jpeg")
            return "<img src = http://608dev-2.net/sandbox/sc/team24/GROVER/server/camera/grover_image.jpeg alt = \"GetImageError\"> </img> "
        
    if request["method"] == "POST":
        if request["content-type"] == "application/x-www-form-urlencoded":
            image_data = ""
            try:
                image_data = request["form"]["data"].replace(' ', '+')
                image = open('/var/jail/home/team24/GROVER/server/camera/grover_image.jpeg', 'wb')
                image.write(base64.b64decode(image_data))
                image.close()
                return image_data
            except Exception as e:
                return str(e)
        else:
            return "Check incorrect content-type"
            


            













            
        
        
    







    
