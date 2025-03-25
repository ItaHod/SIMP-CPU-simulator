#define _CRT_SECURE_NO_WARNINGS
#include "instructions.h"


////////////////////////////////////////////////////////////////////////////
// //this file deals with the instractions: parsing,reading and pc control//
////////////////////////////////////////////////////////////////////////////


static int pc = 0; // program counter register of the CPU
static int irq2_raise_cycle = -1; // next cycle to raise interrupt 2

// instructions memory 
static instruction instructions[INSTRACTION_MEM_SIZE] = { 0 };



//executes next instruction
int execute_next_inst(char* trace_file_name, char* hw_file_name, char* leds_file_name, char* disp_file_name)
{
	// hw_registers_names for hw_trace file
	char* hw_registers_names[] = { "irq0enable", "irq1enable", "irq2enable", "irq0status", "irq1status",
								  "irq2status", "irqhandler", "irqreturn", "clks", "leds", "display7seg", "timerenable",
								  "timercurrent", "timermax", "diskcmd", "disksector", "diskbuffer", "diskstatus",
								   "reserved","reserved", "monitoraddr", "monitordata", "monitorcmd" };

	instruction instraction = get_intruction();// returns the instruction for the *current* pc
	trace_update(get_pc(), instraction, trace_file_name); // updates the trace file *and updates the imm regs*
	update_pc(get_pc() + 1); // increment pc for the NEXT instruction, notice that the current instruction's pc is now pc-1 

	// read from the register array
	int rs = reg_read(instraction.rs);// the *value* of rs register
	int rt = reg_read(instraction.rt);// the *value* of rt register
	int rm = reg_read(instraction.rm);// the *value* of rm register
	
	//execute instruction according the opcode
	switch (instraction.opcode)
	{
	case(_ADD):
		reg_write(instraction.rd, rs + rt + rm);
		break;
	case(_SUB):
		reg_write(instraction.rd, rs - rt - rm);
		break;
	case(_MAC):
		reg_write(instraction.rd, rs * rt + rm);
		break;
	case(_AND):
		reg_write(instraction.rd, rs & rt & rm);
		break;
	case(_OR):
		reg_write(instraction.rd, rs | rt | rm);
		break;
	case(_XOR):
		reg_write(instraction.rd, rs ^ rt ^ rm);
		break;
	case(_SLL):
		reg_write(instraction.rd, rs << rt); //logical left shift
		break;
	case(_SRA):
		reg_write(instraction.rd, rs >> rt);// arithmetic right shift
		break;
	case(_SRL):
		reg_write(instraction.rd, (unsigned)rs >> rt);// logical right shift
		break;
	case(_BEQ):
		if (rs == rt)
			update_pc(rm & 0xfff); //take only 12 lsb
		break;
	case(_BNE):
		if (rs != rt)
			update_pc(rm & 0xfff); //take only 12 lsb
		break;
	case(_BLT):
		if (rs < rt)
			update_pc(rm & 0xfff); //take only 12 lsb
		break;
	case(_BGT):
		if (rs > rt)
			update_pc(rm & 0xfff); //take only 12 lsb
		break;
	case(_BLE):
		if (rs <= rt)
			update_pc(rm & 0xfff); //take only 12 lsb
		break;
	case(_BGE):
		if (rs >= rt)
			update_pc(rm & 0xfff); //take only 12 lsb
		break;
	case(_JAL):
		reg_write(instraction.rd, (get_pc()));
		update_pc(rm & 0xfff); //take only 12 lsb
		break;
	case(_LW):
		reg_write(instraction.rd, dmem_read((rs + rt + rm) & 0xfff));
		break;
	case(_SW):
		dmem_write((rs + rt) & 0xfff, rm + reg_read(instraction.rd));
		break;
	case(_RETI):
		update_pc(read_IO(IRQRETURN)); // set pc to the address at IRQRETURN
		end_interrupt_routine(); // end interrupt service routine
		break;
	case(_IN):
		reg_write(instraction.rd, read_IO(rs + rt));
		append_hw_record(hw_file_name, (uint32_t)read_IO(rs + rt), "READ", hw_registers_names[rs + rt]); // update hw_trace
		break;
	case(_OUT):
		if (rs + rt == LEDS)
			write_IO(rs + rt, rm, leds_file_name);
		else if (rs + rt == DISPLAY7SEG)
			write_IO(rs + rt, rm, disp_file_name);
		else
			write_IO(rs + rt, rm,"");

		append_hw_record(hw_file_name, (uint32_t)rm, "WRITE", hw_registers_names[rs + rt]);// update hw_trace
		break;
	case(_HALT):
		return -1;
		break;

	default:
		return -99;
		break;
	}
	return 1;
}


// returns pointer to the pc (for updating the pc from the interrupt routine) 
int* get_pc_ptr()
{
	return &pc;
}

// reads the imem file and updates the imem array
// each line from imem_in is parsed into the intsruction struct using the parse_instruction function
void imem_load(char* imem_file_name)
{
	FILE* input_f = File_open(imem_file_name, "r");
	char inputString[12+2 /*+2 to end of line char*/]; //holds the lines from the input file
	int i = 0;
	while (fgets(inputString, 12+2, input_f))
	{
		instructions[i] = parse_instruction(inputString);
		i++;
	}

	fclose(input_f);
}

// returns pc
int get_pc()
{
	return pc;
}

// update pc
void update_pc(int value)
{	
	pc = value % INSTRACTION_MEM_SIZE; // to avoid pc overflow
}

// return the instruction for the current pc
 instruction get_intruction()
{
	 return instructions[pc]; 
}

 // parse binary instruction string to instruction format (struct)
instruction parse_instruction(char* inst)
{
	instruction instruction;
	unsigned int temp;
	//opcode
	char opcode[2] = { inst[0], inst[1] };
	sscanf(opcode, "%X", &temp); //string to int
	instruction.opcode = temp;
	//rd
	char reg[1] = { inst[2] };
	sscanf(reg, "%X", &temp); //string to int
	instruction.rd = temp;
	//rs
	reg[0] = inst[3];
	sscanf(reg, "%X", &temp); //string to int
	instruction.rs = temp;
	//rt
	reg[0] = inst[4];
	sscanf(reg, "%X", &temp); //string to int
	instruction.rt = temp;
	//rm
	reg[0] = inst[5];
	sscanf(reg, "%X", &temp); //string to int
	instruction.rm = temp;
	//imm1
	char imm[3] = { inst[6], inst[7], inst[8] };
	sscanf(imm, "%X", &temp); //string to int
	instruction.imm1 = temp;
	//imm2
	imm[0] = inst[9],imm[1]= inst[10],imm[2]= inst[11];
	sscanf(imm, "%X", &temp); //string to int
	instruction.imm2 = temp;

	return instruction;
}
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
void trace_update(int pc, instruction inst, char* trace_file_name)
{
	FILE* trace_f = File_open(trace_file_name, "a");
	imm_regs_update(inst.imm1, inst.imm2); // update the imm regs before writing the trace file
	fprintf(trace_f, "%03X %02X%01X%01X%01X%01X%03X%03X ",pc,inst.opcode,inst.rd,inst.rs,inst.rt,inst.rm,inst.imm1,inst.imm2);
	for (int i = 0;i < NOF_REGISTERS-1;i++)
	{
		fprintf(trace_f, "%08x ", reg_read(i));
	}
	fprintf(trace_f, "%08x\n", reg_read(NOF_REGISTERS - 1));
	fclose(trace_f);
}
// reads the irq2 file and gets the next irq2 cycle
void get_next_irq2(FILE* irq2_f)
{
	if (irq2_raise_cycle >= -1) // if irq2_raise_cycle < -1 there is no more cycles for irq2
	{
		char inputString[500] = "";
		while ((strcmp(inputString, "") == 0 || strcmp(inputString, "\n") == 0) && irq2_raise_cycle != -2)
		{
			if (fgets(inputString, 500, irq2_f))
			{
				if (inputString != "" || inputString != "\n")
					sscanf(inputString, "%d", &irq2_raise_cycle); //string hexa to int
			}
			else
			{
				fclose(irq2_f);
				irq2_raise_cycle = -2; // -2 indicates that there is no more irq2 cycles
			}
		}
	}
}

// checks if there interrupt 2 request for the current cycle, and 
void irq2_raise(int cycle, FILE* irq2_f)
{
	if (cycle == irq2_raise_cycle)
	{
		get_next_irq2(irq2_f);
		write_IO(IRQ2STATUS, 1,""); // raise interrupt
	}

	if (cycle == 0) // set irq2_raise_cycle for the first time
		get_next_irq2(irq2_f);
}