import random
import asyncio
import json
import websockets

from fastapi import APIRouter, WebSocket, WebSocketDisconnect

from src.rpi.arduino import main as arduino


app = APIRouter(prefix=(_prefix := '/metrics'))
app_metrics = app


# _prefix not working with @app.websocket
@app.websocket(f'{_prefix}/sensors')
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
