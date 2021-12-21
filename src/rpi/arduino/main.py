from __future__ import annotations
import asyncio
from typing import Optional, Awaitable, AsyncGenerator, Type, Generator
from datetime import datetime
from time import time, sleep
from abc import ABC

from serial import Serial
import serial.tools.list_ports
import serial_asyncio


class OutputProtocol(asyncio.protocols.Protocol):

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.current_received_string = b''
        self.send_data_to: Generator = None

    def connection_made(self, transport: serial_asyncio.SerialTransport):
        self.transport: serial_asyncio.SerialTransport = transport
        print('port opened', transport)
        transport.serial.rts = False  # You can manipulate Serial object via transport
        transport.write(b'Hello, World!\n')  # Write serial data via transport

    def data_received(self, data: bytes):
        # print('\tdata receiving\t:\t', repr(data))
        self.current_received_string += data
        if self.current_received_string.endswith(b'\r\n'):
            data = self.current_received_string
            print('data received:\t', repr(data))
            data = data.replace(b'\r\n', b'').decode('utf-8')
            if self.send_data_to is not None:
                self.send_data_to: Generator
                old_data = self.send_data_to.send(data)
                print('old_data', old_data)
            else:
                print('Нет обработчика')
            self.current_received_string = b''
            """
            Делать какие-то действия с пришедшими данными
            """


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


class GetArduinoData(ABC):
    all_objects: set['GetArduinoData'] = set()
    get_data_generator: Generator
    audition: ArduinoControl

    def __init__(self):
        self.data_generator = self.get_data()
        self.__class__.all_objects.add(self)
        self.queue = asyncio.Queue()

    def data(self) -> Awaitable:
        return self.data_generator.__anext__()

    async def get_data(self):
        while True:
            yield await self.queue.get()

    def put_in_queue(self, putting_data):
        return self.queue.put_nowait(putting_data)

    @classmethod
    def get_params_from_arduino(cls):
        data = 0
        while True:
            # await asyncio.sleep(1)
            print("get_params_from_arduino 1:", data)
            new_data = yield data
            print("get_params_from_arduino 2:", new_data)
            if new_data is not None:
                data = new_data
                [obj.put_in_queue(data) for obj in cls.all_objects]

    def __class_getitem__(cls, item: ArduinoControl):
        new_cls: Type[GetArduinoData] = type(
            f'{cls.__name__}_{item.__class__.__name__}_',
            (GetArduinoData,),
            dict()
        )
        new_cls.get_data_generator = new_cls.get_params_from_arduino()
        new_cls.audition = item
        return new_cls


class ArduinoControl(object):
    speed = 9600
    all_objects: list['ArduinoControl'] = []
    all_objects_as_dict: dict[str, 'ArduinoControl'] = dict()

    def __init__(self, name='no_name'):
        self.__class__.all_objects.append(self)
        self.__class__.all_objects_as_dict[name] = self
        self.port = self.get_all_serial_ports()
        self.send_data_to: Generator = GetArduinoData[self].get_params_from_arduino()
        self.coro = serial_asyncio.create_serial_connection(
            asyncio.get_running_loop(),
            OutputProtocol,
            self.port[0],
            baudrate=self.speed,
        )
        self._transport: Optional[serial_asyncio.SerialTransport] = None
        self._protocol: Optional[OutputProtocol] = None

        async def _lambda():
            self._transport, self._protocol = await self.coro
            self._protocol.send_data_to = self.send_data_to
            return self._transport, self._protocol

        async def _useless_lambda():
            data = 0
            while True:
                await asyncio.sleep(1)
                new_data = yield data
                if new_data is not None:
                    data = new_data
        next(self.send_data_to)
        self._connection_task = asyncio.create_task(_lambda())
        # asyncio.create_task(_useless_lambda(), name='test_fantom_task')

    @property
    async def transport(self):
        if self._transport is None:
            self._transport, self._protocol = await self._connection_task
            self._protocol.send_data_to = self.send_data_to
        self._transport: serial_asyncio.SerialTransport
        return self._transport

    @property
    async def protocol(self):
        if self._protocol is None:
            self._transport, self._protocol = await self._connection_task
            self._protocol.send_data_to = self.send_data_to
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
