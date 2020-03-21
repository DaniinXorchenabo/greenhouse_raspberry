# 192.168.0.102
from importLibs import *
from WorkingWiyhPhotos import *
from timeTargeting import *
from communicationWithArduino import *


d, h, m = wath_start_time()
start_time_str = ctime()
start_time = time()
programm_time = HowLong(30, start_time, d, h, m,
                        debag=True,
                        name="programm_time")
programm_time.start()
data_pros_ard = DataProcessingForArduino(name="data_pros_ard")
send_for_Arduino = ContactWithArd(pros_ard=data_pros_ard,
                                  name="send_for_Arduino",
                                  debag=True) #класс для отправки сообщения на ардуину
send_for_Arduino.start()
sleep(1)

led2 = ManipulationClass(send_for_Arduino,
                         ['WhiteLedHIGH', 'WhiteLedLOW'],
                         [2 * 3600, 4 * 3600],
                         name='led2')
poliv_class = ManipulationClass(send_for_Arduino,
                                ['polivHIGH'],
                                [8 * 60],
                                name='poliv_class')
led2.start()
sleep(1)
poliv_class.start()
a = [[(26, 50, 50), (85, 255, 255)],
     [(17, 100, 150), (24, 255, 255)]]  # интервал определения жёлтого

"""
cv2.namedWindow("frame")  # создаем главное окно
try:
    cap = cv2.VideoCapture(0)
except Exception:
    try:
        cap = cv2.VideoCapture(-1)
    except Exception:
        cap = cv2.VideoCapture(0)
print('камера подключена')
for i in range(30):
    _, img = cap.read()
cap.release()
porog_yellow = 20
ch1 = 0
print('камера прогрета')

# cap = cv2.VideoCapture(0)
# out = cv2.VideoWriter(filename, get_video_type(filename), 25, get_dims(cap, res)) #
# print('видио инициализированно')
error_time = -1
vixod = ''
adres, port = read_IP_setver_for_text_file()  # input('введите адрес сервера (при запуске сервера он печатается)\nв виде: 192.168.0.0\n')
protect_for_yellow = ProtectForYellow(0, name="protect_for_yellow")

  #
protect_for_yellow.start()
sleep(0.5)
print('сейчас будет попытка в передачу фотографий')
if False and adres:
    give_neiro = NeiroCetGiveImage(adres, port, 60*10,
                                   send_for_Arduino,
                                   protect_for_yellow,
                                   name="give_neiro")
    give_neiro.start()
else:
    print("передача фоток на сервер не стартовала, из-за отсутствия IP")
"""


while True:
    pass

cap.release()
