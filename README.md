# SIMP Processor Implementation

A Computer Organization project that implements an assembler and simulator for the SIMP processor architecture.

## Project Overview

This project consists of two main components:

1. **Assembler**: Translates assembly code into SIMP binary representation
2. **Simulator**: Simulates the execution of SIMP processor instructions

## The Assembler

The assembler takes an assembly program as input and translates it into SIMP assembly language, producing two output files:

- `imemin.txt`: The SIMP binary representation of the program
- `dmemin.txt`: The initial memory image of the program

### Assembler Operation

The assembler performs two passes over the input program:

- **First Pass**: 
  - Detects all labels and their addresses
  - Processes `.word` directives 
  - Creates the `dmemin.txt` file

- **Second Pass**:
  - Parses each line with SIMP instructions
  - Encodes each instruction field into its binary representation
  - Combines fields into a single string for the `imemin.txt` file

### Assembler Structure

The program consists of:
- Main function with two pass loops
- Helper functions for encoding and label resolution
- Header file with general parameters like opcode encoding

## The Simulator

The simulator's main purpose is to simulate the SIMP processor execution.

### Simulator Structure

The program is separated into several modules:

- **sim_top.c**: Contains the main function with the execution loop
- **regs.c**: CPU registers file with read/write functions; produces `trace.txt` and `regout.txt`
- **data_memory.c**: Contains the memory array with read/write functions
- **input_output.c**: 
  - Handles I/O registers
  - Controls interrupt routines
  - Contains the monitor frame buffer and disk drive data
  - Creates I/O-related output files
- **instructions.c**: Executes instructions based on opcode and manages the program counter

The instruction execution module uses a custom bit field structure for parsing and handling each instruction loaded from `imemin.txt`.

## Test Programs

The project includes several test programs:

### Circle.asm
Draws a circle on the monitor by calculating the distance from the center point (127,127) and utilizing symmetry to optimize calculations.

### Mulmat.asm
Performs matrix multiplication between two 4x4 matrices stored in memory, with the result stored back to memory.

### Binom.asm
Calculates Newton binomial coefficients using recursion, with stack management for storing register values.

### Disktest.asm
Moves the first 8 sectors of the disk one place forward, demonstrating disk I/O operations and sector management.

## Building and Running

The project is built from C source files and header files that contain general parameters and function declarations.

To run the project:
1. Compile the assembler
2. Run the assembler on an assembly program to generate `imemin.txt` and `dmemin.txt`
3. Compile the simulator
4. Run the simulator using the generated files as input
