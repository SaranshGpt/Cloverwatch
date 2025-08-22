from typing import List

from packetiser import Packet

def run_logic_test(port: str, data: str, repeat: int, data_format: str, noise: str, length: int, error_prob: float, config: str):


logic_test_command = {
    "name": "logic_test",
    "help": "Tests device error correction. Waits for reply before sending next packet. Terminates if reply is not received.",
    "args": [
        {"flags": ["-p", "--port"], "help": "Serial port", "type": str},
        {"flags": ["-d", "--data"], "help": "Data to send", "type": str, "default": "Hello World!"},
        {"flags": ["-r", "--repeat"], "help": "Number of times the packet is sent", "type": int, "default": 1},
        {"flags": ["-f", "--format"], "help": "Data format", "type": str, "default": "str"},
        {"flags": ["-n", "--noise"], "help": "Noise Type", "type": str, "default": "uniform"},
        {"flags": ["-l", "--length"], "help": "Length of error block", "type": int, "default": 10},
        {"flags": ["-e", "--error_prob"], "help": "Probability of any particular byte changing", "type": float, "default": 0.001},
        {"flags": ["-c", "--config"], "help": "Config file", "type": str, "default": "config.json"}
    ],
    "func": run_logic_test
}

def run_stress_test(port: str, data: str, data_format: str, time: int, rate: int, config: str):
    pass
# TODO

stress_test_command = {
    "name": "stress_test",
    "help": "Tests device capacity. Does not verify error correction, only calculates performance metrics",
    "args": [
        {"flags": ["-p", "--port"], "help": "Serial port", "type": str},
        {"flags": ["-d", "--data"], "help": "Data to send", "type": str, "default": "Hello World!"},
        {"flags": ["-f", "--format"], "help": "Data format", "type": str, "default": "str"},
        {"flags": ["-t", "--time"], "help": "Duration of test in seconds", "type": int, "default": 10},
        {"flags": ["-r", "--rate"], "help": "Rate of transmission in packets per second", "type": int, "default": 10},
        {"flags": ["-c", "--config"], "help": "Config file", "type": str, "default": "config.json"}
    ],
    "func": run_stress_test
}