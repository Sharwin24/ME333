#include "nu32dip.h"
#include "encoder.h"
#include "ina219.h"
#include "utilities.h"

/**
 * Menu options:
 *
 * a: Read current sensor (ADC counts)
 * b: Read current sensor (mA)
 * c: Read encoder (counts)
 * d: Read encoder (deg)
 * e: Reset encoder
 * f: Set PWM (-100 to 100)
 * g: Set current gains (Kp, Ki)
 * h: Get current gains (Kp, Ki)
 * i: Set position gains (Kp, Ki)
 * j: Get position gains (Kp, Ki)
 * k: Test current control
 * l: Go to angle (deg)
 * m: Load step trajectory
 * n: Load cubic trajectory
 * o: Execute trajectory
 * p: Unpower the motor
 * q: Quit client
 * r: Get mode
 *
 */

#define BUF_SIZE 200
#define ENC_TICKS_PER_REV 1336
#define ENC_COUNTS_TO_DEG (360 / ENC_TICKS_PER_REV)

#define PWM_FREQ 20000 // 20kHz
#define PR2_VAL ((NU32DIP_SYS_FREQ / PWM_FREQ) / 256) - 1 // period value for 20kHz with 256 prescaler

#define CURRENT_CTRL_FREQ 1000 // 1kHz
#define CURRENT_CTRL_PR3_VAL ((NU32DIP_SYS_FREQ / CURRENT_CTRL_FREQ) / 256) - 1 // period value for 1kHz with 256 prescaler

 // Direction pin is RPB13 (pin 24)
#define DIR_PIN LATBbits.LATB13


void setup_PWM() {
  __builtin_disable_interrupts();
  // Configure Timer2 to call an ISR at a frequency of 20 kHz with a priority of 5
  T2CONbits.TCKPS = 0b111; // Timer2 prescaler N=256 (1:256)
  PR2 = PR2_VAL; // period = (PR2+1) * N * 12.5 ns = 50 us, 20 kHz
  TMR2 = 0; // initial TMR2 count is 0
  T2CONbits.ON = 1; // turn on Timer2
  // Remap OC3 to pin RPA3 (pin 10)
  RPA3Rbits.RPA3R = 0b0101; // OC3
  // Configure OC3 to use Timer2 with the interrupt enabled
  OC3CONbits.OCTSEL = 0; // use Timer2 for OC3
  OC3CONbits.OCM = 0b110; // PWM mode without fault pin; other OC3CON bits are defaults
  OC3RS = 0; // Initialize to 0
  OC3R = 0; // initialize before turning OC3 on; afterward it is read-only
  OC3CONbits.ON = 1; // turn on OC3
  IPC2bits.T2IP = 5; // interrupt priority 5
  IFS0bits.T2IF = 0; // clear interrupt flag
  IEC0bits.T2IE = 1; // enable interrupt
  __builtin_enable_interrupts();
}

void setup_current_control_ISR() {
  __builtin_disable_interrupts();
  // Configure Timer3 to call an ISR at a frequency of 1 kHz with a priority of 5
  T3CONbits.TCKPS = 0b111; // Timer3 prescaler N=256 (1:256)
  PR3 = CURRENT_CTRL_PR3_VAL; // period = (PR3+1) * N * 12.5 ns = 1 ms, 1 kHz
  TMR3 = 0; // initial TMR3 count is 0
  T3CONbits.ON = 1; // turn on Timer3
  IPC3bits.T3IP = 5; // interrupt priority 5
  IFS0bits.T3IF = 0; // clear interrupt flag
  IEC0bits.T3IE = 1; // enable interrupt
  __builtin_enable_interrupts();
}

void __ISR(_TIMER_3_VECTOR, IPL5SOFT) CurrentControlISR(void) {
  mode_t m = get_mode();

  // Perform control based on mode
  switch (m) {
  case IDLE: {
    // Set the H-bridge to brake mode
    break;
  }
  case PWM: {
    break;
  }
  case ITEST: {
    break;
  }
  case HOLD: {
    break;
  }
  case TRACK: {
    break;
  }
  default: {
    break;
  }
  }

  // Clear Interrupt flag
  IFS0bits.T3IF = 0;
}

int main(void) {
  NU32DIP_Startup();
  UART2_Startup();
  INA219_Startup();

  char buffer[BUF_SIZE];
  NU32DIP_GREEN = 1;
  NU32DIP_YELLOW = 1;
  __builtin_disable_interrupts();
  // in future, initialize modules or peripherals here
  __builtin_enable_interrupts();
  while (1) {
    NU32DIP_ReadUART1(buffer, BUF_SIZE); // we expect the next character to be a menu command
    NU32DIP_YELLOW = 1; // clear the error LED
    switch (buffer[0]) {
    case 'a': { // read current sensor (ADC counts)
      break;
    }
    case 'b': { // read current sensor (mA)
      float current = INA219_read_current();
      char m[50];
      sprintf(m, "%f\r\n", current);
      NU32DIP_WriteUART1(m);
      break;
    }
    case 'c': { // read encoder counts
      WriteUART2("a");
      while (!get_encoder_flag()) {}
      set_encoder_flag(0);
      char m[50];
      int p = get_encoder_count();
      sprintf(m, "%d\r\n", p);
      NU32DIP_WriteUART1(m);
      break;
    }
    case 'd': { // read encoder degrees
      WriteUART2("a");
      while (!get_encoder_flag()) {}
      set_encoder_flag(0);
      char m[50];
      int p = get_encoder_count();
      // Convert encoder counts to degrees
      p = p % ENC_TICKS_PER_REV; // make sure p is in the range [0, 1336)
      float deg = (float)p * ENC_COUNTS_TO_DEG;
      sprintf(m, "%f\r\n", deg);
      NU32DIP_WriteUART1(m);
      break;
    }
    case 'e': { // reset encoder
      WriteUART2("b");
      break;
    }
    case 'f': { // set PWM (-100 to 100)
      break;
    }
    case 'g': {
      break;
    }
    case 'h': {
      break;
    }
    case 'i': {
      break;
    }
    case 'j': {
      break;
    }
    case 'k': {
      break;
    }
    case 'l': {
      break;
    }
    case 'm': {
      break;
    }
    case 'n': {
      break;
    }
    case 'o': {
      break;
    }
    case 'p': {
      break;
    }
    case 'q': {
      break;
    }
    case 'r': {
      mode_t m = get_mode();
      char mode[10];
      sprintf(mode, "%d\r\n", m);
      NU32DIP_WriteUART1(mode);
      break;
    }
    case 'x': {
      NU32DIP_GREEN = !NU32DIP_GREEN; // Toggle LED1 to indicate success
      break;
    }
    default: {
      NU32DIP_YELLOW = 0; // turn on LED2 to indicate an error
      break;
    }
    }
  }
  return 0;
}