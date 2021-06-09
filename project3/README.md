#  COMP 304 Project 3: Virtual Memory Manager

## How to run the code

### Part 1
The program can be run through terminal by the commands “gcc part1.c -o part1” and “./part1 <backing-store> <input>”.

### Part 2
“gcc part2.c -o part2”  “./part2 <backing-store> <input> <policy>”. You can indicate the page replacement algorithm you want to use by adding “0” or “1”. ‘0’ is for FIFO and ‘1’ is for LRU.

## Note:
We also added an additional test file, ‘addresses2.txt’, to see the worst case for FIFO and the best case for LRU, and shared the file on the class WhatsApp group. 
The results we got for it was:
LRU: 259 page faults 
FIFO: 514 page faults