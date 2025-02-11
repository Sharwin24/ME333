#include "nu32dip.h" // constants, funcs for startup and UART

#define CORE_TICKS (NU32DIP_SYS_FREQ / 2) // 24 M ticks (one second)
#define DELAY_250_MS (CORE_TICKS / 4) // 6 M ticks (250 ms)
#define DEBOUNCE_DELAY (CORE_TICKS / 100) // 240 K ticks (10 ms)
#define STOPWATCH_BTN PORTBbits.RB7 // TDI/RPB7/CTED3/PMD5/INT0/RB7

static volatile int stopwatch_state = 0; // 0 = stopped, 1 = running

void __ISR(_EXTERNAL_0_VECTOR, IPL2SOFT) Ext0ISR(void) { // step 1: the ISR
  // Debounce by delaying and then checking the button again
  unsigned int timer_ticks = _CP0_GET_COUNT();
  while (_CP0_GET_COUNT() - timer_ticks < DEBOUNCE_DELAY) { ; } // delay for 10ms
  // if the button is still pressed, it's a real press

  if (!STOPWATCH_BTN) {
    if (stopwatch_state == 0) {
      NU32DIP_WriteUART1("Starting the Stopwatch ...\r\n");
      stopwatch_state = 1; // Start the watch

      // Start the timer
      _CP0_SET_COUNT(0);
    }
    else {
      NU32DIP_WriteUART1("Stopping the Stopwatch\r\n");
      stopwatch_state = 0; // Stop the watch

      // Stop the timer and record the time
      float time_in_seconds = (float)_CP0_GET_COUNT() / CORE_TICKS;
      // Subtract 10ms from the time to account for the debounce delay
      time_in_seconds -= 0.01;
      char msg[50];
      sprintf(msg, "Time: %2.4f [sec]\r\n", time_in_seconds);
      NU32DIP_WriteUART1(msg);
    }
  }
  else {
    NU32DIP_WriteUART1("Button press ignored\r\n");
  }
  IFS0bits.INT0IF = 0; // clear interrupt flag IFS0<3>
}

int main(void) {
  NU32DIP_Startup(); // cache on, min flash wait, interrupts on, LED/button init, UART init
  stopwatch_state = 0; // Stop the watch

  __builtin_disable_interrupts(); // step 2: disable interrupts
  INTCONbits.INT0EP = 0; // step 3: INT0 triggers on falling edge
  IPC0bits.INT0IP = 2; // step 4: interrupt priority 2
  IPC0bits.INT0IS = 1; // step 4: interrupt priority 1
  IFS0bits.INT0IF = 0; // step 5: clear the int flag
  IEC0bits.INT0IE = 1; // step 6: enable INT0 by setting IEC0<3>
  __builtin_enable_interrupts(); // step 7: enable interrupts

  NU32DIP_WriteUART1("Press the Stopwatch button\r\n");
  while (1) {
    ; // do nothing, loop forever
  }
  return 0;
}