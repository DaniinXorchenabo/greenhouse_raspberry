from fastapi import APIRouter

from src.rpi.arduino import main as arduino


app = APIRouter(prefix='/tests')
app_for_test = app

@app.get('/sending_arduino')
async def test_sending_arduino(param: str = 'something'):
    print( arduino.ArduinoControl.all_objects_as_dict)
    await arduino.ArduinoControl.all_objects_as_dict['Arduino'].send(param)
    return 'Ok'
