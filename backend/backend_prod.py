"""
author: Jayson De La Vega
created: 2/5/25

backend_prod.py: The backend service for the oyster application in production
"""

import backend

if __name__ == "__main__":
    from waitress import serve
    serve(backend.app, host='0.0.0.0', port=backend.PORT_NUM)