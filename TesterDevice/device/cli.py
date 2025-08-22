from typing import List

import argparse
import shlex

class CLI:

    def __init__(self, port: str, config, commands: List[dict]):

        self.port = port
        self.config = config

        self.parser = argparse.ArgumentParser(description='TesterDevice CLI')
        self.commands = self.parser.add_subparsers(title = 'Commands', help='Available commands')

        for cmd in commands:
            cmd_parser = self.commands.add_parser(cmd["name"], help=cmd["help"])

            for arg in cmd["args"]:
                cmd_parser.add_argument(*arg["flags"], help=arg["help"], type=arg["type"], default=arg["default"])

            cmd_parser.set_defaults(func=cmd["func"])

    def run(self):
        
        while True:

            inp = input(self.config['starting_string'])

            if not inp:
                continue

            args = shlex.split(inp)

            if args[0] in ('help', '?'):
                self.parser.print_help()
                continue

            try:
                parsed_args = self.parser.parse_args(args)
            except Exception as e:
                print(f'Error parsing Arguments: {e}')
                self.parser.print_help()
                continue

            try:
                parsed_args.func(parsed_args)
            except Exception as e:
                print(f'Exception thrown: {e}')
                continue