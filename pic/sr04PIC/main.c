#include "sr04.h"
#include "nu32dip.h"

#define MSG_LEN 25

int main(void) {
  NU32DIP_Startup(); // cache on, interrupts on, LED/button init, UART init
  SR04_Startup(); // initialize the pins used by the SR04

  char msg[MSG_LEN];

  while (1) {
    float meters = SR04_read_meters(); // read the distance in meters
    sprintf(msg, "Distance: %4.4f [m]\r\n", meters);
    NU32DIP_WriteUART1(msg);
    unsigned int wait_us = 250000; // wait 1/4th second
    delay_us(wait_us);
  }

  return 0;
}