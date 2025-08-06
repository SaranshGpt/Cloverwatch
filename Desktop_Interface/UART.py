import threading
from time import sleep

import serial
import argparse

import time

import format
def comm_verify(args):

    config = format.PacketConfig.import_from_json(args.packet_format)

    data: bytes

    if args.data_format == 'str':
        data = args.data.encode()
    elif args.data_format == 'hex':
        data = bytes.fromhex(args.data)
    elif args.data_format == 'oct':
        raise "Format not currently suppoerted"
    elif args.data_format == 'bin':
        raise "Format not currently suppoerted"
    else:
        raise ValueError(f'Unknown data format: {args.data_format}')

    packet = format.Packet(config=config, data=data)

    raw_packet = packet.packet_with_errors(args.error_rate)

    running = True

    num_sent = 0
    num_recieved_error_corrected = 0
    num_recieved_error_detected = 0
    num_recieved_crc_error = 0
    num_recieved_failure = 0

    ser = serial.serial_for_url(args.port, args.baud, timeout=1)

    def sender_thread_func():
        while running:
            target_time = time.time() + 1/args.rate
            ser.write(raw_packet)
            sleep(target_time - time.time())

    def receiver_thread_func():
        while running:
            if ser.in_waiting > 0:
                print(ser.read(ser.in_waiting))


    sender_thread = threading.Thread(target=sender_thread_func)
    receiver_thread = threading.Thread(target=receiver_thread_func)

    sender_thread.start()
    #receiver_thread.start()

    sleep(args.duration)

    running = False

    sender_thread.join(timeout=1)
    #receiver_thread.join(timeout=1)

    ser.close()

    return

if __name__ == "__main__":

    parser = argparse.ArgumentParser(prog='cloverwatch_desktop', description='Cloverwatch UART Inteface')

    parser.add_argument('port', help='Serial port to use')
    parser.add_argument('-b', '--baud', default=115200, type=int, help='Baud rate to use')

    subparsers = parser.add_subparsers(dest='command')

    parser_comm_verify = subparsers.add_parser('comm_verify', help='Verify communication with Cloverwatch')
    parser_comm_verify.add_argument('--duration', default=10, type=int, help='Duration of test')
    parser_comm_verify.add_argument('--rate', default=100, type=int, help='Number of packets send per second')
    parser_comm_verify.add_argument('--data', default="hello", type=str, help='Data to send to device')
    parser_comm_verify.add_argument('--error_rate', default=0, type=int, help='Max number of simulated errors per packet')
    parser_comm_verify.add_argument('--data_format', default='str', type=str, choices=['str', 'hex', 'oct', 'bin'], help='Format of data to send to device')
    parser_comm_verify.add_argument('--packet_format', default='config/format.json', type=str, help='Format of packets to send to device')
    parser_comm_verify.set_defaults(func=comm_verify)

    args = parser.parse_args()

    if args.command is None:
        parser.print_help()
        exit(1)

    args.func(args)