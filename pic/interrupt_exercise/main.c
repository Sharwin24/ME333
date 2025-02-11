#include "nu32dip.h"           // constants, funcs for startup and UART
#define CORE_TICKS NU32DIP_SYS_FREQ / 2 // 40 M ticks (one second)

// use only the SFRs IECx, IFSx, IPCy, and INTCON, and their
// CLR, SET, and INV registers(do not use other registers, nor the bit fields as in IFS0bits.INT0IF).

// Enable the Timer2 interrupt, set its flag status to 0, and set its vectors priority and
// subpriority to 5 and 2, respectively.
// b.Enable the Real - Time Clock and Calendar interrupt, set its flag status to 0, and set its
// vectors priority and subpriority to 6 and 1, respectively.
// c.Enable the UART1 receiver interrupt, set its flag status to 0, and set its vector’s
// priority and subpriority to 7 and 3, respectively.
// d.Enable the INT2 external input interrupt, set its flag status to 0, set its vector’s
// priority and subpriority to 3 and 2, and configure it to trigger on a rising edge.

int main(void) {
  NU32_Startup(); // cache on, min flash wait, interrupts on, LED/button init, UART init

  __builtin_disable_interrupts();

  // Timer 2 (IRQ #9, Vector #8) is IFS0<9>, IEC0<9>, IPC2<4:2>, IPC2<1:0>
  IFS0CLR = 1 << 9; // clear Timer2 interrupt flag
  IEC0SET = 1 << 9; // enable Timer2 interrupt
  IPC2CLR = 31 << 2; // clear priority and subpriority bits
  IPC2SET = 5 << 2; // set priority to 5
  IPC2SET = 2 << 0; // set subpriority to 2

  // RTCC (IRQ #30, Vector #25) is IFS0<30>, IEC0<30>, IPC6<12:10>, IPC6<9:8>
  IFS0CLR = 1 << 30; // clear RTCC interrupt flag
  IEC0SET = 1 << 30; // enable RTCC interrupt
  IPC6CLR = 31 << 10; // clear priority and subpriority bits
  IPC6SET = 6 << 10; // set priority to 6
  IPC6SET = 1 << 8; // set subpriority to 1

  // UART1 Recieve Done (IRQ #40, Vector #32) is IFS1<8>, IEC1<8>, IPC8<4:2>, IPC8<1:0>
  IFS1CLR = 1 << 8; // clear UART1 Recieve Done interrupt flag
  IEC1SET = 1 << 8; // enable UART1 Recieve Done interrupt
  IPC8CLR = 31 << 2; // clear priority and subpriority bits
  IPC8SET = 7 << 2; // set priority to 7
  IPC8SET = 3 << 0; // set subpriority to 3

  // INT2 External Input (IRQ #13, Vector #11) is IFS0<13>, IEC0<13>, IPC2<28:26>, IPC2<25:24>
  IFS0CLR = 1 << 13; // clear INT2 External Input interrupt flag
  IEC0SET = 1 << 13; // enable INT2 External Input interrupt
  IPC2CLR = 31 << 26; // clear priority and subpriority bits
  IPC2SET = 3 << 26; // set priority to 3
  IPC2SET = 2 << 24; // set subpriority to 2
  INTCONSET = 1 << 26; // set INT2 to trigger on rising edge

  __builtin_enable_interrupts();

  return 0;
}


