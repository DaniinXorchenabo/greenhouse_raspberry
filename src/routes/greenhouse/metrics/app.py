import random
import asyncio
import json
from typing import Any, Optional
import enum

import websockets
from fastapi import APIRouter, WebSocket, WebSocketDisconnect
from pydantic import BaseModel, Field

from src.rpi.arduino import main as arduino

app = APIRouter(prefix=(_prefix := '/metrics'))
app_metrics = app


class ActionsEnum(BaseModel):
    water: Optional[bool] = None


class SetParamEnum(BaseModel):
    led: Optional[int] = Field(None, ge=0, le=100)
    reb_blue_led: Optional[int] = Field(None, ge=0, le=100)
    temperature: Optional[int] = Field(None, ge=-10, le=70)
    fan: Optional[bool] = None
    root_fan: Optional[bool] = None
    humidity: Optional[int] = Field(None, ge=0, le=100)
    root_humidity: Optional[int] = Field(None, ge=0, le=100)

class GetArduinoRequest(BaseModel):
    pass

class InputAndroidData(BaseModel):
    set: Optional[SetParamEnum]
    get: Optional[GetArduinoRequest]
    action: Optional[ActionsEnum]


"""
{\"set\": {\"led\": 59}}
"""


# _prefix not working with @app.websocket
@app.websocket(f'{_prefix}/sensors')
async def websocket_chat(websocket: WebSocket):
    arduino_controller: arduino.ArduinoControl = arduino.ArduinoControl.all_objects_as_dict['Arduino']
    get_arduino_data = arduino.GetArduinoData[arduino_controller]()
    try:
        print('подключение...')
        await websocket.accept()
        print('подключен')

        async def send_lambda():
            nonlocal websocket
            async for d in get_arduino_data.data_generator:
                # print(type(d), d)
                if isinstance(d, dict):
                    await websocket.send_json(d)

        async def lambda_receive():
            nonlocal websocket
            while True:
                try:
                    answer = old_ans = await websocket.receive_json()
                    answer = InputAndroidData(**answer)
                    print("\n\n------------------\n", answer, old_ans, "\n------------\n")
                    [await arduino_controller.send(InputAndroidData(**{key: {key1: val1}}).json(exclude_unset=True, exclude_none=True, exclude_defaults=True))
                     for key, val in answer.dict().items() if val is not None
                     for key1, val1 in val.items()]

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
