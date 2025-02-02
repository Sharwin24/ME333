#include <xc.h>          // Load the proper header for the processor

void delay(void, int);

#define MAX_CYCLES 1000000 // number is 1 million
#define DELTA_CYCLES 100000 // number is 100 thousand

int main(void) {
  TRISBCLR = 0x30; // Make TRIS for B4 and B5 0, 
  LATBbits.LATB4 = 1;    // Turn GREEN and YELLOW off.  These pins sink current
  LATBbits.LATB5 = 1;    // on the NU32DIP, so "high" (1) = "off" and "low" (0) = "on"
  int cycles = MAX_CYCLES;
  delay(cycles);
  while (1) {
    cycles -= DELTA_CYCLES;
    if (cycles < 0) {
      cycles = MAX_CYCLES;
    }
    delay(cycles);
    LATBINV = 0b110000;    // toggle GREEN and YELLOW; 
  }
  return 0;
}

void delay(void, int cycles) {
  int j;
  for (j = 0; j < cycles; j++) {
    while (!PORTAbits.RA4) {
      ;   // Pin A4 is the USER switch, low (FALSE) if pressed.
    }
  }
}
