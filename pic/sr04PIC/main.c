#include "sr04.h"
#include "nu32dip.h"

#define MSG_LEN 100

// Ticks for 250 ms
#define TICKS_250MS (250 * (CORE_TICKS_PER_SECOND / 1000))

int main(void) {
  NU32DIP_Startup(); // cache on, interrupts on, LED/button init, UART init
  SR04_Startup(); // initialize the pins used by the SR04

  char msg[MSG_LEN];
  NU32DIP_WriteUART1("Measuring Distance\r\n");
  while (1) {
    float meters = SR04_read_meters(); // read the distance in meters
    sprintf(msg, "Distance: %4.4f [m]\r\n", meters);
    NU32DIP_WriteUART1(msg);
    _CP0_SET_COUNT(0);
    while (_CP0_GET_COUNT() < TICKS_250MS) { ; }
  }

  return 0;
}