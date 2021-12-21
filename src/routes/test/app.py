import asyncio
import random
import json

import websockets
from fastapi import APIRouter, WebSocket, WebSocketDisconnect

from src.rpi.arduino import main as arduino

app = APIRouter(prefix=(_prefix := '/tests'))
app_for_test = app

@app.get('/sending_arduino')
async def test_sending_arduino(param: str = 'something'):
    print(arduino.ArduinoControl.all_objects_as_dict)
    await arduino.ArduinoControl.all_objects_as_dict['Arduino'].send(param)
    return 'Ok'


@app.get('/task')
async def websocket_chat():
    print(*asyncio.all_tasks(), sep='\n')
    my_fantom_task = [i for i in asyncio.all_tasks() if i.get_name() == "test_fantom_task"][0]
    coro_: asyncio = my_fantom_task.get_coro()
    print([coro_.__class__], dir(coro_))
    # my_fantom_task.set_result('17.8')
    # print(my_fantom_task.result())
    print('----------')
    # print(*asyncio.all_tasks(), sep='\n')

    return None
