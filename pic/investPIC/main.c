#include "nu32dip.h"
#include "calculate.h"
#include "io.h"

// Replace all instances of printf and scanf
// with the appropriate combinations of 
// sprintf, sscanf, NU32_ReadUART3, and NU32_WriteUART3

int main(void) {
  NU32DIP_Startup(); // cache on, interrupts on, LED/button init, UART init

  Investment inv;
  while (getUserInput(&inv)) {
    inv.invarray[0] = inv.inv0;
    calculateGrowth(&inv);
    sendOutput(inv.invarray, inv.years);
  }
  return 0;
}
