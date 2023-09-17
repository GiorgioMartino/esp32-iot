import uvicorn
from fastapi import FastAPI
import socket

app = FastAPI()


@app.get("/")
async def root():
    return "Hello World"


def get_local_ip():
    """
    Get the local IP address of the machine
    """
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.connect(("8.8.8.8", 80))
    local_ip = s.getsockname()[0]
    s.close()
    return local_ip


print(f"HTTP server running at {get_local_ip()}")
