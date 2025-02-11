#include "nu32dip.h" // constants, funcs for startup and UART
#define CORE_TICKS (NU32DIP_SYS_FREQ / 2) // 24 M ticks (one second)

#define DELAY_250_MS (CORE_TICKS / 4) // 6 M ticks (250 ms)
#define DEBOUNCE_DELAY (CORE_TICKS / 100) // 240 K ticks (10 ms)

#define STOPWATCH_BTN PORTBbits.RB7 // TDI/RPB7/CTED3/PMD5/INT0/RB7

void __ISR(_EXTERNAL_0_VECTOR, IPL2SOFT) Ext0ISR(void) { // step 1: the ISR
  // Debounce by delaying and then checking the button again
  _CP0_SET_COUNT(0);
  while (_CP0_GET_COUNT() < DEBOUNCE_DELAY) { ; } // delay for 10ms
  if (!STOPWATCH_BTN) { // if the button is still pressed, it's a real press
    TRISBCLR = 0x30; // Make TRIS for B4 and B5 0, 
    NU32DIP_GREEN = 1;    // Turn GREEN and YELLOW off.  These pins sink current
    NU32DIP_YELLOW = 1;    // on the NU32DIP, so "high" (1) = "off" and "low" (0) = "on"
    _CP0_SET_COUNT(0);
    while (_CP0_GET_COUNT() < DELAY_250_MS) { ; } // delay for 250ms
    NU32DIP_GREEN = 0;    // Turn GREEN and YELLOW on.  These pins sink current
    NU32DIP_YELLOW = 0;    // on the NU32DIP, so "high" (1) = "off" and "low" (0) = "on"ds
  }
  IFS0bits.INT0IF = 0; // clear interrupt flag IFS0<3>
}

int main(void) {
  NU32_Startup(); // cache on, min flash wait, interrupts on, LED/button init, UART init
  __builtin_disable_interrupts(); // step 2: disable interrupts
  INTCONbits.INT0EP = 0; // step 3: INT0 triggers on falling edge
  IPC0bits.INT0IP = 2; // step 4: interrupt priority 2
  IPC0bits.INT0IS = 1; // step 4: interrupt priority 1
  IFS0bits.INT0IF = 0; // step 5: clear the int flag
  IEC0bits.INT0IE = 1; // step 6: enable INT0 by setting IEC0<3>
  __builtin_enable_interrupts(); // step 7: enable interrupts
  // Connect RD7 (USER button) to INT0 (D0)
  while (1) {
    ; // do nothing, loop forever
  }
  return 0;
}