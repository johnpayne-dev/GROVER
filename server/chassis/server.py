import sqlite3

controls_db = '/var/jail/home/team24/GROVER/server/chassis/controls.db'

def request_handler(request):
    conn = sqlite3.connect(controls_db)
    c = conn.cursor()
    c.execute('''CREATE TABLE IF NOT EXISTS controls (up INT, down INT, left INT, right INT);''')
    directions = c.execute('''SELECT * FROM controls''').fetchone()
    if directions == None:
        c.execute('''INSERT into controls VALUES (?,?,?,?)''', (0, 0, 0, 0))
        directions = (0, 0, 0, 0)
    
    if request['method'] == 'GET':
        return ''.join('1' if direction else '0' for direction in directions)
    elif request['method'] == 'POST':
        for direction in ['up', 'down', 'left', 'right']:
            if direction in request['form']:
                try:
                    c.execute(f'''UPDATE controls SET {direction}=?''', (int(request['form'][direction]),))
                except Exception as e:
                    return str(e)
        conn.commit()
        return 'POST Successful'
    return 'Request not supported\n' + str(request)
