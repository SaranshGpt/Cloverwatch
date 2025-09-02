import sys
import os
import typing
from typing import List, Collection
from unittest import case
import random
from collections.abc import Sequence

module_path = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', 'cmake-build-debug'))

sys.path.append(module_path)

import TesterLib

random.seed()

class Packet:
    def __init__(self, data: str, data_format: str):
        match data_format:
            case "hex":
                self.packet = bytes.fromhex(data)
            case "str":
                self.packet = bytes(data, 'utf-8')
            case _:
                raise Exception("Invalid payload format")

    def apply_uniform_noise(self, noise_prob: float):

        self.packet = bytes([
            byte if random.uniform(0, 1) > noise_prob else random.randint(0, 255) for byte in self.packet
        ])

    def apply_burst_noise(self, length: int):

        start_ind = random.randint(0, len(self.packet) - length)
        end_ind = start_ind + length

        self.packet = self.packet[:start_ind] + bytes([random.randint(0, 255) for _ in range(length)]) + self.packet[end_ind:]

    def _apply_block_ecc(self, ecc_name: str, ecc_args: List[str]) -> bytes:
        match ecc_name:
            case "reed-solomon":
                if len(ecc_args) != 2:
                    raise Exception("Invalid number of arguments for Reed-Solomon ECC")


                mod_payload = list(self.packet)
                mod_payload = TesterLib.apply_reed_solomon(mod_payload, int(ecc_args[0]), int(ecc_args[1]))
                return bytes(mod_payload)
            case _:
                raise Exception("Invalid ECC name")

    def create_packet_data(self, validator_conf: TesterLib.ValidatorConfig, ecc_type: str, ecc_name: str, ecc_args: List[str]):

        match ecc_type:
            case "block":
                self.packet = bytes(TesterLib.create_packet_simple(
                    list(self._apply_block_ecc(ecc_name, ecc_args)),
                    validator_conf))
            case _:
                raise Exception("Invalid ECC type")

    def get_packet_data(self):
        return self.packet