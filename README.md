# Cloverwatch

The Goal of the project is to create firmware for a device that can be placed on a serial wire to perform error correction and pattern recognition on packets being transmitted

## Project Goals:
- Support for both block-based and stream-based error correction
- Low latency
- Low hardware requirements
- CLI support

## Architecture Requirements
- High reliability
- Error correction requires the device to consume and retransmit incoming data
- Low latency of the Error correction module
- Low overhead of the pattern recognition module

## Design decisions
- Heavy use of lock-free circular queues rather than heap-allocated linked lists
- Minimal use of the heap in general
- DMA uart for serial IO (Currently running on Native_sim, so the API runs interrupt-based IO in the background)
- Interrupt-based UART for CLI interface
- Extensive use of templates to define compile-time values

## Progress made
- Implemented UART API handler for Serial IO
- Implemented code foundations
  - C++ based wrappers around Zephyr APIs
  - Key data structures
  - Pointer ownership model
- Implemented High-throughput and Low-latency data pipeline between the UART handler and validation/processing modules

## Currently working on
- Integration of Reed-Solomon algorithm into the pipeline
- Packet parsing

## Next Steps
1. Support for Viterbi error correction
2. CLI support
3. Hardware integration
4. Support for standard communication protocols
