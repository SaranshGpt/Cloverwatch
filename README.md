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

## Design
<img width="3309" height="3840" alt="Untitled diagram _ Mermaid Chart-2025-10-01-124303" src="https://github.com/user-attachments/assets/0835f569-4069-428c-ad54-cb06eb902a17" />


## Design decisions
- Heavy use of lock-free circular queues rather than heap-allocated linked lists for serial processing
- DMA uart for serial IO (Currently running on Native_sim, so the API runs interrupt-based IO in the background)
- Interrupt-based UART for CLI interface
- Extensive use of templates to define compile-time values
- Use of a variable-sized linked-array-based queue for pattern recognition. Splitting the difference between dynamic flexibility and static performance

## Progress made
- Implemented UART API handler for Serial IO
- Added code foundations
  - C++ based wrappers around Zephyr APIs
  - Key data structures
  - Pointer ownership model
- Implemented a High-throughput and Low-latency data pipeline between the UART handler and validation/processing modules
- Integrated Reed-Solomon implementation into the pipeline
- Implemented a packetiser format
- Developed a Python CLI-based testing suite
- Implemented pattern recognition notation
- Tentative CLI skeleton developed

## Currently working on
- Completed CLI
  - Current Progress
    - Completed CLI skeleton
    - Added functions for pattern recognition
  - Current work
    - Code refactoring
- Integration of Pattern Recognition with DMA pipeline
- Updating testing suite to test new functionality

## Next Steps
1. Hardware Integration (Current selection: Arduino nano 33 BLE)
2. Viterbi Support
3. Support for standard communication protocols
4. Work on wireless functionality
