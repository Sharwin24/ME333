#include "sr04.h"
#include <xc.h>

// macros for the pins
#define TRIG LATBbits.LATB15
#define ECHO PORTBbits.RB14

#define CORE_TICKS_PER_SECOND 40000000
#define MS_TO_CORE_TICKS(ms) ((ms) * CORE_TICKS_PER_SECOND / 1000)
#define US_PER_TICK 24 // number of microseconds per core timer tick
#define SOUND_SPEED 346 // speed of sound in air in meters per second

#define WAIT_US(us) { \
  _CP0_SET_COUNT(0); \
  while (_CP0_GET_COUNT() < (us) * US_PER_TICK) { ; } \
}

// initialize the pins used by the SR04
void SR04_Startup() {
  ANSELA = 0; // turn off the analog input functionality that overrides everything else
  ANSELB = 0;
  TRISBbits.TRISB15 = 0; // B15 is TRIG, output from the PIC
  TRISBbits.TRISB14 = 1; // B14 is ECHO, input to the PIC
  TRIG = 0; // initialize TRIG to LOW
}
// trigger the SR04 and return the value in core timer ticks
unsigned int SR04_read_raw(unsigned int timeout) {
  // turn on TRIG
  TRIG = 1;
  // wait at least 10us
  WAIT_US(10);
  // turn off TRIG
  TRIG = 0;
  // wait until ECHO is on
  while (!ECHO) { ; }
  // note the value of the core timer
  unsigned int start = _CP0_GET_COUNT();
  // wait until ECHO is off or timeout core ticks has passed
  while (ECHO && (_CP0_GET_COUNT() - start) < timeout) { ; }
  // note the core timer
  unsigned int end = _CP0_GET_COUNT();
  // return the difference in core times
  return end - start;
}
float SR04_read_meters() {
  // read the raw rs04 value [core timer ticks]
  raw = SR04_read_raw(MS_TO_CORE_TICKS(1000)); // 1 second timeout
  // convert the time to meters, the velocity of sound in air is 346 m/s
  float meters = raw / CORE_TICKS_PER_SECOND * SOUND_SPEED / 2;
  // return the distance in meters
  return meters;
}