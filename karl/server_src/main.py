import sqlite3

conn = sqlite3.connect('controls.db')
c = conn.cursor()
c.execute('''CREATE TABLE IF NOT EXISTS controls (up int, down int, left int, right int);''')
c.execute('''INSERT into controls VALUES (?,?,?,?)''', (0, 0, 0, 0))
conn.commit()
conn.close()