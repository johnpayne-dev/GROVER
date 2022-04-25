import sqlite3

controls_db = '/var/jail/home/team24/chassis/controls.db'

def html_form(up, down, left, right):
    return f'''
            <!DOCTYPE html>
            <html>
                <head>
                    <title>Grover Controls</title>
                </head>
                <body>
                    <h1>UP:</h1>
                    <p id="up">{up}</p>
                    <h1>DOWN:</h1>
                    <p id="down">{down}</p>
                    <h1>LEFT:</h1>
                    <p id="left">{left}</p>
                    <h1>RIGHT:</h1>
                    <p id="right">{right}</p>
                    <script src=controls.js></script>
                </body>
            </html>
            '''

def request_handler(request):
    conn = sqlite3.connect(controls_db)
    c = conn.cursor()
    if request['method'] == 'GET':
        directions = c.execute('''SELECT * FROM controls''').fetchone()
        if request['args'] == ['controls']:
            return ''.join('1' if direction else '0' for direction in directions)
        elif request['args'] == []:
            return html_form(*directions)
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
