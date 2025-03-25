#define _CRT_SECURE_NO_WARNINGS

#include "input_output.h"

////////////////////////
// input_output module//
///////////////////////

//IO register array
static int IO_regs[NOF_IO_REGISTERS] = { 0 };

// hard disk array
static int disk_arr[DISK_LINES] = { 0 };
static int frame_buffer[MONITOR_SIZE][MONITOR_SIZE] = { 0 };// frame buffer of the monitor
static int* cycles; // pointer to cycles of sim_top
static int interrupt_status = 0; // 0- not inside interrupt service routine, 1 - currently inside ISR
static int disk_busy_timer = 0; // cycle counter for disk read/write routine


// "feeds" the IO enteties with clock -> updates counters 
void clk()
{
    // update clock
    if (IO_regs[CLKS] == 0xffffffff)
        IO_regs[CLKS] = 0x00000000;
    else IO_regs[CLKS] = IO_regs[CLKS] + 1;

    // initiate read/write to disk according to DISKSTATUS
    if ((disk_busy_timer == DISK_BUSY_TIME) && IO_regs[DISKSTATUS] == 1) {
        IO_regs[IRQ1STATUS] = 1; // raise interrupt at end of writing/reading
        IO_regs[DISKSTATUS] = 0; // clear disk status
        disk_busy_timer = 0;     // reset counter
    }
    else if (IO_regs[DISKSTATUS] == 1) // disk is busy -> update counter
        disk_busy_timer++;

    //timer
    if (IO_regs[TIMERCURRENT] == IO_regs[TIMERMAX] && (IO_regs[TIMERENABLE] == 1)) // timer got to TIMERMAX
    {
        IO_regs[IRQ0STATUS] = 1; // raise interrupt
        IO_regs[TIMERCURRENT] = 0; // reset timer counter
    }
    else if (IO_regs[TIMERENABLE] == 1) // each clock TIMERENABLE is 1 -> update timer counter
    {
        IO_regs[TIMERCURRENT]= IO_regs[TIMERCURRENT]+1;
    }

}

// set the cycle pointer from sim_top
void set_cycle_ptr(int* cycle)
{
    cycles = cycle;
}


//load disk memory to disk array
void diskin_load(char* disk_filename) {
    //read file
    FILE* file = fopen(disk_filename, "r");
    if (file == NULL) {
        perror("error: cant read disk file");
    }
    int i = 0;
    char hexDecString[LINE_LENGTH + 1];//temp string of hexdecimal read from the file
    while (i < DISK_LINES && fscanf(file, "%8s", hexDecString) != EOF) {
        disk_arr[i] = strtol(hexDecString, NULL, 16);//convert from hex to decimal integer and store in array
        i++;
    }
    fclose(file);
}

// read from disk and write to memory array
void read_disk()
{
    // check if disk is busy
    if (IO_regs[DISKSTATUS] == 1)
    {
        fprintf(stderr, "error: cant read from disk when busy");
    }
    else
    {
        disk_busy_timer = 0; // initiate cycle counter
        IO_regs[DISKSTATUS] = 1; // flag disk as busy
        int sec_index = (IO_regs[DISKSECTOR]) * SECTOR_LINES; //beginning of the sector
        for (size_t i = 0; i < SECTOR_LINES; i++)
        {
            // write to memory
            dmem_write((IO_regs[DISKBUFFER] + i) % DATA_MEM_SIZE, disk_arr[sec_index + i]);
        }
    }   
}

// write disk sector from memory array
void write_disk()
{
    // check if disk is busy
    if (IO_regs[DISKSTATUS] == 1) {
        fprintf(stderr, "error: cant write to disk when busy");
    }
    else
    {
        disk_busy_timer = 0; // initiate cycle counter
        IO_regs[DISKSTATUS] = 1; //busy handling a write command
        int sec_index = (IO_regs[DISKSECTOR]) * SECTOR_LINES; //beginning of the sector
        for (size_t i = 0; i < SECTOR_LINES; i++)
        {
            //read from memory and write to disk array
            disk_arr[i + sec_index] = dmem_read((IO_regs[DISKBUFFER] + i) % DATA_MEM_SIZE);
        }
    }
}

//update frame buffer, gets value and address
void update_monitor() {
    int cmd = IO_regs[MONITORCMD];
    if (cmd == 1)
    {
        int val = IO_regs[MONITORDATA];// get pixel value to write from MONITORDATA reg
        int adr = IO_regs[MONITORADDR];// get address from  MONITORADDR reg

        int row = adr / MONITOR_SIZE;
        int column = adr - (row * MONITOR_SIZE);
        frame_buffer[row][column] = val;
        IO_regs[MONITORCMD] = 0;
    }
       
}
    

// adds a record to the hwetrace file
void append_hw_record(char* fp_hwe, uint32_t data, char* read_write, char* name)
{
    FILE* fp = File_open(fp_hwe, "a");
    fprintf(fp, "%ld %s %s %08x\n", *cycles, read_write, name, data);
    fclose(fp);
}

// create diskout file
void write_disk_out(char* diskout_fname)
{
    FILE* fp_diskout;
    int i = 0;
    errno_t  err2;
    err2 = fopen_s(&fp_diskout, diskout_fname, "w");
    if (err2 != 0)
    {
        perror("could not open disk file");
        return;
    }
    while (i < DISK_LINES)
    {
        fprintf(fp_diskout, "%08X\n", disk_arr[i]);
        i++;
    }
    fclose(fp_diskout);
}

// create monitor files
void write_monitor_file(char* monitor_fname /*, uint8_t *monitor_ptr*/, char* monitor_bin_fname)
{

    FILE* fp_monitor;
    errno_t err = fopen_s(&fp_monitor, monitor_fname, "w");
    FILE* fp_monitor_bin;
    errno_t err_bin = fopen_s(&fp_monitor_bin, monitor_bin_fname, "wb"); // wb writes in binary
    if (err != 0 || err_bin != 0)
    {
        perror("could not open monitor file\n");
        return;
    }
    for (int row = 0; row < MONITOR_SIZE; row++)
        for (int col = 0; col < MONITOR_SIZE; col++)
        {
            fprintf(fp_monitor, "%02X\n", frame_buffer[row][col]);
            fwrite(&frame_buffer[row][col], sizeof(uint8_t), 1, fp_monitor_bin);
        }

    fclose(fp_monitor);
    fclose(fp_monitor_bin);
}
// gets a file pointer and a uint value and writes the int and the clock cycle as the next line in the file - for leds and display - writes the data in Hexa!
void append_record_to_file(char* file_name, uint32_t value)
{
    FILE* fp = File_open(file_name, "a");
    fprintf(fp, "%1d %08x\n", *cycles, value);
    fclose(fp);
}

// read from IO registers
int read_IO(int adr)
{
    if (adr >= 0)
    {
        if (adr == MONITORCMD)
            return 0;
        else
            return IO_regs[adr % NOF_IO_REGISTERS];
    }
    return -1;
}

//write to IO regs, checks the MONITORCMD and DISKCMD and initiates read/write to them
void write_IO(int adr, int val, char* file_name)
{
    val = val & write_mask(adr);
    adr = adr % NOF_IO_REGISTERS;
    switch (adr)
    {
    case LEDS:
    case DISPLAY7SEG:
        if (IO_regs[adr] != val) // only if the LEDS or DISPLAY7SEG is changed, update the files
        {
            IO_regs[adr] = val;
            append_record_to_file(file_name, val); // update leds, 7segement files
        }
        break;
    case MONITORCMD:
        IO_regs[adr] = val;
        update_monitor(); // MONITORCMD changed -> update frame buffer
        break;

    case DISKCMD:
        IO_regs[adr] = val;
        // DISKCMD changed -> initiates read/write
        switch (IO_regs[DISKCMD])
        {
        case 1:
            read_disk();
            break;
        case 2:
            write_disk();
            break;
        default:
            break;
        }
        break;
    default:
        IO_regs[adr] = val;
        break;
    }

}

int write_mask(int addr)
{
    switch (addr)
    {
    case (IRQ0ENABLE):
    case (IRQ1ENABLE):
    case (IRQ2ENABLE):
    case (IRQ0STATUS):
    case (IRQ1STATUS):
    case (IRQ2STATUS):
    case (TIMERENABLE):
    case (DISKSTATUS):
    case (MONITORCMD):
        return 0x1;
        break;
    case (DISKCMD):
        return 0x3;
        break;
    case (DISKBUFFER):
    case (IRQHANDLER):
    case (IRQRETURN):
        return 0xfff;
        break;
    case (CLKS):
    case (LEDS):
    case (DISPLAY7SEG):
    case (TIMERCURRENT):
    case (TIMERMAX):
        return 0xffffffff;
        break;
    case (DISKSECTOR):
        return 0x7f;
        break;
    case (MONITORADDR):
        return 0xffff;
        break;
    case (MONITORDATA):
        return 0xff;
        break;
    case (RESERVED):
    default:
        break;
    }
    return 0xffffffff;
}

// set interrupt_status to 0 to indicate the end of the ISR
void end_interrupt_routine()
{
    interrupt_status = 0;
}

//check interrupt signals, to be called from main loop
void check_interrupt(int* pc_ptr)
{
    if ((IO_regs[IRQ0ENABLE] & IO_regs[IRQ0STATUS]) | (IO_regs[IRQ1ENABLE] & IO_regs[IRQ1STATUS]) | (IO_regs[IRQ2ENABLE] & IO_regs[IRQ2STATUS]))
    {
        if (interrupt_status == 0)// not inside ISR already
        {
            interrupt_status = 1; // flag ISR
            IO_regs[IRQRETURN] = *pc_ptr; // save current pc to IRQRETURN
            *pc_ptr = IO_regs[IRQHANDLER]; // change pc to IRQHANDLER
        }
    }
}

FILE* File_open(char* input_name, const char* mode)
{
    FILE* f = fopen(input_name, mode);
    if (f == NULL)
    {
        printf("Error: could not open file %s", input_name);
    }

    return f;
}
