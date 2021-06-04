#include <stdio.h>
#include <math.h>

#define OFFSET_MASK 20

int main () {
    int logical_address = 640626;// + pow(2, 20); // pow to check if masking correctly, no effect if working properly
    u_int32_t address = ((u_int32_t) (logical_address << 12)) >> 12; // our own little Evil Bit Hack    
    printf("logical_address << 12 = %d\n", logical_address);
    printf("after masking = %u\n", address);
    int logical_page = address >> 10;
    int offset = (address << 22) >> 22;

    printf("expected: page = 3, offset = 3\n");
    printf("  actual: page = %d, offset = %d\n", logical_page, offset);
} 


// logical_address = (int) (*((long*) &logical_address) << OFFSET_MASK); // Evil bit hack

// 0000 0000 0000 1001 1100 0110 0111 0010