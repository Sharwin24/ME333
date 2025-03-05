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

 // a: Read current sensor (ADC counts)
void read_current_sensor_adc(void) {

}

// b: Read current sensor (mA)
void read_current_sensor_ma(void) {

}

// c: Read encoder (counts)
void read_encoder_counts(void) {
  WriteUART2("a");
  while (!get_encoder_flag()) {}
  set_encoder_flag(0);
  char m[50];
  int p = get_encoder_count();
  sprintf(m, "%d\r\n", p);
  NU32DIP_WriteUART1(m);
}

// d: Read encoder (deg)
void read_encoder_deg(void) {

}

// e: Reset encoder
void reset_encoder(void) {
  WriteUART2("b");
}

// f: Set PWM (-100 to 100)
void set_pwm(void) {

}

// g: Set current gains (Kp, Ki)
void set_current_gains(void) {

}

// h: Get current gains (Kp, Ki)
void get_current_gains(void) {

}

// i: Set position gains (Kp, Ki)
void set_position_gains(void) {

}

// j: Get position gains (Kp, Ki)
void get_position_gains(void) {

}

// k: Test current control
void test_current_control(void) {

}

// l: Go to angle (deg)
void go_to_angle(void) {

}

// m: Load step trajectory
void load_step_trajectory(void) {

}

// n: Load cubic trajectory
void load_cubic_trajectory(void) {

}

// o: Execute trajectory
void execute_trajectory(void) {

}

// p: Unpower the motor
void unpower_motor(void) {

}

// q: Quit client
void quit_client(void) {

}

// r: Get mode
void get_mode(void) {

}


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
      read_current_sensor_adc();
      break;
    }
    case 'b': {
      read_current_sensor_ma();
      break;
    }
    case 'c': {
      read_encoder_counts();
      break;
    }
    case 'd': {
      read_encoder_deg();
      break;
    }
    case 'e': {
      reset_encoder();
      break;
    }
    case 'f': {
      set_pwm();
      break;
    }
    case 'g': {
      set_current_gains();
      break;
    }
    case 'h': {
      get_current_gains();
      break;
    }
    case 'i': {
      set_position_gains();
      break;
    }
    case 'j': {
      get_position_gains();
      break;
    }
    case 'k': {
      test_current_control();
      break;
    }
    case 'l': {
      go_to_angle();
      break;
    }
    case 'm': {
      load_step_trajectory();
      break;
    }
    case 'n': {
      load_cubic_trajectory();
      break;
    }
    case 'o': {
      execute_trajectory();
      break;
    }
    case 'p': {
      unpower_motor();
      break;
    }
    case 'q': {
      quit_client();
      break;
    }
    case 'r': {
      get_mode();
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