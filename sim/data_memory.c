#define _CRT_SECURE_NO_WARNINGS

#include "data_memory.h"

//////////////////////////////////////
// this is the CPU's memory file/////
/////////////////////////////////////


// memory array of 4096 lines of 32 bits
static int dmem[DATA_MEM_SIZE] = { 0 };

// reads the dmem file and updates the dmem array
void dmem_load(char* dmem_file_name)
{
    FILE* fp_memin;
    char line[14];
    int i = 0;
    errno_t err;
    err = fopen_s(&fp_memin, dmem_file_name, "r");
    if (err != 0)
    {
        perror("could not open memin file\n");
        return;
    }
    while (fscanf_s(fp_memin, "%s\n", line, (unsigned int)sizeof(line)) != EOF)
    {
        sscanf(line, "%X", &dmem[i]); //string to int
        i++;
    }
    fclose(fp_memin);
}

// creates dmemout file
// if the file has less than 4096 lines they are assumed to be equal to zero
void dmem_out(char* dmem_file_name)
{
    int i, last_non_zero = 0;
    FILE* fp_mem;
    errno_t err = fopen_s(&fp_mem, dmem_file_name, "w");
    if (err != 0)
    {
        perror("could not open reg file");
        return;
    }
    for (i = (DATA_MEM_SIZE) - 1; i >= 0; i--)
    {
        if (dmem[i]!=0) // if equal strcmp is 0
        {
            last_non_zero = i;
            break;
        }
    }
    for (i = 0; i <= last_non_zero; i++)
        fprintf(fp_mem, "%08X\n", dmem[i]);
    fclose(fp_mem);
}

// write to memory array
void dmem_write(int address, int value)
{
	if (address>=0)
	{
		dmem[address % DATA_MEM_SIZE] = value;// % to prevent overflow
	}
}
// read from memory array
int dmem_read(int address)
{
    if (address >= 0)
        return dmem[address % DATA_MEM_SIZE];

    return -1;
}