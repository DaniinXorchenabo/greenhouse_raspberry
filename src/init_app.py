from fastapi import FastAPI
from .application import app_inits


__all__ = ['init']


def init(app: FastAPI):

    [f(app) for f in app_inits]

    return app
