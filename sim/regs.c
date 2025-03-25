#include "regs.h"

//////////////////////////////////////
// this is the CPU's registers file//
/////////////////////////////////////

// register array of 16 registers of 32 bits
static int regs[NOF_REGISTERS] = { 0 };


// write to register array
void reg_write(int address, int value)
{
	if ((address != $zero) || (address != $imm1) || (address != $imm2)) //can't change these registers value
	{
		regs[address % NOF_REGISTERS] = value; // modulo to prevent overflow
	}
}

// update the imm fields and preforms sign extantsion
void imm_regs_update(int imm1, int imm2)
{
	regs[$imm1] = (imm1 << (4 * 5)) >> (4 * 5); // fancy way to sign extend from 12 bits to 32 (4*5=20=32-12)
	regs[$imm2] = (imm2 << (4 * 5)) >> (4 * 5); // fancy way to sign extend
}

//read from register array
int reg_read(int address)
{
	if (address >= 0)
		return regs[address % NOF_REGISTERS];
	return -1;
}

/*
creates the file regout.txt
The file regout.txt is an output text file, that contains the values of the registers R3-
R15 at the end of the run (note that the constants R0-R2 should not be printed in this file). Every line will be written in 8 hexadecimal letters.
*/
void regout(char* regfile)
{
	int i;
	FILE* fp_reg;
	errno_t err = fopen_s(&fp_reg, regfile, "w");
	if (err != 0)
	{
		perror("could not open reg file");
		return;
	}
	for (i = 3; i < NOF_REGISTERS; i++)
		fprintf(fp_reg, "%08X\n", regs[i]);
	fclose(fp_reg);
}
