from fastapi import FastAPI
from .application import app_inits
from .routes import include_routes


__all__ = ['init']


def init(app: FastAPI):

    [f(app) for f in app_inits]
    [app.include_router(route) for route in include_routes]

    return app
