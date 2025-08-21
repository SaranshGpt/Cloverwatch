import argparse
import shlex

class CLI:

    def _shell(self, command: str):
        #TODO: Add shell commands when main Device CLI ready
        pass

    def _logic_test(self, args):

        payload_bytes = bytes

        if args.format == 'str':
            payload_bytes = args.payload.encode()
        elif args.format == 'hex':
            payload_bytes = bytes.fromhex(args.payload)
        else:
            print(f'Error: Invalid format {args.format}')
            return

    def __init__(self, port: str, config):

        self.port = port
        self.config = config


        self.parser = argparse.ArgumentParser(description='Device Control CLI')
        self.commands = self.parser.add_subparsers(dest='command', description="CLI commands")

        shell = self.commands.add_parser('shell', help='Send commands to device shell')
        shell.add_argument('command', type=str, help='Command to send to device shell', required=True)

        logic_test = self.commands.add_parser('logic_test', help='Send Single Packet to the device to test error correction logic. It given more detailed diagnostics')
        logic_test.add_argument('payload', type=str, help='Data to send to device', required=True)
        logic_test.add_argument('format', type=str, default='str', description='format of payload', required=False)
        logic_test.add_argument('num_errors', type=int, default=1, description='Number of errors to add to payload', required=False)
        logic_test.add_argument('error_type', type=str, default='gaussian', description='Type of error to add to payload. Options are gaussian and burst', required=False)
        logic_test.add_argument('packet_rate', type=float, default=1.0, description='Number of packets to send per second', required=False)
        logic_test.add_argument('duration', type=int, default=1, description='Duration of test in seconds', required=False)

    def run(self):
        
        while True:

            inp = input(self.config['starting_string'])

            if not inp:
                continue

            inp = inp.lower()

            args = shlex.split(inp)

            if args[0] in ('help', '?'):
                self.parser.print_help()
                continue

            try:
                args = self.parser.parse_args(args)
            except argparse.ArgumentError as e:
                print(f'Error parsing Arguments: {e}')
                self.parser.print_help()
                continue

