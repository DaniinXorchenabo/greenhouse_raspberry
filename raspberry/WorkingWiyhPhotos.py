from importLibs import *


def read_IP_setver_for_text_file():
    try:
        file = open('serverIP.txt', 'r', encoding='utf-8')
        Server_IP = file.readlines()[0]
        file.close()
        ip, port = Server_IP.split()
        return ip, int(port)
    except Exception as e:
        print("erorr in reading serverIP.txt file: ", e)
        return None, None


# класс для отправки фотографий на сервер и приёма ответа обратно
class ProSoket():
    def __init__(self, adres, port, clas_UNO, debag=False, name="ProSoket"):
        self.clas_UNO = clas_UNO
        self.adres, self.port = adres, port
        self.debag = debag
        self.prnt = lambda *cont: fixprint(*cont, test=debag, class_name=name)
        s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.no_error_sending = True
        try:
            s.connect(("gmail.com", 80))
            self.prnt('мои ip', s.getsockname()[0])
        except Exception as e:
            self.prnt('я не подключен к сети, не могу знать свой ip', e)
        s.close()

    def soket_otpravka(self, img):
        try:
            sock = socket.socket()
            fixprint(4.1)
            self.prnt("адрес и айпи", (self.adres, self.port))
            sock.connect((self.adres, self.port))
            data = pickle.dumps(img)
            self.prnt('отправляю')
            sock.sendall(data)
            self.prnt("отправил")
            sock.close()
            self.no_error_sending = True
        except Exception as e:
            self.no_error_sending = False
            print('ошибка в отправке фотографии на сервер: ', e)

    def sohet_priem(self, d):
        if self.no_error_sending:
            sleep(10)
            self.prnt('прием ответа начат...')
            self.prnt("адрес и айпи", (self.adres, self.port))
            sock = socket.socket()
            self.prnt(4.2)
            sock.connect((self.adres, self.port))
            self.prnt(4.3)
            sock.sendall(b'give_me')
            self.prnt(4.4)
            otvet = sock.recv(4096)
            data_arr = pickle.loads(otvet)
            self.prnt('принял ответ')
            self.prnt('Received', repr(otvet))
            self.prnt(data_arr)
            self.clas_UNO.flovers_yes_or_no = data_arr[0]
            sock.close()


class ProtectForYellow(threading.Thread):
    def __init__(self, interval, debag=False, name="ProtectForYellow"):
        threading.Thread.__init__(self)
        self.interval = interval
        self.comand_find_yellow = False
        self.porog_yellow = 20
        self.a = [[(26, 50, 50), (85, 255, 255)],
                  [(17, 100, 150), (24, 255, 255)]]  # интервал определения жёлтого
        self.img = None
        self.dengerous = False
        self.debag = debag
        self.prnt = lambda *cont: fixprint(*cont, test=debag, class_name=name)

    def run(self):
        while True:
            if self.comand_find_yellow:
                self.comand_find_yellow = False

    def opredelenie(self):
        if self.img:
            ch = 0
            hsv = cv2.cvtColor(self.img.copy(), cv2.COLOR_BGR2HSV)  # перевод изображения в формат hsv
            mask = cv2.inRange(hsv.copy(), self.a[0][0], self.a[0][1]).copy()  # Создаём маску зелёного

            mask_for_yellow = cv2.inRange(hsv.copy(), self.a[-1][0], self.a[-1][1])  # Создаём маску желтого

            for i in self.a[1::]:  # Создаём маску зелёного с учётом всего массива значений
                mask2 = cv2.inRange(hsv.copy(), i[0], i[1]).copy()
                if i[0][0] < self.porog_yellow:
                    ch += 1
                    cv2.add(mask_for_yellow, mask2, mask_for_yellow)
                    if ch == 1:
                        mask_for_green = mask.copy()
                cv2.add(mask, mask2, mask)  # складываем вновь полученую маску со всеми остальными

            imask_for_green = mask_for_green > 0
            solid_green_fon = self.img.copy()  # создаём полностью зелёный фон
            cv2.circle(solid_green_fon, (300, 300), 600, (0, 255, 0), -1)  # делаем его полностью зелёным
            solid_green = np.zeros_like(self.img.copy(), np.uint8)  # создаём слой с наложеной маской
            solid_green[imask_for_green] = solid_green_fon[imask_for_green]  # накладываем маску зелёного
            # cv2.imshow('green_image', solid_green)

            imask_for_yellow = mask_for_yellow > 0
            solid_yellow_fon = self.img.copy()  # создаём полностью зелёный фон
            cv2.circle(solid_yellow_fon, (300, 300), 600, (0, 0, 200), -1)  # делаем его полностью зелёным
            solid_yellow = np.zeros_like(self.img.copy(), np.uint8)  # создаём слой с наложеной маской
            solid_yellow[imask_for_yellow] = solid_yellow_fon[imask_for_yellow]  # накладываем маску зелёного
            # cv2.imshow('yellow_image', solid_yellow)

            sum_yellow, sum_green = (solid_yellow == 200).sum(), (solid_green == 255).sum()
            if (sum_yellow > 500 and sum_green < 6500) or sum_yellow > 1000:
                self.dengerous = True
            elif sum_yellow > 250:
                self.dengerous = False
            '''
            cv2.setMouseCallback('green_image', draw_circle)
            cv2.setMouseCallback('yellow_image', draw_circle)  

            k = cv2.waitKey(5) & 0xFF
            if k == 27:
                break    
        cap.release()
        cv2.destroyAllWindows() 
        '''


class NeiroCetGiveImage(threading.Thread):
    def __init__(self, adres, port,
                 interval, clas_UNO,
                 class_find_yellow,
                 debag=False,
                 name="NeiroCetGiveImage"):

        self.clas_UNO = clas_UNO
        self.class_find_yellow = class_find_yellow
        self.sok = ProSoket(adres, port, self.clas_UNO)
        threading.Thread.__init__(self)
        self.interval = interval
        self.debag = debag
        self.prnt = lambda *cont: fixprint(*cont, test=debag, class_name=name)

    def run(self):
        self.heating_cam()
        while True:
            if 1 == 1:  # try:
                try:
                    cap = cv2.VideoCapture(0)
                except Exception:
                    try:
                        cap = cv2.VideoCapture(-1)
                    except Exception:
                        cap = cv2.VideoCapture(0)
                ret, frame = cap.read()
                self.class_find_yellow.img = frame
                self.class_find_yellow.comand_find_yellow = True
                self.prnt('считал с камеры')
                cap.release()
                cv2.imwrite('kostil.jpg', frame)
                self.prnt(2)
                try:
                    frame1 = Image.open('kostil.jpg')
                    frame2 = Image.open('kostil.jpg')
                except Exception:
                    print('ошибка при чтении файла kostil.jpg\nпопытка исправить..')
                    frame1 = Image.open('kostil2.jpg')
                    frame2 = Image.open('kostil2.jpg')
                self.prnt(2.1)
                im1, im2 = frame1, frame2.transpose(Image.ROTATE_180)
                w_old, h_old = im1.size
                self.prnt(2.2)
                min_sixe, max_size = (w_old, h_old) if w_old < h_old else (h_old, w_old)
                self.prnt(2.3, 'sixe =', min_sixe, max_size)
                im1 = im1.crop((0, 0, 480, 480))
                im2 = im2.crop((0, 0, min_sixe, min_sixe))  # .transpose(Image.ROTATE_180)
                self.prnt(2.4, im1.size, im2.size)
                im1 = im1.resize((150, 150), Image.ANTIALIAS)
                im2 = im2.resize((150, 150), Image.ANTIALIAS)
                # img1 = plus_rasmer(im1,'no.jpg','8.jpg',150,150)
                # img2 = plus_rasmer(im2,'no.jpg','8.jpg',150,150)

                self.prnt(im1)
                im1.save('kostil2_1.jpg')
                im2.save('kostil2_2.jpg')
                frame1.close()
                frame2.close()
                for i in ['kostil2_1.jpg', 'kostil2_2.jpg']:
                    f1 = open(i, "rb")
                    ff1 = f1.read()
                    self.prnt(3)
                    self.sok.soket_otpravka(ff1)
                    f1.close()
                self.sok.sohet_priem(1)
                self.prnt(4)
                self.clas_UNO.dengerous = self.class_find_yellow.dengerous
                # end_signal = input()
                # if end_signal == '1':
                #   break
                # if cv2.waitKey(1) & 0xFF == ord('q'):
                #      break
                self.prnt('ждёмс')
            # except VIDEOIOERROR:
            #   print('ошибка с индексом камеры\nпопробуйте вставить USB камеры заново')
            # except Exception as e:
            #   print("какаая-то ошибка", e)
            #     print('ошибка системы\nскорее всего кривой файл фото "kostil1"\nпопробуйте вставить USB камеры заново')

            sleep(self.interval)

    @staticmethod
    def heating_cam():
        # cv2.namedWindow("frame")  # создаем главное окно
        try:
            cap = cv2.VideoCapture(0)
        except Exception:
            try:
                cap = cv2.VideoCapture(-1)
            except Exception:
                cap = cv2.VideoCapture(0)
        print('камера подключена')
        for i in range(30):
            cap.read()
        cap.release()
        print('камера прогрета')


def obrezka_do_razmer(image, inp, out, w, h):
    im = image
    w_old, h_old = im.size
    if w_old > w:
        plus = (w_old - w) / 2
        w1, w2 = (int(plus), w_old - int(plus) - 1) if plus % 1 != 0 else (int(plus), w_old - int(plus))
    else:
        w1, w2 = 0, w_old
    if h_old > h:
        plus = (h_old - h) / 2
        h1, h2 = (int(plus), h_old - int(plus) - 1) if plus % 1 != 0 else (int(plus), h_old - int(plus))
    else:
        h1, h2 = 0, h_old
    im = im.crop((w1, h1, w2, h2))
    return im


def plus_rasmer(image, inp, out, w, h, debag=False):
    global word

    im = image
    w_old, h_old = im.size

    if (w_old == w) and (h_old == h):
        im.save(out)
        return True
    if (w_old > 256) or (h_old > 256):
        # print('===============================')
        im = scale_image(im, inp, out)
    w_old, h_old = im.size
    if (w_old > w) or (h_old > h):
        im = obrezka_do_razmer(im, inp, out, w, h)
    w_old, h_old = im.size
    if w_old < w:
        plus = (w_old - w) / 2
        w1, w2 = (int(plus), w_old - int(plus) + 1) if plus % 1 != 0 else (int(plus), w_old - int(plus))
        print(w1, w2)
    else:
        w1, w2 = 0, w_old
    if h_old < h:
        plus = (h_old - h) / 2
        h1, h2 = (int(plus), h_old - int(plus) + 1) if plus % 1 != 0 else (int(plus), h_old - int(plus))
        print(h1, h2)
    else:
        h1, h2 = 0, h_old
    im = im.crop((w1, h1, w2, h2))
    try:
        im.save(out)
    except Exception as e:
        print('Непредвиденная ошибка при сохранении %s ' % e, inp)
        word += '-------------Попытка исправить' + inp + '\n-------------Попытка исправить' + str(e) + '\n'
        out = out[:-4:] + inp[-4::]
        print('Попытка исправить на', out)
        im.save(out)
    return im


def cuting(input_image_path, output_image_path, w0, h0, w, h, debag=False):
    img = Image.open(input_image_path)
    w_o, h_o = img.size
    w = w_o // abs(w) if w < 0 else w
    h = h_o // abs(h) if h < 0 else h
    w0 = w_o // abs(w0) if w0 < 0 else w0
    h0 = h_o // abs(h0) if h0 < 0 else h0
    area = (w0, h0, w, h)
    print(area)
    cropped_img = img.crop(area)
    return cropped_img


def scale_image(ing, input_image_path, output_image_path, width=None, height=None, debag=False):
    original_image = ing
    w, h = original_image.size
    if w > h:
        width = 256
    else:
        height = 256
    if width and height:
        max_size = (width, height)
    elif width:
        max_size = (width, h)
    elif height:
        max_size = (w, height)
    if (w < 150) or (h < 150):
        plus_rasmer(original_image, input_image_path, output_image_path, 150, 150)
    else:
        original_image.thumbnail(max_size, Image.ANTIALIAS)
    print(output_image_path)
    return original_image
