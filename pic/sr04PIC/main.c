#include "sr04.h"
#include "nu32dip.h"

int main(void) {
  NU32DIP_Startup(); // cache on, interrupts on, LED/button init, UART init
  SR04_Startup(); // initialize the pins used by the SR04


  return 0;
}