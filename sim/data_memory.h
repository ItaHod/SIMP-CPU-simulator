#ifndef DATA_MEMORY_HEADER
#define DATA_MEMORY_HEADER

#include <stdio.h>

#define DATA_MEM_SIZE (1<<12)

void dmem_load(char* dmem_file_name);
void dmem_out(char* dmem_file_name);
void dmem_write(int address, int value);
int dmem_read(int address);

#endif