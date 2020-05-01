# 192.168.0.102
from importLibs import *
from WorkingWiyhPhotos import *
from timeTargeting import *
from communicationWithArduino import *


programm_time = HowLong(30, time(), *wath_start_time(),
                        debag=True,
                        name="programm_time")
programm_time.start()

data_pros_ard = DataProcessingForArduino(name="data_pros_ard")
send_for_Arduino = ContactWithArd(pros_ard=data_pros_ard,
                                  name="send_for_Arduino",
                                  debag=True)  # класс для отправки сообщения на ардуину
send_for_Arduino.start()

led2 = ManipulationClass(send_for_Arduino,
                         ['fitoLedHIGH', 'fitoLedLOW'],
                         [3600 * 3, 3600 * 1],
                         name='led2')
poliv_class = ManipulationClass(send_for_Arduino,
                                ['polivHIGH'],
                                [8 * 60],
                                name='poliv_class')
led2.start()
poliv_class.start()

protect_for_yellow = ProtectForYellow(0, name="protect_for_yellow")
protect_for_yellow.start()

print('сейчас будет попытка в передачу фотографий')
try:
    give_neiro = NeiroCetGiveImage(*read_IP_setver_for_text_file(), 60 * 10,
                                   send_for_Arduino,
                                   protect_for_yellow,
                                   name="give_neiro")
    give_neiro.start()
except Exception as e:
    print("передача фоток на сервер не стартовала, из-за отсутствия IP,", e)

while True:
    pass

cap.release()
