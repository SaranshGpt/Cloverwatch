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

        def to_bytes(val: str) -> bytes:
            if val.startswith("0x"):
                return bytes([int(val, 16)])
            elif val.startswith("b'") or val.startswith('b"'):
                return eval(val)  # Use carefully; assumes trusted input
            else:
                return bytes(val, encoding='utf-8')
    
        config["header_byte"] = to_bytes(config["header_byte"])
        config["footer_byte"] = to_bytes(config["footer_byte"])
        config["escape_byte"] = to_bytes(config["escape_byte"])

        return PacketConfig(**config)

    def export_to_json(self, file_path: str):
        with open(file_path, "w") as file:
            json.dump(self.__dict__, file, indent=4)


class Packet:

    def crc(self, data: bytearray)-> bytes:

        config = self.config
        crc = bytearray(config.crc_length)
        for i in range(0, len(data), config.crc_length):
            chunk = data[i:i + config.crc_length]
            for j in range(len(chunk)):
                crc[j] ^= chunk[j]
        return bytes(crc)

        return crc

    def add_escape(self, data: bytes)-> bytes:

        config = self.config
        escaped = bytearray()
        reserved = {config.header_byte, config.footer_byte, config.escape_byte}
        for byte in data:
            if bytes([byte]) in reserved:
                escaped += config.escape_byte
            escaped.append(byte)
        return bytes(escaped)



    def __init__(self, config: PacketConfig, data: bytes):
        self.config = config
        self.data = data

        # Padding to align with error block size
        padding_len = (config.error_block_size - len(data) % config.error_block_size) % config.error_block_size
        self.padding = bytes(padding_len)

        # Internal length for the original data
        self.length_internal = len(data).to_bytes(config.length_bytes, byteorder=config.endianness)

        # Reed-Solomon encoding
        self.codec = reedsolo.RSCodec(config.block_resilience, config.error_block_size)
        self.payload = self.length_internal + data + self.padding
        self.payload_encoded = self.codec.encode(self.payload)

        # External length for the encoded payload
        self.length_external = len(self.payload_encoded).to_bytes(config.length_bytes, byteorder=config.endianness)

        # CRC for encoded payload
        self.crc = self.crc(self.length_external + self.payload_encoded)

        # Assemble the final packet with header/footer/escape
        escaped = self.add_escape(self.length_external + self.payload_encoded + self.crc)
        self.packet = (
            config.header_byte +
            config.header_byte +
            escaped +
            config.footer_byte +
            config.footer_byte
        )

    def packet_with_errors(self, max_errors: int) -> bytes:
        packet = bytearray(self.packet)

        for i in range(max_errors):
            ind = random.randint(2, len(packet)-2)
            val = random.randint(0, 255)
            packet[ind] = val

        return bytes(packet)