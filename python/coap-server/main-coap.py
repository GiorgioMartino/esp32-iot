import datetime
import logging
import asyncio
import aiocoap.resource as resource
import aiocoap
import socket


class HelloResource(resource.ObservableResource):
    def __init__(self):
        super().__init__()
        self.handle = None

    def notify(self):
        self.updated_state()
        self.reschedule()

    def reschedule(self):
        self.handle = asyncio.get_event_loop().call_later(5, self.notify)

    async def render_get(self, request):
        payload = datetime.datetime.now().strftime("%Y-%m-%d %H:%M").encode('ascii')
        print("Received GET Request")
        return aiocoap.Message(payload=payload)

    async def render_put(self, request):
        time = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
        print(f'{time} - Received PUT payload: {request.payload}')
        response_payload = b'Hello from Python Server'
        return aiocoap.Message(code=aiocoap.CHANGED, payload=response_payload)


# logging setup
logging.basicConfig(level=logging.INFO)
# logging.getLogger("coap-server").setLevel(logging.DEBUG)


def get_local_ip():
    """
    Get the local IP address of the machine
    """
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.connect(("8.8.8.8", 80))
    local_ip = s.getsockname()[0]
    s.close()
    return local_ip


async def main():
    # Resource tree creation
    root = resource.Site()
    root.add_resource(['hello'], HelloResource())

    await aiocoap.Context.create_server_context(bind=(get_local_ip(), 5683), site=root)

    print(f"CoAP server running at coap://{get_local_ip()}:5683")
    # Run forever
    await asyncio.get_running_loop().create_future()


if __name__ == "__main__":
    asyncio.run(main())
