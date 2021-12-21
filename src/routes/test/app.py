import asyncio
import random
import json

import websockets
from fastapi import APIRouter, WebSocket, WebSocketDisconnect

from src.rpi.arduino import main as arduino

app = APIRouter(prefix=(_prefix := '/tests'))


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


# _prefix not working with @app.websocket
@app.websocket(f'{_prefix}/received_arduino')
async def websocket_chat(websocket: WebSocket):
    get_arduino_data = arduino.GetArduinoData[arduino.ArduinoControl.all_objects_as_dict['Arduino']]()
    try:
        print('подключение...')
        await websocket.accept()
        print('подключен')

        async def send_lambda():
            nonlocal websocket
            async for d in get_arduino_data.data_generator:
                print(d)
                await websocket.send_json({
                    "temperature": round(random.random() * 50, 2),
                    "humidity": round(random.random() * 50, 2),
                    'test_': d,
                })
                await asyncio.sleep(1.5)

        async def lambda_receive():
            nonlocal websocket
            while True:
                try:
                    answer = await websocket.receive()
                    print("\n\n------------------\n", answer, "\n------------\n")
                except json.decoder.JSONDecodeError as e:
                    print('сообщение не является json-ом', e)
                except ValueError as e:
                    print('кажется, ошибка в раскодировании данных', e)

        await asyncio.gather(send_lambda(), lambda_receive())

    except WebSocketDisconnect as e:
        try:
            await websocket.close()
        except Exception as ee:
            pass
        print('вебсокет разорвал соединение', e)
    except websockets.exceptions.ConnectionClosed as e:
        print('соединение разорвано', e)
    except RuntimeError as e:
        print('RuntimeError, соединение закрыто', e)


app_for_test = app
