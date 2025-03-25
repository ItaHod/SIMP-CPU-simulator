
#ifndef IO_REGS_HEADER
#define IO_REGS_HEADER

#include "data_memory.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>


#define MONITOR_SIZE		256
#define NOF_IO_REGISTERS	23
#define DISK_LINES			16384
#define SECTOR_LINES		128
#define LINE_LENGTH			8
#define	DISK_BUSY_TIME		1024


//IO registers addresses
#define	 IRQ0ENABLE	    0
# define IRQ1ENABLE	    1
# define IRQ2ENABLE	    2
# define IRQ0STATUS	    3
# define IRQ1STATUS	    4
# define IRQ2STATUS	    5
# define IRQHANDLER	    6
# define IRQRETURN      7
# define CLKS           8
# define LEDS           9
# define DISPLAY7SEG    10
# define TIMERENABLE	11
# define TIMERCURRENT	12
# define TIMERMAX	    13 
# define DISKCMD	    14
# define DISKSECTOR     15
# define DISKBUFFER     16
# define DISKSTATUS     17
# define RESERVED       18
// 19 is also RESERVED
# define MONITORADDR    20
# define MONITORDATA    21
# define MONITORCMD     22




// "feeds" the IO enteties with clock -> updates counters 
void clk();
// set the cycle pointer from sim_top
void set_cycle_ptr(int* cycle);

//load disk memory to disk array
void diskin_load(char* disk_filename);

//update frame buffer, gets value and address
void update_monitor();

// adds a record to the hwetrace file
void append_hw_record(char* fp_hwe, uint32_t data, char* read_write, char* name);

// gets a file pointer and a uint value and writes the int and the clock cycle as the next line in the file - for leds and display - writes the data in Hexa!
void append_record_to_file(char* file_name, uint32_t value);

// read from IO registers
int read_IO(int adr);
//write to IO regs, checks the MONITORCMD and DISKCMD and initiates read/write to them
void write_IO(int adr, int val, char* file_name);

// creates bit mask to prevent overflow whem writing to IO regs 
int write_mask(int addr);

// set interrupt_status to 0 to indicate the end of the ISR
void end_interrupt_routine();
//check interrupt signals, to be called from main loop
void check_interrupt(int* pc_ptr);
// create monitor files
void write_monitor_file(char* monitor_fname , char* monitor_bin_fname);
// create diskout file
void write_disk_out( char* diskout_fname);
FILE* File_open(char* input_name, const char* mode);// wrapper function to open file and check that the file has opened propely




#endif


