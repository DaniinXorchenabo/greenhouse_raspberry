from serial import Serial

class ArduinoControl(object):

    speed = 9600

    def __init__(self):
        pass

    @classmethod
    def get_all_serial_ports(cls):
        import serial.tools.list_ports

        ports = serial.tools.list_ports.comports()

        ports_ = []
        for port, desc, hwid in sorted(ports):
            print("{}: {} [{}]".format(port, desc, hwid))
            try:
                ser = Serial(port, cls.speed)
                ser.close()
                ports_.append(port)
            except serial.serialutil.SerialException as e:
                print(e)
        return ports_


print(ArduinoControl.get_all_serial_ports())