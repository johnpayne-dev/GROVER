import sqlite3
import datetime
from bokeh.plotting import figure
from bokeh.embed import components
ht_db = '/var/jail/home/team24/GROVER/server/sensors/settings.db' #assumes you have a lab07a dir in your home dir
plot1 = figure() 
plot2 = figure()
plot3 = figure() 
plot4 = figure()
plot5 = figure() 
plot6 = figure()
plot7 = figure() 
plot8 = figure()
plot9 = figure() 
plot10 = figure()
def request_handler(request):
    if request['method'] =="GET":
        with sqlite3.connect(ht_db) as c:
            alt = c.execute('''SELECT alt FROM deriv_table ORDER by time_ ASC;''').fetchall()[-10:]
            vx = c.execute('''SELECT vx FROM deriv_table ORDER by time_ ASC;''').fetchall()[-10:]
            vy = c.execute('''SELECT vy FROM deriv_table ORDER by time_ ASC;''').fetchall()[-10:]
            ti = c.execute('''SELECT time_ FROM deriv_table ORDER by time_ ASC;''').fetchall()[-10:]
            temp = c.execute('''SELECT temp FROM final_table ORDER by time_ ASC;''').fetchall()[-10:]
            pr = c.execute('''SELECT pressure FROM final_table  ORDER by time_ ASC;''').fetchall()[-10:]
            accx =  c.execute('''SELECT x FROM final_table  ORDER by time_ ASC;''').fetchall()[-10:]
            tim = c.execute('''SELECT time_ FROM final_table ORDER by time_ ASC;''').fetchall()[-10:]
            accy = c.execute('''SELECT y FROM final_table ORDER by time_ ASC;''').fetchall()[-10:]
            gyrox = c.execute('''SELECT gyrox FROM final_table ORDER by time_ ASC;''').fetchall()[-10:]
            gyroy =  c.execute('''SELECT gyroy FROM final_table ORDER by time_ ASC;''').fetchall()[-10:]
            sonar = c.execute('''SELECT Sonar_distance FROM final_table ORDER by time_ ASC;''').fetchall()[-10:]
            #time_ timestamp, temp REAL,pressure REAL, x REAL, y REAL, sound REAL,gyrox REAL, gyroy REAL, Sonar_distance REAL,
            final = []
            tf = []
            pres= []
            ac = []
            acc = []
            gyro1 = []
            gyro2 = []
            son = []
            al = []
            v1 = []
            v2 = []
            tt = []
            for j in tim:
                dto = datetime.datetime.strptime(j[0],'%Y-%m-%d %H:%M:%S.%f')
                final.append(dto)
            for lp in ti:
                dto = datetime.datetime.strptime(lp[0],'%Y-%m-%d %H:%M:%S.%f')
                tt.append(dto)
            for k in temp:
                tf.append(k[0])
            for y in pr:
                pres.append(y[0])
            for l in accx:
                ac.append(l[0])
            for w in accy:
                acc.append(w[0])
            for q in gyrox:
                gyro1.append(q[0])
            for i in gyroy:
                gyro2.append(i[0])
            for ty in sonar:
                son.append(ty[0])
            for lm in alt:
                al.append(lm[0])
            for g in vx:
                v1.append(g[0])
            for e in vy:
                v2.append(e[0])
            
        plot1.line(final, tf, legend_label= "temperature", line_dash=[4, 4], line_color="red", line_width=2)
        plot2.line(final, pres, legend_label= "pressure", line_dash=[4, 4], line_color="orange", line_width=2)
        plot3.line(final, ac, legend_label= "accelx", line_dash=[4, 4], line_color="blue", line_width=2)
        plot4.line(final, acc, legend_label= "accely", line_dash=[4, 4], line_color="blue", line_width=2)
        plot5.line(final, gyro1,legend_label= "angular velocity x", line_dash=[4, 4], line_color="black", line_width=2)
        plot6.line(final, gyro2, legend_label= "angular velocity y",line_dash=[4, 4], line_color="black", line_width=2)
        plot7.line(final, son,legend_label= "sonar", line_dash=[4, 4], line_color="yellow", line_width=2)
        plot8.line(tt,al ,legend_label= "altitude", line_dash=[4, 4], line_color="black", line_width=2)
        plot9.line(tt, v1, legend_label= "velocity x",line_dash=[4, 4], line_color="black", line_width=2)
        plot10.line(tt, v2,legend_label= "velocity y", line_dash=[4, 4], line_color="red", line_width=2)

        from bokeh.embed import components
        script1, div1 = components(plot1)
        script2, div2 = components(plot2)
        script3, div3 = components(plot3)
        script4, div4 = components(plot4)
        script5, div5 = components(plot5)
        script6, div6 = components(plot6)
        script7, div7 = components(plot7)
        script8, div8 = components(plot8)
        script9, div9 = components(plot9)
        script10, div10 = components(plot10)
        return f'''
            {div1}
            {script1}
            {div2}
            {script2}
            {div3}
            {script3}
            {div4}
            {script4}
            {div5}
            {script5}
            {div6}
            {script6}
            {div7}
            {script7}
            {div8}
            {script8}
            {div9}
            {script9}
            {div10}
            {script10}
        '''   
    else:
      return "invalid HTTP method for this url."
