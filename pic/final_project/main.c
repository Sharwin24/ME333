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

volatile float pwm_duty_cycle = 0.0;

// Current control variables
volatile float mA_Kp = 0.6;
volatile float mA_Ki = 0.02;
volatile float mA_integrator = 0.0;
const float mA_integrator_max = 100.0;
const float mA_integrator_min = -mA_integrator_max;
#define ITEST_MAX_COUNT 100
float itest_ref_current[ITEST_MAX_COUNT];
float itest_mA[ITEST_MAX_COUNT];

// Position control variables
volatile float pos_Kp = 0.0;
volatile float pos_Ki = 0.0;
volatile float pos_integrator = 0.0;
const float pos_integrator_max = 100.0;
const float pos_integrator_min = -pos_integrator_max;
#define POS_MAX_COUNT 100
float pos_ref_angle[POS_MAX_COUNT];
float pos_angle[POS_MAX_COUNT];


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
    NU32DIP_GREEN = 0;
    // Set the H-bridge to brake mode
    DIR_PIN = 0;
    // Set the PWM value to 0
    OC3RS = 0;
    break;
  }
  case PWM: {
    // Set the duty cycle and direction bit according to the value (-100 to 100)
    // Set the direction bit based on the sign of the PWM value
    NU32DIP_GREEN = 1;
    DIR_PIN = (pwm_duty_cycle < 0) ? 1 : 0;
    // Set the PWM duty cycle
    float dc = (float)abs(pwm_duty_cycle);
    OC3RS = (unsigned int)((dc / 100.0) * PR2_VAL);
    break;
  }
  case ITEST: {
    // Track a +-200 mA 100 Hz square wave reference current.
    // Since a half-cycle of a 100Hz signal is 5ms, and 5 ms at
    // 5000 samples/s is 25 samples. The reference currrent toggles between
    // +200 and -200 mA every 25 samples.
    // To implement two full cycles of the current reference,
    // you can use a static int variable that counts from 0 to 99,
    // at 25, 50, and 75, the reference current sign changes.
    // When the counter reaches 99, the mode should be set to IDLE and the counter reset to 0.
    static int counter = 0;
    static float ref_current = 200.0;
    const int half_cycle = 25;
    // Increment the counter
    counter++;
    // Reset the counter and set the mode to IDLE if the counter reaches 99
    if (counter == ITEST_MAX_COUNT - 1) {
      counter = 0;
      NU32DIP_GREEN = 1;
      set_mode(IDLE);
      // Send arrays to python
      char message[BUF_SIZE];
      for (int i = 0; i < ITEST_MAX_COUNT; i++) {
        sprintf(message, "%f\r\n", itest_ref_current[i]);
        NU32DIP_WriteUART1(message);
        sprintf(message, "%f\r\n", itest_mA[i]);
        NU32DIP_WriteUART1(message);
      }
    } else if (counter == 0) {
      // Reset the integrator
      mA_integrator = 0.0;
    }
    // Get the reference current
    if (counter % half_cycle == 0) {
      ref_current = (ref_current == 200.0) ? -200.0 : 200.0;
      // Set the direction bit based on the sign of the reference current
      DIR_PIN = (ref_current < 0) ? 1 : 0;
    }
    // Read the current sensor
    float mA = INA219_read_current();
    float error = ref_current - mA;
    float u = mA_Kp * error + mA_Ki * mA_integrator;
    mA_integrator += error;
    if (mA_integrator > mA_integrator_max) {
      mA_integrator = mA_integrator_max;
    } else if (mA_integrator < mA_integrator_min) {
      mA_integrator = mA_integrator_min;
    }
    // Update the arrays for plotting
    itest_ref_current[counter] = ref_current;
    itest_mA[counter] = mA;

    // // Print debugging information
    // char debug_msg[100];
    // sprintf(debug_msg, "Counter: %d, Ref Current: %f, Measured Current: %f, Error: %f, Control Output: %f\r\n",
    //   counter, ref_current, mA, error, u);
    // NU32DIP_WriteUART1(debug_msg);

    // Set the PWM duty cycle
    OC3RS = (unsigned int)((abs(u) / 200.0) * PR2_VAL);
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
  setup_PWM();
  setup_current_control_ISR();

  // Setup DIR_PIN as an output
  TRISBbits.TRISB13 = 0;
  DIR_PIN = 0;

  char buffer[BUF_SIZE];

  // Turn off both LEDs
  NU32DIP_GREEN = 1;
  NU32DIP_YELLOW = 1;
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
      // make sure p is in the range [0, 1336)
      int p = get_encoder_count() % ENC_TICKS_PER_REV;
      // Convert encoder counts to degrees
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
      // Set the PWM value (duty cycle)
      NU32DIP_ReadUART1(buffer, BUF_SIZE);
      sscanf(buffer, "%f", &pwm_duty_cycle);
      char m[50];
      sprintf(m, "%f\r\n", pwm_duty_cycle);
      NU32DIP_WriteUART1(m);
      // Set the mode to PWM to start motor control
      set_mode(PWM);
      break;
    }
    case 'g': { // set current gains (Kp, Ki)
      NU32DIP_ReadUART1(buffer, BUF_SIZE);
      sscanf(buffer, "%f", &mA_Kp);
      NU32DIP_ReadUART1(buffer, BUF_SIZE);
      sscanf(buffer, "%f", &mA_Ki);
      char m[50];
      sprintf(m, "%f %f\r\n", mA_Kp, mA_Ki);
      NU32DIP_WriteUART1(m);
      break;
    }
    case 'h': { // get current gains (Kp, Ki)
      char m[50];
      sprintf(m, "%f %f\r\n", mA_Kp, mA_Ki);
      NU32DIP_WriteUART1(m);
      break;
    }
    case 'i': { // set position gains (Kp, Ki)
      NU32DIP_ReadUART1(buffer, BUF_SIZE);
      sscanf(buffer, "%f", &pos_Kp);
      NU32DIP_ReadUART1(buffer, BUF_SIZE);
      sscanf(buffer, "%f", &pos_Ki);
      char m[50];
      sprintf(m, "%f %f\r\n", pos_Kp, pos_Ki);
      NU32DIP_WriteUART1(m);
      break;
    }
    case 'j': { // get position gains (Kp, Ki)
      char m[50];
      sprintf(m, "%f %f\r\n", pos_Kp, pos_Ki);
      NU32DIP_WriteUART1(m);
      break;
    }
    case 'k': { // test current control
      char m[50];
      sprintf(m, "%f %f\r\n", mA_Kp, mA_Ki);
      NU32DIP_WriteUART1(m);
      set_mode(ITEST);
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
    case 'p': { // unpower the motor
      // Set the mode to IDLE to stop motor control
      set_mode(IDLE);
      break;
    }
    case 'q': { // quit client
      // Set the mode to IDLE to stop motor control
      set_mode(IDLE);
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
      NU32DIP_YELLOW = !NU32DIP_YELLOW; // Toggle LED1 to indicate success
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