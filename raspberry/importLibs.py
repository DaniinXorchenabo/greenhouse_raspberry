from random import shuffle
class TrySerial():
    def __init__(self, way, speed, debag=False):
        self.debag = debag
        self._write = ""

    def write(self, write, encoding='utf-8'):
        self._write = write.decode().strip()
        fixprint("SerialClass printing:", write, test=self.debag)

    def readline(self):
        sleep(10)
        answers = ["I get: " + self._write, 'какая-то строка']
        shuffle(answers)
        return bytes(answers[0], encoding="utf-8")

try:
    from PIL import Image
except Exception as e:
    print('import PIL error', e)

import socket, pickle
import numpy as np
import os
from time import sleep, ctime, time #clock
from itertools import cycle, chain
import glob

import threading
import sys
fixedBags = [True]
fix_func = lambda *a, test=fixedBags: print(*(str(i) for i in a)) if test[0] else a
fixprint= lambda *a, test=False, class_name='':\
    print(("поток " + str(class_name) + ":" \
               if class_name != "" else ""), *(str(i) for i in a)) if test else ""

try:
    from serial import Serial, SerialException
except Exception as e:
    print('import serial error', e)
    Serial = TrySerial
    SerialException = OSError
import cv2