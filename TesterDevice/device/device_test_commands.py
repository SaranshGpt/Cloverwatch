from typing import List
import yaml
import sys
import os

from device.packetiser import Packet
from device.serial_comm import SerialComm

module_path = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', 'cmake-build-debug'))
sys.path.append(module_path)
import TesterLib

def run_logic_test(args):

    try:

        with open(args.config, 'r') as config_file:
            config_obj = yaml.load(config_file, Loader=yaml.FullLoader)

        serial_comm = SerialComm()

        serial_comm.configure(args.port, config_obj.get("baud_rate"), config_obj.get("timeout"))
        #serial_comm.enable_port()

        validator_config = TesterLib.ValidatorConfig()
        validator_config.header_byte = config_obj.get("header_byte")
        validator_config.header_size = config_obj.get("header_size")
        validator_config.escape_byte = config_obj.get("escape_byte")
        validator_config.footer_byte = config_obj.get("footer_byte")
        validator_config.footer_size = config_obj.get("footer_size")
        validator_config.length_size = config_obj.get("length_size")

        for _ in range(args.repeat):

            packet = Packet(args.data, args.format)

            match args.noise:
                case "uniform":
                    packet.apply_uniform_noise(args.error_prob)
                case "burst":
                    packet.apply_burst_noise(args.length)
                case _:
                    raise Exception("Invalid noise type")

            packet.create_packet_data(validator_config, "block", "reed-solomon", [config_obj.get("chunk_size"), config_obj.get("symbols")])

            serial_comm.add_data(packet.get_packet_data())

            print("Packet data in hex:", ' '.join([f'0x{x:02x}' for x in packet.get_packet_data()]))

            rx = serial_comm.send_recieve()

            if len(rx) != len(packet.get_packet_data()):
                raise Exception(f"Received packet length does not match expected length. \nExpected length: {len(packet.get_packet_data())} \nActual length: {len(rx)}\nTerminating test.")

    except Exception as e:
        print(f"Error: {e}")

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
        {"flags": ["-c", "--config"], "help": "Config file", "type": str, "default": "config.yaml"}
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
    "func": lambda args: run_stress_test(args.port, args.data, args.format, args.time, args.rate, args.config)
}