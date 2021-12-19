from fastapi import FastAPI

from src.rpi.arduino.main import ArduinoControl


def add_handlers(app: FastAPI):
    app.add_event_handler("startup", lambda : ArduinoControl())
    return app
