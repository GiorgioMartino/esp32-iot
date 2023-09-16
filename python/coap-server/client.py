#!/usr/bin/env python3

"""This is a usage example of aiocoap that demonstrates how to implement a
simple client. See the "Usage Examples" section in the aiocoap documentation
for some more information."""

import logging
import asyncio
import socket

from aiocoap import *

logging.basicConfig(level=logging.INFO)


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
    print(f"CoAP client running at {get_local_ip()}")

    protocol = await Context.create_client_context()

    request = Message(code=GET, uri='coap://192.168.1.12:5683/time')

    # payload = b"The quick brown fox jumps over the lazy dog.\n"
    # request = Message(code=PUT, payload=payload, uri="coap://192.168.1.12:5683/block")
    try:
        response = await protocol.request(request).response
    except Exception as e:
        print('Failed to fetch resource:')
        print(e)
    else:
        print('Result: %s\n%r' % (response.code, response.payload))


if __name__ == "__main__":
    asyncio.run(main())
