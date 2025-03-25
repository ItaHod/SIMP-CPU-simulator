# SIMP Processor Implementation

A Computer Organization project that implements an assembler and simulator for the SIMP processor architecture.

## Project Overview

This project consists of two main components:

1. **Assembler**: Translates assembly code into SIMP binary representation
2. **Simulator**: Simulates the execution of SIMP processor instructions

## The Assembler

The assembler gets as an input an assemble program and translates it into SIMP assembly language.

The output of the assembler is two text files: imemin.txt, which is the SIMP binary representation of the program, and dmemin.txt which is the initial memory image of the program.

These two files are part of the inputs to the simulator program.

The assembler preforms two passed over the input program text file.

In the first pass, the program detects all the labels and their addresses (number of meaningful lines, i.e., not white space or comments) and stores them in an array. Also, in the first pass the program detects all the .word directives and stores each memory value in an array.

At the end of the first pass the program creates the dmemin text file.

In the second pass, each line in the input file is parsed. There are several line types: empty line, Label, word pseudo-instruction, label+word, SIMP instruction, SIMP+label.

In the second pass the program only processes the lines that contains SIMP instruction. Each field of the instruction is parsed into its binary encoding and all the fields are combined into one string which is written to the output file imemin.

The program has main function which contains two loops, one for each pass. There are also other several helper functions which preforms sub-tasks, like to encode different fields of the command to its corresponding binary representation, or find the address of a given label (for the second pass).

<img width="532" alt="image" src="https://github.com/user-attachments/assets/c6084138-a4f8-4ee7-b36f-712c477b87ae" />


The program is built from c file and c header file. The header file contains general program parameters like the op code encoding and the maximum line length.

<img width="170" alt="image" src="https://github.com/user-attachments/assets/7ac7e65c-3e26-4e54-b2f1-7f1d536b9808" />


## The Simulator

The main purpose of the Simulator program is to simulate the SIMP processor.

### General flow

<img width="465" alt="image" src="https://github.com/user-attachments/assets/a1223cbd-10ec-4b98-8ed7-0fc3da75fc02" />


### The Simulator's structure

The program is separated into several c and c header files which represent sub blocks of the processor.

The headers files contain general parameters and function declarations, so they can be called from another block.

<img width="114" alt="image" src="https://github.com/user-attachments/assets/cd67ab98-b7df-445c-b866-b3a1577014a1" />


Example of header file: input_output.h

### **Sim_top.c**

Contains the main function of the program, with the main loop:

<img width="532" alt="image" src="https://github.com/user-attachments/assets/745cdf71-f632-4b9a-a951-a33dcbccdc01" />


### **Regs.c**

This is the CPU's registers file. Has read/write functions and creates the output files: trace.txt and regout.txt.

### **data_memory.c**

This is the data memory file. It has the memory array and has read/write functions to the memory.

### **INPUT_OUTPUT.c**

- Holds the input/output registers
- Controls the interrupt routine
- Has read/write functions
- Contains the Monitor frame buffer
- Contains the Disk Drive data
- Creates the I\O related output text files

### **Instructions.c**

This is the instructions memory file. This block executes the instructions:

The function *execute_next_inst* reads the next instruction from the instructions array, then gets the values of the registers in regs.c and executes the instructions according to the op code: write/read from/to the registers/memory/IO registers, or preforms brunch and changes the pc.

<img width="532" alt="image" src="https://github.com/user-attachments/assets/9b964ff0-ed1a-45df-bb87-838cf0d4b4b3" />


In the header file, a new "type" called *instruction* is declared, which is a bit field c struct:

<img width="171" alt="image" src="https://github.com/user-attachments/assets/e9337966-f53f-48ae-a2c2-c133547ec70c" />


Each instruction which is loaded to the instruction's memory from imemin.txt is parsed into this new type.

## Test Programs

### Circle.asm

The program loops through each of the pixels in the "first quarter" (relative to the middle pixel at (127,127), line < 127, column < 127). For each pixel, the program calculates the distance from the middle pixel: (x - 127)² + (y - 127)² and if the distance is smaller than the circle radius the program writes to the frame buffer a white pixel, and if the distance is bigger, it writes a black pixel. Due to symmetry, it is sufficient to check only quarter of all the pixels. For example, if the distance from the center of the pixel at (x,y), x,y < 127, is smaller than the radius, then also the distance of the pixels at (x + 127,y), (x + 127,y + 127) and (x,y + 127) are smaller and should be white. This method can save a lot of calculation and running time.

### Mulmat.asm

Mulmat is a function that loads two 4x4 matrices from the data memory, calculates the multiplication of the matrices, and stores the result back into the data memory at another location.

It contains two loops: loopi (for each row of the result matrix) and loopj (for each element in the row). In each iteration of loopj, it branches to the "vector_mul" function, where there are two pointers to the memory addresses of the first element of the corresponding row in the left matrix and the first element in the column of the right matrix. Inside vector_mul, it multiplies the vectors using the L1 loop, which moves between the elements in the vectors and sums the multiplication results. After each loopj iteration, it goes to "save" which stores the result in the matching address in the data memory. When loopi is done (after 4 iterations), it branches to "end" which performs the tasks needed for ending the code.

### Binom.asm

The function calculates the Newton binomial coefficient by using a certain algorithm which uses recursion. We load the two inputs (n and k) from the data memory. We started the program by setting a stack which is located at memory address = 2048. Then we call the recursive function which first saves the current registers values. After that, we check if we are in the base case scenario and if so, we jump to L2 which is the closing of each recursive iteration. If we are not in the base case, the program will calculate the two recursive functions needed and sum the results.

Before we return to the function caller $ra, we will load back the registers we saved in the stack, and we will "close" the stack. At the end of the run, the result will be saved in the data memory at its destined location specified in the project instructions.

### Disktest.asm

This program moves the first 8 sectors of diskin one place forward so sector 1 will have the content of sector 0 and so on. We started the program by setting a stack which is located at memory address = 2048 and we saved two registers ($s0 and $s1) to it to not lose their initial values. We set s0 as a loop iterator and s1 as the current memory address we are working on. We start executing a read loop which goes through the first 8 sectors and "loading" the sector values to the data memory. At the start of each loop iteration, we will check if the disk is available by checking the IO register diskstatus. When we finish the read loop on all the first 8 sectors, we will execute the second loop which is responsible for updating the new values to each sector. We start at sector 8. In each iteration, we will retrieve the value from the memory address representing one sector below and then we will write this value to the current sector. At the end of the run each sector (1-8) will have the value of the sector which is below him in the memory address.

## Building and Running

To run the project:
1. Compile the assembler
2. Run the assembler on an assembly program to generate `imemin.txt` and `dmemin.txt`
3. Compile the simulator
4. Run the simulator using the generated files as input
