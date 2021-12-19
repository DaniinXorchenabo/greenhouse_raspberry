import asyncio


async def timer():
    while True:
        await asyncio.sleep(1)
