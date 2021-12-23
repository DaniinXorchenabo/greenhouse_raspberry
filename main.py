import asyncio
import os

from fastapi import FastAPI
from fastapi.responses import HTMLResponse
import uvicorn

from src.init_app import init

import asyncio
import random
import json

import websockets
from fastapi import APIRouter, WebSocket, WebSocketDisconnect

from src.rpi.arduino import main as arduino

metadata = dict()
app = FastAPI(**metadata)

app = init(app)

PORT = 10010
HOST = "localhost"

# language=HTML
html = """
<!DOCTYPE html>
<html>
    <head>
        <title>Chat</title>
    </head>
    <body>
        <h1>WebSocket Chat</h1>
        <form action="" onsubmit="sendMessage(event)" name='main_form'>
            <input type="text" id="messageText" autocomplete="off"/>
            <p><label>Белый цвет: 
            <input id='led' class='set' type='number' value='0'/></label></p>
            <p><label>фитоосвещение: 
            <input id='reb_blue_led' class='set' type='number' value='0'/></label></p>
            <p><label>Задать желательный уровень влажности воздуха: 
            <input id='humidity' class='set' type='number' value='0'/></label></p>
            <p><label>Задать желательный уровень влажности в корнях: 
            <input id='root_humidity' class='set' type='number' value='0'/></label></p>
            <p><label>Задать желательный уровень температуры воздуха: 
            <input id='temperature' class='set' type='number' value='0'/></label></p>
            <p><label>включить ли проветривание?: 
            <input id='fan' class='set' type='number' value='0'/></label></p>
            <p><label>включить ли проветривание корней?: 
            <input id='root_fan' class='set' type='number' value='0'/></label></p>
            <p><label>Полить: 
            <input id='water' class='action' type='number' value='0'/></label></p>


            <button>Send</button>
        </form>
        <ul id='messages'>
        </ul>
        <script>
            var ws = new WebSocket("ws://%s:%s/metrics/sensors");
            ws.onmessage = function(event) {
                var messages = document.getElementById('messages')
                var message = document.createElement('li')
                var content = document.createTextNode(event.data)
                message.appendChild(content)
                messages.appendChild(message)
            };
            function sendMessage(event) {
                const _data = {
                    set: {},
                    get: {},
                    action: {}
                };
                [...document.forms[0].elements].map(i => {
                    console.log(i.className, i.id, i.value);
                    if (i.className && i.id){
                    _data[i.className.toString()][i.id] = i.value;
                    }
                });
                console.log(_data);
            
                var input = document.getElementById("messageText");
                ws.send(JSON.stringify(_data));
                input.value = '';
                event.preventDefault();
            }
        </script>
    </body>
</html>
""" % (HOST, PORT)


@app.get("/")
async def get():
    return HTMLResponse(html)


if __name__ == "__main__":
    uvicorn.run("main:app", host="0.0.0.0", port=10010, reload=True)
