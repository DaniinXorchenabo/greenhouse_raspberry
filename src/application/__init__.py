from .handlers import add_handlers
from .events import add_events

__all__ = ['app_inits']

app_inits = [add_handlers, add_events]