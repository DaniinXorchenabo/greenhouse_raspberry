from .test import app_for_test
from .greenhouse import greenhouse_routers

include_routes = [app_for_test] + greenhouse_routers
