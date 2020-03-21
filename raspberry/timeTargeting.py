from importLibs import *


def wath_start_time(debag=False):
    try:
        file = open('time.txt', 'r', encoding='utf-8')
        text = file.readlines()
        file.close()
        d, h, m = map(int, text[0].split())
    except Exception as e:
        d, h, m = 0, 0, 0
        print("произошла ошибка при чтении времени из файла", e, "\nфайл перезапишется до '0 0 0'")
        file = open('time.txt', 'w', encoding='utf-8')
        file.write("0 0 0")
        file.close()
    return d, h, m

def rewriting_start_time(d, h, m):
    file = open('time.txt', 'w', encoding='utf-8')
    file.write(str(d) + ' ' + str(h) + ' ' + str(m))
    file.close()

def butiful_time(secunds, d=0, h=0, m=0):
    minuts = secunds // 60 + m
    hours = minuts // 60 + h
    days = hours // 24 + d
    minuts = minuts % 60  #(hours - h%24 - m//60) * 60
    hours = hours % 24 # (days - d - h//24) * 24
    return days, hours, minuts


class HowLong(threading.Thread):
    def __init__(self, interval, start_time, d, h, m, debag=False, name="HowLong"):
        threading.Thread.__init__(self)
        self.interval = interval
        self.minuts = m
        self.hours = h
        self.days = d
        self.prnt = lambda *cont: fixprint(*cont, test=debag, class_name=name)
        self.start_cecunds = d * 24 * 3600 + h * 3600 + m * 60
        self.start_time = start_time
        self.live_potok = True
        self.time_f = 0
        self.debag = debag
        self.days, self.hours, self.minuts = butiful_time(time() - self.start_time, \
                                                          d=self.days,
                                                          h=self.hours,
                                                          m=self.minuts)

    def run(self):
        self.time_f = int(time())
        while self.live_potok:
            if int(time()) - self.time_f - 5 >= self.interval:
                self.days, self.hours, self.minuts = butiful_time(time() - self.time_f,
                                                                  d=self.days,
                                                                  h=self.hours,
                                                                  m=self.minuts)
                self.time_f = int(time())

            self.prnt("дней", self.days, "часов", self.hours, "минут", self.minuts)
            sleep(60)
        rewriting_start_time(self.days, self.hours, self.minuts)

    def get_time(self):
        return self.days, self.hours, self.minuts

    def set_days_hors_min(self, d, h, m):
        self.start_time = time()
        self.minuts = m
        self.hours = h
        self.days = d