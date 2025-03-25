#define _CRT_SECURE_NO_WARNINGS

#include "sim_top.h"
//////////////////////////////////////////////////
//this is the main c file with the main function//
//////////////////////////////////////////////////


static int cycles = -1; //cycles counter

int main(int argc, char* argv[])
{
	int great_success = 0; //will hold the return value of execute_next_inst, if equals to -1 -> HALT  stop the main loop



	set_cycle_ptr(&cycles); // "send" a pointer to cycles to the input_output module
	

	//setup phase
	// creates *new* files in the directory (overrides the old files)
	FILE* irq2_f = File_open(argv[IRQ2IN_IDX], "r");
	FILE* trace_f = File_open(argv[TRACE_IDX], "w");
	FILE* leds_f = File_open(argv[LEDS_IDX], "w");
	FILE* disp_f = File_open(argv[DISPLAY7SEG_IDX], "w");
	FILE* hwtrace = File_open(argv[HWREGTRACE_IDX], "w");
	fclose(trace_f);
	fclose(hwtrace);
	fclose(leds_f);
	fclose(disp_f);


	// load  phase
	dmem_load(argv[DMEMIN_IDX]);
	imem_load(argv[IMEMIN_IDX]);
	diskin_load(argv[DISKIN_IDX]);
	//loop phase
	while (great_success != -1) // -1 means halt instruction 
	{
		cycles++; // each loop iteration increase cycle count

		check_interrupt(get_pc_ptr()); //check interrupts and update pc if needed

		great_success = execute_next_inst(argv[TRACE_IDX], argv[HWREGTRACE_IDX], argv[LEDS_IDX], argv[DISPLAY7SEG_IDX]);// execute_next_inst according to pc

		//interrupts
		clk();	// update the cycles of the input_output module and raise interrupt 0 and 1 if needed
		irq2_raise(cycles, irq2_f); // check if need to raise interrupt 2
	}
	//create output files
	dmem_out(argv[DMEMOUT_IDX]);
	regout(argv[REGOUT_IDX]);
	write_disk_out(argv[DISKOUT_IDX]);
	write_monitor_file(argv[MONITOR_IDX], argv[MONITOR_YUV_IDX]);
	create_cycles(argv[CYCLES_IDX]);
	return 0;
}

// creates the cycles.txt file
void create_cycles(char* file_name)
{
	FILE* cycles_f = File_open(file_name, "w");
	fprintf(cycles_f, "%d\n", (cycles+1));
	fclose(cycles_f);
}