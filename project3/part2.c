#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

#define TLB_SIZE 16
#define LOG_PAGES 1024
#define PHYS_PAGES 256
#define PAGE_MASK 1023

#define PAGE_SIZE 1024
#define OFFSET_BITS 10
#define OFFSET_MASK 1023

#define MEMORY_SIZE PHYS_PAGES * PAGE_SIZE

// Max number of characters per line of input file to read.
#define BUFFER_SIZE 10

#define FIFO 0
#define LRU 1

void rm_page(int physical_page);

struct tlbentry {
  unsigned int logical;
  unsigned int physical;
} typedef tlbentry;


// TLB is kept track of as a circular array, with the oldest element being overwritten once the TLB is full.
tlbentry tlb[TLB_SIZE];
// number of inserts into TLB that have been completed. Use as tlbindex % TLB_SIZE for the index of the next TLB line to use.
int tlbindex = 0;

// pagetable[logical_page] is the physical page number for logical page. Value is -1 if that logical page isn't yet in the table.
int pagetable[LOG_PAGES];
int ref_table[PHYS_PAGES];
signed char main_memory[MEMORY_SIZE];

// Pointer to memory mapped backing file
signed char *backing;

int max(int a, int b)
{
  if (a > b)
    return a;
  return b;
}

/* Returns the physical address from TLB or -1 if not present. */
int search_tlb(unsigned int logical_page) {
  int i=0; 
  while(i < TLB_SIZE) {
    tlbentry entry = tlb[i];
    if (entry.logical == logical_page) return entry.physical;
    i++;
  }
    return -1;
}

/* Adds the specified mapping to the TLB, replacing the oldest mapping (FIFO replacement). */
void add_to_tlb(unsigned int logical, unsigned int physical) {
  /* TODO */
  tlbindex++;
  tlbentry entry;
  entry.logical = logical;
  entry.physical = physical;
  tlb[tlbindex % TLB_SIZE] = entry;
}

int fifo_replacement(unsigned int *free_page) {
	int selectedPage = *free_page;
  rm_page(selectedPage);
	*free_page = (*free_page + 1) % PHYS_PAGES;
	return selectedPage;
}

int lru_replacement() {
  // find lru page
  int selectedPage = 99;
  int min = INT32_MAX;
  for (int i = 0; i < PHYS_PAGES; i++) {
    if (ref_table[i] < min) {
      min = ref_table[i];
      selectedPage = i;
    }
  }
  // remove from pagetable
  rm_page(selectedPage);
  
  // return phys page number
  return selectedPage;
}

void rm_page(int physical_page) {
  for(int i = 0; i < LOG_PAGES; i++) {
    if (pagetable[i] == physical_page) {
      pagetable[i] = -1;
      break;
    } 
  }
}

int main(int argc, const char *argv[])
{
  int policy = atoi(argv[3]);

  const char *backing_filename = "BACKING_STORE.bin";//argv[1]; 
  int backing_fd = open(backing_filename, O_RDONLY);
  backing = mmap(0, MEMORY_SIZE, PROT_READ, MAP_PRIVATE, backing_fd, 0); 
  
  const char *input_filename = "addresses.txt";//argv[2];
  FILE *input_fp = fopen(input_filename, "r");
  
  // Fill page table entries with -1 for initially empty table.
  int i;
  for (i = 0; i < LOG_PAGES; i++) {
    pagetable[i] = -1;
  }

  for (int i = 0; i < PHYS_PAGES; i++) {
    ref_table[i] = -1;
  }
  
  // Character buffer for reading lines of input file.
  char buffer[BUFFER_SIZE];
  int tick = 0;

  // Data we need to keep track of to compute stats at end.
  int total_addresses = 0;
  int tlb_hits = 0;
  int page_faults = 0;
  
  // Number of the next unallocated physical page in main memory
  unsigned int free_page = 0;
  
  while (fgets(buffer, BUFFER_SIZE, input_fp) != NULL) {
    tick++;
    total_addresses++;
    int logical_address = atoi(buffer);

    /* TODO
    / Calculate the page offset and logical page number from logical_address */
    u_int32_t address = ((u_int32_t) (logical_address << 12)) >> 12; // our own little Evil Bit Hack    
    int offset = (address << 22) >> 22; 
    int logical_page = address >> 10;
    ///////
    int physical_page = search_tlb(logical_page);
    // TLB hit
    if (physical_page != -1) {
      tlb_hits++;
    }
    // TLB miss
    else 
    {
      physical_page = pagetable[logical_page];
      
      // Page fault
      if (physical_page == -1) {
        page_faults++;
        physical_page = policy ? lru_replacement() : fifo_replacement(&free_page);

       // open store file
        FILE * fp;
        fp = fopen ("BACKING_STORE.bin", "r");
        fseek(fp, logical_page * PAGE_SIZE, SEEK_SET);

        // read page from store
        unsigned char v[PAGE_SIZE];
        fread(v, sizeof(v), 1, fp);

        // write page to memory
        for (int i = 0; i < PAGE_SIZE; i++)
          main_memory[physical_page * PAGE_SIZE + i] = v[i];
        fclose(fp);

        // add to page table
        pagetable[logical_page] = physical_page;
      }
      add_to_tlb(logical_page, physical_page);
    }
    // add to ref table
    if (policy == LRU) ref_table[physical_page] = tick;
    int physical_address = (physical_page << OFFSET_BITS) | offset;
    signed char value = main_memory[physical_page * PAGE_SIZE + offset];
    printf("Virtual address: %d Physical address: %d Value: %d\n", logical_address, physical_address, value);
  }
  
  printf("Number of Translated Addresses = %d\n", total_addresses);
  printf("Page Faults = %d\n", page_faults);
  printf("Page Fault Rate = %.3f\n", page_faults / (1. * total_addresses));
  printf("TLB Hits = %d\n", tlb_hits);
  printf("TLB Hit Rate = %.3f\n", tlb_hits / (1. * total_addresses));
  
  return 0;
}
