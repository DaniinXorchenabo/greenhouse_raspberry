import asyncio
import os

from fastapi import FastAPI
import uvicorn

from src.init_app import init

metadata = dict()
app = FastAPI(**metadata)

app = init(app)

if __name__ == "__main__":
    uvicorn.run("main:app", host="localhost", port=10010, reload=True)
