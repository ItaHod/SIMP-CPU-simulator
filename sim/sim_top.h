#ifndef SIM_TOP_HEADER
#define SIM_TOP_HEADER
#include "instructions.h"
#include "regs.h"
#include "data_memory.h"
#include <stdio.h>
#include "input_output.h"


// argv input idx
#define IMEMIN_IDX		1
#define DMEMIN_IDX		2
#define DISKIN_IDX		3
#define IRQ2IN_IDX		4
#define DMEMOUT_IDX		5
#define REGOUT_IDX		6
#define TRACE_IDX		7
#define HWREGTRACE_IDX	8
#define CYCLES_IDX		9
#define LEDS_IDX		10
#define DISPLAY7SEG_IDX 11
#define DISKOUT_IDX		12
#define MONITOR_IDX		13
#define MONITOR_YUV_IDX 14



int main(int argc, char* argv[]);
void create_cycles(char* file_name);


#endif


