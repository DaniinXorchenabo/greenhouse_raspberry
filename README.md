# greenhouse

код для умной теплицы

Используемое железо:
        
        -> Arduino Mega,
        -> Raspberri Pi B3,
        -> ESP-8266_01,
        -> HC-05 (или HC-06)


Железо, не входящее в состав теплицы на прямую:
        
        -> Какой-нибудь сервер,
        -> Девайс с системой Android,
        -> Любое устройство для входа на сайт


Всё для Raspberry находится в папке: raspberry

Всё для Arduino mega находится в папке: arduino_classes_code_try

Коды для остальных железок отправлю на гитхаб потом


----------------------------------------------------------------------------------

Raspberry Pi

Для запуска кода при старте Raspberry, необходимо поместить файлы:

                .bashrc         (взять из DaniinXorchenabo/greenhouse/raspberry/.bashrc)
                .profile        (взять из DaniinXorchenabo/greenhouse/raspberry/.profile)

в директорию /pi/.../   (в которой находится папка с рабочим столом) (автору: написать имя директории)
если такие файлы там уже есть, то заменить их.

DaniinXorchenabo/greenhouse/raspberry/serverIP.txt - указывается адрес сервера, куда отсыдать фотографии Raspberry
для их анадиза нейросетью на предмет наличия цветения.

DaniinXorchenabo/greenhouse/raspberry/time.txt - задается время, прощедшее с начала выращивания растений в теплице.
Формат:

        *количество дней* *количество часов* *количество минут*
        
файлы:

        auto_start_file_1.service
        auto_start_file.sh

не используются совсем, если автору, конечно, не изменяет память.


Чтобы запустить программу на Raspberry, необходимо запустить файл 

        all_proba9.py

он выполняет функцию main.py.

Роль других файлов:


communicationWithArduino.py - содержит все, что требуется для связи Raspberry с Arduino (со стороны raspberry конечно)

importLibs.py - импорт нужных библиотек

timeTargeting.py - отслеживание времени

WorkingWiyhPhotos.py - работа с фотографиями

Установка нужных библиотек:

        -> Open CV (cv2)        тут потребуется немало колдовства и танцев с бубном, но я взял образ Raspbian с уже установленной OpenCV
        Моя OpenCV установлена в виртуальную среду (cv), если это будет не так, то последние строчки в файлах .bashrc и .profile придется менять
 
 
        pip install numpy
        pip install pyserial 
        (или pip install serial - у меня получилось, что для тестов на Windows - одна, а на Raspberry - другая (какая библиотека куда - не помню))
        
