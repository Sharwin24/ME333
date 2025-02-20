#include "nu32dip.h"

#define ISR_FREQ 1000 // 1kHz
#define PR2_VAL ((NU32DIP_SYS_FREQ / ISR_FREQ) / 256) - 1 // period value for 1kHz with 256 prescaler

#define NUMSAMPS 1000 // number of points in waveform
#define CENTER 500
#define AMPLITUDE 300
#define PLOTPTS 200 // number of data points to plot
#define DECIMATION 10 // plot every 10th point
#define EINT_MAX 1000000 // maximum error integral value

static volatile int Waveform[NUMSAMPS]; // waveform
static volatile int ADCarray[PLOTPTS]; // measured values to plot
static volatile int REFarray[PLOTPTS]; // reference values to plot
static voltale int StoringData = 0; // if this flag = 1, currently storing

static volatile float Kp = 0, Ki = 0; // Controller gains
static volatile int Eint = 0; // Integral error

/**
 * @brief Creates a waveform and stores it in the global array Waveform
 *
 * @param center the center value of the waveform
 * @param A the amplitude of the waveform
 */
void makeWaveform() {
  for (int i = 0; i < NUMSAMPS; ++i) {
    Waveform[i] = (i < NUMSAMPS / 2) ? (CENTER + AMPLITUDE) : (CENTER - AMPLITUDE);
  }
}

void configure_ISR() {
  __builtin_disable_interrupts();
  // Configure Timer2 to call an ISR at a frequency of 1 kHz with a priority of 5
  T2CONbits.TCKPS = 0b111; // Timer2 prescaler N=256 (1:256)
  PR2 = PR2_VAL; // period = (PR2+1) * N * 12.5 ns = 1 ms, 1 kHz
  TMR2 = 0; // initial TMR2 count is 0
  T2CONbits.ON = 1; // turn on Timer2
  // Remap OC3 to pin RPA3
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

int ReadADC() {
  // PhotoTransistor is connected to pin 2: RA0
  AD1CHSbits.CH0SA = 0; // connect pin 2 to MUXA for CH0
  AD1CON1bits.SAMP = 1; // start sampling
  while (!AD1CON1bits.DONE) {
    ; // wait for conversion
  }
  AD1CON1bits.SSRC = 0; // clear SSRC to stop sampling
  AD1CON1bits.SAMP = 0; // clear SAMP to start next conversion
  return ADC1BUF0; // read the buffer with the result
}

int clamp(int val, int min, int max) {
  return val < min ? min : (val > max ? max : val);
}

void __ISR(_TIMER_2_VECTOR, IPL5SOFT) Controller(void) {   // _TIMER_2_VECTOR = 8
  // Define static variables that only initialize once
  static int counter = 0; // initialize counter
  static int plotind = 0; // index for plotting
  static int decctr = 0; // decimation counter
  static int adcval = 0; // ADC value

  // Read the ADC Value and set OC3RS using controller gains
  adcval = ReadADC();
  float error = (float)(adcval - Waveform[counter]);
  float u = Kp * error + Ki * (float)Eint;
  Eint += error;
  Eint = clamp(Eint, -EINT_MAX, EINT_MAX);

  float unew = clamp(u + 50.0, 0.0, 100.0);
  OC3RS = (unsigned int)((unew / 100.0) * PR3_VAL);

  if (StoringData) {
    decctr++;
    if (decctr == DECIMATION) {
      decctr = 0;
      ADCarray[plotind] = adcval;
      REFarray[plotind] = Waveform[counter];
      plotind++;
    }
    if (plotind == PLOTPTS) {
      StoringData = 0;
    }
  }

  // Rollover the counter
  counter++;
  if (counter == NUMSAMPS) {
    counter = 0;
  }

  // Clear Interrupt flag
  IFS0bits.T2IF = 0;
}

int main(void) {
  NU32DIP_Startup();

  makeWaveform();
  configure_ISR();

  chat message[100];
  float kptemp = 0, kitemp = 0;
  int i = 0;

  while (1) {
    NU32_ReadUART3(message, sizeof(message));
    sscanf(message, "%f %f", &kptemp, &kitemp);
    __builtin_disable_interrupts();
    Kp = kptemp;
    Ki = kitemp;
    __builtin_enable_interrupts();
    Eint = 0;
    StoringData = 1;
    while (StoringData) {
      ;
    }
    NU32_WriteUART3("Data stored\r\n");
    for (i = 0; i < PLOTPTS; i++) {
      sprintf(message, "%d %d %d\r\n", PLOTPTS - 1, ADCarray[i], REFarray[i]);
      NU32_WriteUART3(message);
    }
  }

  return 0;
}