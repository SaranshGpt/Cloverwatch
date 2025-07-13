import enum
import struct
from operator import length_hint
from typing import Literal

import reedsolo
import math
import json
import random

class PacketConfig:
    def __init__(self,
                 length_bytes: int,
                 error_block_size: int,
                 block_resilience: int,
                 crc_length: int,
                 endianness: Literal["little", "big"],
                 header_byte: bytes,
                 footer_byte: bytes,
                 escape_byte: bytes):

        self.length_bytes = length_bytes
        self.error_block_size = error_block_size
        self.block_resilience = block_resilience
        self.crc_length = crc_length
        self.endianness = endianness
        self.header_byte = header_byte
        self.footer_byte = footer_byte
        self.escape_byte = escape_byte

    def import_from_json(file_path: str):
        with open(file_path, "r") as file:
            config = json.load(file)

        return PacketConfig(**config)

    def export_to_json(self, file_path: str):
        with open(file_path, "w") as file:
            json.dump(self.__dict__, file, indent=4)


class Packet:

    def crc(self, data: bytes)-> bytes:

        config = self.config

        crc = bytes(config.crc_length)

        for i in range(0, len(data), config.crc_length):
            for j in range(config.crc_length):
                crc[j] ^= data[i+j]

        return crc

    def add_escape(self, data: bytes)-> bytes:

        config = self.config

        new_data = bytes()

        for i in range(len(data)):
            byte = data[i]

            if byte == config.header_byte or byte == config.footer_byte or byte == config.escape_byte:
                new_data += config.escape_byte

            new_data += byte

        return new_data



    def __init__(self, config: PacketConfig, data: bytes):
        self.config = config
        self.data = data

        padding_len = len(data) % config.error_block_size

        self.padding = bytes(padding_len)

        self.length_internal = len(data).to_bytes(config.length_bytes, byteorder=config.endianness)

        self.codec = reedsolo.RSCodec(config.block_resilience, config.error_block_size)

        self.payload = self.length_internal + self.data + self.padding

        self.payload_encoded = self.codec.encode(self.payload)

        self.length_external = len(self.payload).to_bytes(config.length_bytes, byteorder=config.endianness)

        self.crc = self.crc(self.length_external + self.payload)

        self.packet = config.header_byte + config.header_byte + self.add_escape(self.length_external + self.payload + self.crc) + config.footer_byte + config.footer_byte

    def packet_with_errors(self, max_errors: int) -> bytes:
        packet = bytearray(self.packet)

        for i in range(max_errors):
            ind = random.randint(2, len(packet)-2)
            val = random.randint(0, 255)
            packet[ind] = val

        return bytes(packet)