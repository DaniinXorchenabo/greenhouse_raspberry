import asyncio
import os


from fastapi import FastAPI
import uvicorn


metadata = dict()
app = FastAPI(**metadata)


if __name__ == "__main__":


    uvicorn.run("main:app", host="localhost", port=8010, reload=True, reload_includes=['*.py', '*product_type.json'])
