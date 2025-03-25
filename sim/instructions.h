#ifndef INSTRUCTION_HEADER
#define INSTRUCTION_HEADER


#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "regs.h"
#include "input_output.h"
#include "data_memory.h"


#define INSTRACTION_MEM_SIZE (1<<12)

// opcodes
#define _ADD 0
#define _SUB 1
#define _MAC 2
#define _AND 3
#define _OR 4
#define _XOR 5
#define _SLL 6
#define _SRA 7
#define _SRL 8
#define _BEQ 9
#define _BNE 10
#define _BLT 11
#define _BGT 12
#define _BLE 13
#define _BGE 14
#define _JAL 15
#define _LW 16
#define _SW 17
#define _RETI 18
#define _IN 19
#define _OUT 20
#define _HALT 21

// declare a new "type" called instruction which is bit field struct
typedef struct instruction_fields {
	unsigned int opcode : 8;
	unsigned int rd		: 4;
	unsigned int rs		: 4;
	unsigned int rt		: 4;
	unsigned int rm		: 4;
	unsigned int imm1	: 12;
	unsigned int imm2	: 12;
} instruction;

// returns pc
int get_pc();
// update pc
void update_pc(int value);
// return the instruction for the current pc
instruction get_intruction();
// reads the imem file and updates the imem array
// each line from imem_in is parsed into the intsruction struct using the parse_instruction function
void imem_load (char* imem_file_name);
// parse binary instruction string to instruction format (struct)
instruction parse_instruction(char* instraction);
//executes next instruction
int execute_next_inst(char* trace_file_name, char* hw_file_name, char* leds_file_name, char* disp_file_name);
// returns pointer to the pc (for updating the pc from the interrupt routine)
int* get_pc_ptr();
/*creates and updates the trace.txt file
	The file trace.txt is an output text file, that contains a line of text for every instruction
	executed by the processor, in the following format :
	PC INST R0 R1 R2 R3 R4 R5 R6 R7 R8 R9 R10 R11 R12 R13 R14 R15
	Every field is printed in hexadecimal letters.The PC is the Program Counter of the
	instruction(printed in 3 hex letters).INST is the instruction encoding as was read
	form the instruction memory(12 letters), and afterwards the contents of the registers
	before the execution of instruction(meaning the result of execution can be seen only
	in the registers of the next line).Each register is printed with 8 digits.
	In the R0 field 8 zeroes should be written.R1 and R2 should print the contents of the
	immediates after sign extension was performed to 32 bits.For example if bits 11:0 in
	the instruction were all ‘1’, the value for R1 will be FFFFFFFF.
*/
void trace_update(int pc, instruction inst, char* trace_file_name);
// reads the irq2 file and gets the next irq2 cycle
void get_next_irq2(FILE* irq2_f);
// checks if there interrupt 2 request for the current cycle, and 
void irq2_raise(int cycle, FILE* irq2_f);



#endif