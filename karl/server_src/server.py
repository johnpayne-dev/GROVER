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
                    <script>
                        document.addEventListener("keydown", (e) => {{
                            switch(e.key) {{
                                case "ArrowUp":
                                    if(document.getElementById("up").innerText == "0") {{
                                        document.getElementById("up").innerText = "1";
                                        let xhr = new XMLHttpRequest();
                                        xhr.open("POST", "https://608dev-2.net/sandbox/sc/team24/chassis/server.py");
                                        xhr.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
                                        xhr.send("up=1");
                                    }}
                                    break;
                                case "ArrowDown":
                                    if(document.getElementById("down").innerText == "0") {{
                                        document.getElementById("down").innerText = "1";
                                        let xhr = new XMLHttpRequest();
                                        xhr.open("POST", "https://608dev-2.net/sandbox/sc/team24/chassis/server.py");
                                        xhr.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
                                        xhr.send("down=1");
                                    }}
                                    break;
                                case "ArrowLeft":
                                    if(document.getElementById("left").innerText == "0") {{
                                        document.getElementById("left").innerText = "1";
                                        let xhr = new XMLHttpRequest();
                                        xhr.open("POST", "https://608dev-2.net/sandbox/sc/team24/chassis/server.py");
                                        xhr.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
                                        xhr.send("left=1");
                                    }}
                                    break;
                                case "ArrowRight":
                                    if(document.getElementById("right").innerText == "0") {{
                                        document.getElementById("right").innerText = "1";
                                        let xhr = new XMLHttpRequest();
                                        xhr.open("POST", "https://608dev-2.net/sandbox/sc/team24/chassis/server.py");
                                        xhr.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
                                        xhr.send("right=1");
                                    }}
                                    break;
                            }}
                        }});
                        document.addEventListener("keyup", (e) => {{
                            switch(e.key) {{
                                case "ArrowUp":
                                    if(document.getElementById("up").innerText == "1") {{
                                        document.getElementById("up").innerText = "0";
                                        let xhr = new XMLHttpRequest();
                                        xhr.open("POST", "https://608dev-2.net/sandbox/sc/team24/chassis/server.py");
                                        xhr.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
                                        xhr.send("up=0");
                                    }}
                                    break;
                                case "ArrowDown":
                                    if(document.getElementById("down").innerText == "1") {{
                                        document.getElementById("down").innerText = "0";
                                        let xhr = new XMLHttpRequest();
                                        xhr.open("POST", "https://608dev-2.net/sandbox/sc/team24/chassis/server.py");
                                        xhr.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
                                        xhr.send("down=0");
                                    }}
                                    break;
                                case "ArrowLeft":
                                    if(document.getElementById("left").innerText == "1") {{
                                        document.getElementById("left").innerText = "0";
                                        let xhr = new XMLHttpRequest();
                                        xhr.open("POST", "https://608dev-2.net/sandbox/sc/team24/chassis/server.py");
                                        xhr.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
                                        xhr.send("left=0");
                                    }}
                                    break;
                                case "ArrowRight":
                                    if(document.getElementById("right").innerText == "1") {{
                                        document.getElementById("right").innerText = "0";
                                        let xhr = new XMLHttpRequest();
                                        xhr.open("POST", "https://608dev-2.net/sandbox/sc/team24/chassis/server.py");
                                        xhr.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
                                        xhr.send("right=0");
                                    }}
                                    break;
                            }}
                        }});
                    </script>
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