try:
    from PIL import Image
except Exception as e:
    print('import PIL error', e)
from random import shuffle
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
    '''
    class Serial():
        def __init__(self, way, speed, debag=False):
            self.debag = debag

        def write(self, write, encoding='utf-8'):
            fixprint("SerialClass printing:", write, test=self.debag)
    '''
import cv2