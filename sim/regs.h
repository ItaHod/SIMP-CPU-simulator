#ifndef REGS_HEADER
#define REGS_HEADER

#include "instructions.h"

#define NOF_REGISTERS 16

//registers addresses
# define $zero	0
# define $imm1	1
# define $imm2	2
# define $v0	3
# define $a0	4
# define $a1	5
# define $a2	6
# define $t0    7
# define $t1    8
# define $t2    9
# define $s0    10
# define $s1	11
# define $s2	12
# define $gp	13 
# define $sp	14
# define $ra    15

void reg_write(int address, int value); 
void imm_regs_update(int imm1, int imm2);
int reg_read(int address);
void regout(char* regfile);

#endif // !REGS_HEADER