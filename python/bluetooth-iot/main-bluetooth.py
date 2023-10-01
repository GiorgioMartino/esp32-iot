import bluetooth

target_name = "ESP32-Giorgio"
target_address = None

nearby_devices = bluetooth.discover_devices()

for blt_addr in nearby_devices:
    if target_name == bluetooth.lookup_name(blt_addr):
        target_address = blt_addr
        break

if target_address is None:
    print("could not find target bluetooth device nearby")
    exit(1)

print(f"found target bluetooth device with address {target_address}")

service_matches = bluetooth.find_service(address=target_address)
first_match = service_matches[0]
port = first_match["port"]
name = first_match["name"]
host = first_match["host"]
print("connecting to \"%s\" on %s, port %s" % (name, host, port))
socket = bluetooth.BluetoothSocket(bluetooth.RFCOMM)
socket.connect((host, port))
print("connected")

buf_size = 128

while True:
    data = socket.recv(buf_size).decode()
    print(f"Received {data}")

    print(f"Sending msg")
    socket.send("python")
