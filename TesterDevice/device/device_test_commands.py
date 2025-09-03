from typing import List
import yaml
import sys
import os
import time
from datetime import datetime
import threading

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

            start_time = time.time()

            rx = serial_comm.send_recieve()

            end_time = time.time()

            duration = end_time - start_time

            if(duration >= config_obj.get("timeout")):
                raise Exception(f"Timeout occurred. Terminating test.")
            elif len(rx) != len(packet.get_packet_data()):
                raise Exception(f"Received packet length does not match expected length. \nExpected length: {len(packet.get_packet_data())} \nActual length: {len(rx)}\nTerminating test.")
            else:
                print(f"Test passed. Latency measured: {duration*1000}ms.")

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

def stress_test_sender(packet, serial_comm, time_period, num_sends):

    serial_comm.add_data(packet)
    for _ in range(num_sends):
        end_time = time.time() + time_period

        serial_comm.send_data()
        time.sleep(end_time - time.time())

def stress_test_receiver(serial_comm, expected_length, num_packets):

    for i in range(num_packets):
        rx = serial_comm.wait_for_data(expected_length)
        if rx is None:
            raise Exception(f"Timeout occurred. Terminating test.")
        elif len(rx) != expected_length:
            raise Exception(f"Received packet length does not match expected length. \nExpected length: {expected_length} \nActual length: {len(rx)}\nTerminating test.")
        else :
            print(f"Received response packet {i+1}")
def run_stress_test(args):
    try:

        with open(args.config, 'r') as config_file:
            config_obj = yaml.load(config_file, Loader=yaml.FullLoader)

        serial_comm = SerialComm()

        serial_comm.configure(args.port, config_obj.get("baud_rate"), config_obj.get("timeout"))

        validator_config = TesterLib.ValidatorConfig()
        validator_config.header_byte = config_obj.get("header_byte")
        validator_config.header_size = config_obj.get("header_size")
        validator_config.escape_byte = config_obj.get("escape_byte")
        validator_config.footer_byte = config_obj.get("footer_byte")
        validator_config.footer_size = config_obj.get("footer_size")
        validator_config.length_size = config_obj.get("length_size")

        packet = Packet(args.data, args.format)
        packet.create_packet_data(validator_config, "block", "reed-solomon", [config_obj.get("chunk_size"), config_obj.get("symbols")])

        time_period = 1 / args.rate
        num_sends = args.time * args.rate

        sender_thread = threading.Thread(target=stress_test_sender, args=(packet.get_packet_data(), serial_comm, time_period, num_sends))
        receiver_thread = threading.Thread(target=stress_test_receiver, args=(serial_comm, len(packet.get_packet_data()), num_sends))

        sender_thread.start()
        receiver_thread.start()

        sender_thread.join()
        receiver_thread.join()

        if(receiver_thread.is_alive()):
            raise Exception(f"Receiver thread is still running. Terminating test.")
        else:
            print(f"Test Concluded")

    except Exception as e:
        print(f"Error: {e}")

stress_test_command = {
    "name": "stress_test",
    "help": "Tests device capacity. Does not verify error correction, only calculates performance metrics",
    "args": [
        {"flags": ["-p", "--port"], "help": "Serial port", "type": str},
        {"flags": ["-d", "--data"], "help": "Data to send", "type": str, "default": "Hello World!"},
        {"flags": ["-f", "--format"], "help": "Data format", "type": str, "default": "str"},
        {"flags": ["-t", "--time"], "help": "Duration of test in seconds", "type": int, "default": 10},
        {"flags": ["-r", "--rate"], "help": "Rate of transmission in packets per second", "type": int, "default": 10},
        {"flags": ["-c", "--config"], "help": "Config file", "type": str, "default": "config.yaml"}
    ],
    "func": lambda args: run_stress_test(args)
}