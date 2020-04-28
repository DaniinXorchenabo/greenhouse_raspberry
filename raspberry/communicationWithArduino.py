from importLibs import *


class ManipulationClass(threading.Thread):
    def __init__(self, uno_class, signals,
                 intervals,
                 name='ManipulationClass',
                 interval_updates=60,
                 debag=False):

        threading.Thread.__init__(self)
        self.intervals = cycle(iter(intervals))
        self.signals = cycle(iter(signals))
        self.uno_class = uno_class
        self.ckass_name = name
        self.prnt = lambda *cont: fixprint(*cont, test=debag, class_name=name)
        self.live_potok = True
        self.stek = 0
        self.time_f = 0
        self.interval_updates = interval_updates
        self.debag = debag

    def volue_interval(self, intervals):
        self.intervals = cycle(iter(intervals))

    def run(self):
        self.time_f = int(time())

        now_signal = next(self.signals)
        now_interval = next(self.intervals)
        self.uno_class.send_to_port(now_signal)
        while self.live_potok:
            if int(time()) - self.time_f - 1 >= now_interval:
                fixprint("Вроде отправка", now_signal, test=self.debag)
                self.uno_class.send_to_port(now_signal)  # дем нуное время, потом подаём обратный сигнал на выключение
                self.time_f = int(time())
                now_signal = next(self.signals)
                now_interval = next(self.intervals)
            fixprint('времени с прошлого', self.ckass_name,
                        'прошло', time() - self.time_f,
                        'интервалы сейчас:', now_signal, test=self.debag)
            sleep(self.interval_updates)


# класс в котором просто отправляется сообщение на ардуину
class ContactWithArd(threading.Thread):

    def __init__(self, debag=False, pros_ard=None, name="ContactWithArd"):
        threading.Thread.__init__(self)
        self.debag = debag
        self.pros_ard = pros_ard
        self.prnt = lambda *cont: fixprint(*cont, test=debag, class_name=name)
        ports = ['/dev/ttyACM0', '/dev/ttyACM1',
                 '/dev/ttyACM2', '/dev/ttyUSB0',
                 '/dev/ttyUSB1']
        self.working_ports = []
        for i in ports:
            try:
                ser = Serial(i, 9600)
                ser.close()
                self.working_ports.append(i)
            except Exception:
                pass
        self.working_ports = list(set(self.working_ports + serial_ports()))
        self.prnt(self.working_ports)
        self.ard_port = self.working_ports[0] if bool(self.working_ports) else "some_port"
        self.speed_port = 9600
        try:
            self.ser = Serial(self.ard_port, self.speed_port)
        except SerialException as e:
            self.ser = TrySerial(self.ard_port, self.speed_port)
        self.daemon = True
        self.flovers_yes_or_no = 'no'
        self.dengerous = False
        self.live_potok = True
        self.send = iter([])
        self.is_good_send_set = set()

    def run(self):
        while self.live_potok:
            ard_input = self.ser.readline().decode().strip()
            if "I get: " == ard_input[:7]:
                self.is_good_send_set -= set([ard_input[7:]])
            send = self.get_send_from_iter()
            if send: #  если отправка
                self.is_good_send_set.add(str(send))
                formating = bytes(str(send) + '\r\n', encoding='utf-8')
                self.prnt("отправил строку УНЕ", formating)
                self.ser.write(formating)
            self.prnt("получил строку от UNO", ard_input)
            #self.pros_ard.new_str(ard_input)

    def send_to_port(self, text):
        self.send = chain(iter([text]), self.send)

    def get_send_from_iter(self):
        try:
            return next(self.send)
        except StopIteration:
            if bool(self.is_good_send_set):
                el = self.is_good_send_set.pop()
                self.is_good_send_set.add(el)
                return el
            return None


class DataProcessingForArduino(threading.Thread):

    def __init__(self, debag=False, name="DataProcessingForArduino"):
        threading.Thread.__init__(self)
        self.debag = debag
        self.live_potok = True
        self.params = ["t_in", "h_in", "gas", "isp_l",
                       "poliv_l", "fito_led", "led",
                       "Hoter", "isp_st", "vent", "vent_root"]
        self.last_str = ""
        self.new_str = ""
        self.param_ard = {i: '--' for i in self.params}
        self.prnt = lambda *cont: fixprint(*cont, test=debag, class_name=name)

    def new_str(self, text):
        self.prnt("получил новую строку для обработки", text)
        self.new_str = text

    def run(self):
        while self.live_potok:
            if self.last_str != self.new_str:
                self.last_str = self.new_str
                self.parsing_str(self.new_str)
            sleep(1)

    def parsing_str(self, st):
        if "control_str:" in st:
            self.prnt("начал обработку строки", self.debag)
            data = st[12:].split('/')
            self.param_ard = {self.params[i]: data[j] for i, j in enumerate(data)}


def serial_ports():
    global Serial, SerialException
    """ Lists serial port names
        :raises EnvironmentError:
            On unsupported or unknown platforms
        :returns:
            A list of the serial ports available on the system
    """
    if sys.platform.startswith('win'):
        ports = ['COM%s' % (i + 1) for i in range(256)]
    elif sys.platform.startswith('linux') or sys.platform.startswith('cygwin'):
        # this excludes your current terminal "/dev/tty"
        ports = glob.glob('/dev/tty[A-Za-z]*')
    elif sys.platform.startswith('darwin'):
        ports = glob.glob('/dev/tty.*')
    else:
        raise EnvironmentError('Unsupported platform')

    result = []
    for port in ports:
        if "AMA" not in port:
            try:
                s = Serial(port)
                s.close()
                result.append(port)
            except (OSError, SerialException):
                pass
    return result
