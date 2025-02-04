#include "sr04.h"
#include <xc.h>
#include "nu32dip.h"

// macros for the pins
#define TRIG LATBbits.LATB15
#define ECHO PORTBbits.RB14
#define SOUND_SPEED 346 // speed of sound in air in meters per second

// Ticks for 10 us
#define TICKS_10US (10 * (CORE_TICKS_PER_SECOND / 1000000))

// initialize the pins used by the SR04
void SR04_Startup() {
  ANSELA = 0; // turn off the analog input functionality that overrides everything else
  ANSELB = 0;
  TRISBbits.TRISB15 = 0; // B15 is TRIG, output from the PIC
  TRISBbits.TRISB14 = 1; // B14 is ECHO, input to the PIC
  TRIG = 0; // initialize TRIG to LOW
  _CP0_SET_COUNT(0); // reset the core timer
}
// trigger the SR04 and return the value in core timer ticks
unsigned int SR04_read_raw(unsigned int timeout) {
  // turn on TRIG
  TRIG = 1;
  // wait at least 10us
  _CP0_SET_COUNT(0);
  while ((_CP0_GET_COUNT()) < TICKS_10US) {}
  // turn off TRIG
  TRIG = 0;
  // wait until ECHO is on
  while (!ECHO) { ; }
  // note the value of the core timer
  // unsigned int start = _CP0_GET_COUNT();
  _CP0_SET_COUNT(0);
  // wait until ECHO is off or timeout core ticks has passed
  while (ECHO && (_CP0_GET_COUNT()) < timeout) {}
  // while (ECHO) { ; }
  // note the core timer
  unsigned int end = _CP0_GET_COUNT();
  // return the difference in core times
  return end;
}

float SR04_read_meters() {
  // read the raw rs04 value [core timer ticks]
  unsigned int raw = SR04_read_raw(CORE_TICKS_PER_SECOND / 2);
  // convert the time to meters, the velocity of sound in air is 346 m/s
  float sec_per_tick = 1.0f / ((float)CORE_TICKS_PER_SECOND);
  float meters = ((float)raw) * (sec_per_tick / 2.0f) * ((float)SOUND_SPEED);
  // return the distance in meters
  return meters;
}