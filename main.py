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

html = """
<!DOCTYPE html>
<html>
    <head>
        <title>Chat</title>
    </head>
    <body>
        <h1>WebSocket Chat</h1>
        <form action="" onsubmit="sendMessage(event)">
            <input type="text" id="messageText" autocomplete="off"/>
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
                var input = document.getElementById("messageText")
                ws.send(input.value)
                input.value = ''
                event.preventDefault()
            }
        </script>
    </body>
</html>
""" % (HOST, PORT)


@app.get("/")
async def get():
    return HTMLResponse(html)


if __name__ == "__main__":
    uvicorn.run("main:app", host="localhost", port=10010, reload=True)
