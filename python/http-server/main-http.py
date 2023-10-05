from fastapi import FastAPI
from pydantic import BaseModel
import socket

app = FastAPI()


class PostPayload(BaseModel):
    data: str


@app.get("/")
async def get_root():
    return "Hello World"


@app.post("/")
async def post_root(payload: PostPayload):
    print(f'Received POST with payload: {payload.data}')
    return "Hello from Python Server"


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
