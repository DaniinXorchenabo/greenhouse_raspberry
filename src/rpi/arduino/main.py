import asyncio
from typing import Optional, Awaitable
from datetime import datetime
from time import time, sleep

from serial import Serial
import serial.tools.list_ports
import serial_asyncio


class OutputProtocol(asyncio.protocols.Protocol):

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.current_received_string = b''


    def connection_made(self, transport: serial_asyncio.SerialTransport):
        self.transport: serial_asyncio.SerialTransport = transport
        print('port opened', transport)
        transport.serial.rts = False  # You can manipulate Serial object via transport
        transport.write(b'Hello, World!\n')  # Write serial data via transport

    def data_received(self, data: bytes):
        if data.endswith(b'\r\n'):
            data = self.current_received_string + data
            print('data received:\t', repr(data))
            data = data.replace(b'\r\n', b'').decode('utf-8')
            self.current_received_string = b''
            """
            Делать какие-то действия с пришедшими данными
            """
        else:
            self.current_received_string += data

        # if b'\r\n'  data:
        #     self.current_received_string = b''
        #     self.transport.close()

    def connection_lost(self, exc: Optional[Exception]):
        print('port closed')
        # self.transport.loop.stop()

    def pause_writing(self):
        print('pause writing')
        print(self.transport.get_write_buffer_size())

    def resume_writing(self):
        print(self.transport.get_write_buffer_size())
        print('resume writing')


class ArduinoControl(object):
    speed = 9600
    all_objects: list['ArduinoControl'] = []
    all_objects_as_dict: dict[str, 'ArduinoControl'] = dict()

    def __init__(self, name='no_name'):
        self.__class__.all_objects.append(self)
        self.__class__.all_objects_as_dict[name] = self
        self.port = self.get_all_serial_ports()
        self.coro = serial_asyncio.create_serial_connection(
            asyncio.get_running_loop(),
            OutputProtocol,
            self.port[0],
            baudrate=self.speed
        )
        self._transport: Optional[serial_asyncio.SerialTransport] = None
        self._protocol: Optional[OutputProtocol] = None

        async def _lambda():
            self._transport, self._protocol = await self.coro
            return self._transport, self._protocol

        self._connection_task = asyncio.create_task(_lambda())

    @property
    async def transport(self):
        if self._transport is None:
            self._transport, self._protocol = await self._connection_task
        self._transport: serial_asyncio.SerialTransport
        return self._transport

    @property
    async def protocol(self):
        if self._protocol is None:
            self._transport, self._protocol = await self._connection_task
        self._protocol: OutputProtocol
        return self._protocol

    async def close(self):
        protocol = await self.protocol
        protocol.connection_lost(None)

    async def send(self, text: str):
        transport = await self.transport
        transport.write(text.encode('utf-8'))

    @classmethod
    def get_all_serial_ports(cls):
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

    @classmethod
    def close_all(cls) -> Awaitable:
        return asyncio.gather([i.close() for i in cls.all_objects])




# class Test(object):
#
#     @property
#     async def fff(self):
#         # await asyncio.sleep(1)
#         print(1)
#         return 1001
#
#     async def test(self):
#         r = time()
#         d = self.fff
#         asyncio.create_task(d)
#
#         print(time() - r)
#         sleep(2)
#         print(time() - r)
#
#
#
#
# loop = asyncio.get_event_loop()
# loop.run_until_complete(Test().test())
# loop.close()

# print(ArduinoControl.get_all_serial_ports())
