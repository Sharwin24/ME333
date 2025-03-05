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
    case 'a': {
      break;
    }
    case 'b': {
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
    case 'd': {
      break;
    }
    case 'e': { // reset encoder
      WriteUART2("b");
      break;
    }
    case 'f': {
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