#include "nu32dip.h"

#define PWM_FREQ 20000 // 20kHz
#define PR3_VAL (NU32DIP_SYS_FREQ / PWM_FREQ) - 1 // period value for 20kHz
#define DUTY_CYCLE 75 // 75%
#define ISR_FREQ 1000 // 1kHz
#define PR2_VAL ((NU32DIP_SYS_FREQ / ISR_FREQ) / 256) - 1 // period value for 1kHz with 256 prescaler

#define NUMSAMPS 1000 // number of points in waveform
static volatile int Waveform[NUMSAMPS]; // waveform

void PWM_Startup() {
  // Remap OC3 to pin RPA3
  RPA3Rbits.RPA3R = 0b0101; // OC3
  // Set up OC2 using Timer3 without interrupts
  T3CONbits.TCKPS = 0b000; // Timer3 prescaler N=1 (1:1)
  PR3 = PR3_VAL; // period value for 20kHz
  TMR3 = 0; // initial TMR3 count is 0
  T3CONbits.ON = 1; // turn on Timer3
  OC3CONbits.OCTSEL = 1; // use Timer3 for OC3
  OC3CONbits.OCM = 0b110; // PWM mode without fault pin; other OC3CON bits are defaults
  OC3RS = DUTY_CYCLE * (PR3_VAL + 1) / 100; // duty cycle = 75%
  OC3R = 0; // initialize before turning OC2 on; afterward it is read-only
  OC3CONbits.ON = 1; // turn on OC3
}

/**
 * @brief Creates a waveform and stores it in the global array Waveform
 *
 * @param center the center value of the waveform
 * @param A the amplitude of the waveform
 */
void makeWaveform(int center, int A) {
  for (unsigned int i = 0; i < NUMSAMPS; i++) {
    Waveform[i] = (i < NUMSAMPS / 2) ? center + A : center - A;
  }
}

void __ISR(_TIMER_2_VECTOR, IPL5SOFT) Controller(void) {   // __TIMER_2_VECTOR = 8
  static int counter = 0; // initialize counter once

  OC3RS = Waveform[counter];

  counter++;
  if (counter == NUMSAMPS) {
    counter = 0;
  }

  // Clear Interrupt flag
  IFS0bits.T2IF = 0;
}

int main(void) {
  NU32DIP_Startup();
  // PWM_Startup();

  makeWaveform((PR3_VAL + 1) / 2, PR3_VAL);
  // Configure Timer2 to call an ISR at a frequency of 1 kHz with a priority of 5
  T2CONbits.TCKPS = 0b111; // Timer2 prescaler N=256 (1:256)
  PR2 = PR2_VAL; // period = (PR2+1) * N * 12.5 ns = 1 ms, 1 kHz
  TMR2 = 0; // initial TMR2 count is 0
  T2CONbits.ON = 1; // turn on Timer2
  // Remap OC3 to pin RPA3
  RPA3Rbits.RPA3R = 0b0101; // OC3
  // Configure OC3 to use Timer2 with the interrupt enabled
  OC3CONbits.OCTSEL = 0; // use Timer2 for OC3
  OC3CONbits.OCM = 0b111; // PWM mode with fault pin; other OC3CON bits are defaults
  OC3RS = 0; // Initialize to 0
  OC3R = 0; // initialize before turning OC3 on; afterward it is read-only
  OC3CONbits.ON = 1; // turn on OC3
  IPC2bits.T2IP = 5; // interrupt priority 5
  IFS0bits.T2IF = 0; // clear interrupt flag
  IEC0bits.T2IE = 1; // enable interrupt

  while (1) { ; }
  return 0;
}