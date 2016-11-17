
#include <stdio.h>

#define WORDSIZE 4

main() {

int addr = 0x42c;
int size = 64;
unsigned displ, start, rest;


  start = addr;
  displ = addr % size ;

  for (rest = size ; rest >= WORDSIZE; rest -= WORDSIZE) {
    printf("%d %x\n", (addr/5)%2, addr);
    if ( (addr+WORDSIZE) % size == 0 ) { addr = start - displ; }
    else { addr += WORDSIZE; }
  }

}
