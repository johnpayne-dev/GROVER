import sqlite3
import datetime
import json

db_path = "/var/jail/home/team24/GROVER/server/audio/from_controller_database.db"
def request_handler(request):
    # return request
    conn = sqlite3.connect(db_path)
    c = conn.cursor()
    c.execute('''CREATE TABLE IF NOT EXISTS from_controller_table (encoded_audio text, time timestamp);''')
    c.execute('''CREATE TABLE IF NOT EXISTS is_new_table (new bool, time timestamp);''')
    
    now = datetime.datetime.now()
    if request["method"] == "GET":
        return c.execute('''SELECT * FROM from_controller_table''').fetchall()

    elif request["method"] == "POST":
        if "new" in request["args"]:
            try:
                new = c.execute('''SELECT * FROM is_new_table ORDER BY time DESC''').fetchone()[0]
            except TypeError:
                new = False

            if new:
                c.execute('''INSERT into is_new_table VALUES (?,?);''',(False, now))
                new_audio = c.execute('''SELECT * FROM from_controller_table ORDER BY time DESC''').fetchone()[0]
                conn.commit()
                conn.close()
                return new_audio
            else:
                return "!"
        else:
            try:
                encoding = json.loads(request["data"])["encoding"]
            except:
                try:
                    encoding = request["form"]["encoding"]
                except:
                    return "Request must contain encoding field."

            c.execute('''INSERT into from_controller_table VALUES (?,?);''',(encoding, now))
            c.execute('''INSERT into is_new_table VALUES (?,?);''',(True, now))
            conn.commit()
            conn.close()

            return "Successfully Posted Audio"
    else:
        return "Only GET and POST requests are accepted."
