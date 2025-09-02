import serial
import time

class SerialComm:
    def __init__(self):
        self.port = None
        self.baudrate = None
        self.timeout = None
        self.serial = None
        self.packet = None
    def configure(self, port, baudrate, timeout):
        self.port = port
        self.baudrate = baudrate
        self.timeout = timeout
        self.serial = serial.Serial(port, baudrate, timeout=timeout)

    def add_data(self, data : bytes):
        self.packet = data

    def enable_port(self):
        self.serial.open()

    def disable_port(self):
        self.serial.close()

    def send_data(self):
        self.serial.write(self.packet.encode())

    def wait_for_data(self, num_bytes: int, timeout: int = 5):
        self.serial.read(num_bytes)

    def send_recieve(self)-> bytes:
        self.serial.write(self.packet)
        return self.serial.read(len(self.packet))