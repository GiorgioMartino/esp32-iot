from paho.mqtt import client as mqtt

broker_addr = "192.168.1.12"
broker_port = 1883


def main():
    client = mqtt.Client()
    client.connect(broker_addr)
    print("Connected")
    client.publish("topic/hello", "hello world", qos=1)
    print("Published msg")
    # time.sleep(5)
    client.loop_forever()


if __name__ == '__main__':
    main()
