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

    def add_data(self, data, num_errors: int, error_range: tuple[int, int]):

        for i in range(num_errors):
            error_start = error_range[0] + i * (error_range[1] - error_range[0]) // num_errors
            error_end = error_range[0] + (i + 1) * (error_range[1] - error_range[0]) // num_errors
            self.packet = data[:error_start] + bytes([data[error_start] ^ 0xFF]) + data[error_end:]
            self.serial.write(self.packet)
            time.sleep(0.01)