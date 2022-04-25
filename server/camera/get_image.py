import os
import math

import binascii
import PIL.Image
import io

import requests


def get_picture(data=None):
    if data is None:
        image_data = requests.get('''http://www.608dev-2.net/sandbox/sc/aponce/grover_server.py''')
        image_data = image_data.text.strip()
    else:
        print("Given data")
        image_data = data

    try:

        picture_bytes = binascii.unhexlify(image_data)
        picture_stream = io.BytesIO(picture_bytes)
        picture = PIL.Image.open(picture_stream)
        #Gets rgb values
        # print(list(picture.getdata()))
        picture.show()
    except:
        print("Incorrect image format")

get_picture()