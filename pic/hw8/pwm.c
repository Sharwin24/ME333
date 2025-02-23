#include "nu32dip.h"

#define ISR_FREQ 1000 // 1kHz
#define PR2_VAL ((NU32DIP_SYS_FREQ / ISR_FREQ) / 256) - 1 // period value for 1kHz with 256 prescaler

#define NUMSAMPS 1000 // number of points in waveform
#define CENTER 500
#define AMPLITUDE 300
#define PLOTPTS 200 // number of data points to plot
#define DECIMATION 10 // plot every 10th point
#define SAMPLE_TIME 6 // 24MHz*250ns

static volatile int Waveform[NUMSAMPS]; // waveform
static volatile int ADCarray[PLOTPTS]; // measured values to plot
static volatile int REFarray[PLOTPTS]; // reference values to plot
static volatile int StoringData = 0; // if this flag = 1, currently storing

static volatile float Kp = 0, Ki = 0; // Controller gains
static volatile int Eint = 0; // Integral error
static volatile int Eint_max = 400; // maximum error integral value

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

void ADC_Startup() {
  __builtin_disable_interrupts();
  ANSELAbits.ANSA1 = 1; // AN1 is an adc pin
  AD1CON3bits.ADCS = 1; // ADC clock period is Tad = 2*(ADCS+1)*Tpb = 2 * 2 * (1 / 48000000Hz) = 83ns > 75ns
  AD1CON1bits.ADON = 1;
  __builtin_enable_interrupts();
}

unsigned int adc_sample_convert() {
  // unsigned int elapsed = 0, finish_time = 0;
  // PhotoTransistor is connected to pin 3: AN1
  AD1CHSbits.CH0SA = 1;
  AD1CON1bits.SAMP = 1;
  _CP0_SET_COUNT(0);
  // finish_time = elapsed + SAMPLE_TIME;
  while (_CP0_GET_COUNT() < SAMPLE_TIME) {
    ;
  }
  AD1CON1bits.SAMP = 0;
  while (!AD1CON1bits.DONE) {
    ;
  }
  return ADC1BUF0;
}

void __ISR(_TIMER_2_VECTOR, IPL5SOFT) Controller(void) {   // _TIMER_2_VECTOR = 8
  // Define static variables that only initialize once
  static int counter = 0; // initialize counter
  static int plotind = 0; // index for plotting
  static int decctr = 0; // decimation counter
  static int adcval = 0; // ADC value

  // Read the ADC Value and set OC3RS using controller gains
  adcval = adc_sample_convert();
  int error = Waveform[counter] - adcval;
  float u = Kp * error + Ki * Eint;
  Eint += error;
  if (Eint > Eint_max) {
    Eint = Eint_max;
  }
  else if (Eint < -Eint_max) {
    Eint = -Eint_max;
  }
  float unew = u + 50.0;
  if (unew < 0) {
    unew = 0;
  }
  else if (unew > 100) {
    unew = 100;
  }
  OC3RS = (unsigned int)((unew / 100.0) * PR2_VAL);

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
  ADC_Startup();
  configure_ISR();

  char message[100];
  float kptemp = 0, kitemp = 0;
  int eint_maxtemp = 0;
  int i = 0;
  while (1) {
    NU32DIP_ReadUART1(message, sizeof(message));
    sscanf(message, "%f %f %f", &kptemp, &kitemp, &eint_maxtemp);
    __builtin_disable_interrupts();
    Kp = kptemp;
    Ki = kitemp;
    Eint_max = eint_maxtemp;
    __builtin_enable_interrupts();
    Eint = 0;
    StoringData = 1;
    while (StoringData) {}
    for (i = 0; i < PLOTPTS; i++) {
      sprintf(message, "%d %d %d\r\n", PLOTPTS - i, ADCarray[i], REFarray[i]);
      NU32DIP_WriteUART1(message);
    }
  }

  return 0;
}