#include <stdio.h>
#include "nu32dip.h"
#define MAX_YEARS 100
#define MSG_LEN 25

// Replace all instances of printf and scanf
// with the appropriate combinations of 
// sprintf, sscanf, NU32_ReadUART3, and NU32_WriteUART3

typedef struct {
  double inv0;
  double growth;
  int years;
  double invarray[MAX_YEARS + 1];
} Investment;


int getUserInput(Investment* invp);
void calculateGrowth(Investment* invp);
void sendOutput(double* arr, int years);

int main(void) {
  NU32DIP_Startup(); // cache on, interrupts on, LED/button init, UART init

  Investment inv;
  while (getUserInput(&inv)) {
    inv.invarray[0] = inv.inv0;
    calculateGrowth(&inv);
    sendOutput(inv.invarray, inv.years);
  }
  return 0;
}

void calculateGrowth(Investment* invp) {
  for (int i = 1; i <= invp->years; i = i + 1) {
    invp->invarray[i] = invp->growth * invp->invarray[i - 1];
  }
}
int getUserInput(Investment* invp) {

  int valid;
  char inputString[MSG_LEN];
  sprintf(inputString, "Enter investment, growth rate, number of yrs (up to %d): ", MAX_YEARS);

  // printf("Enter investment, growth rate, number of yrs (up to %d): ", MAX_YEARS);
  NU32DIP_WriteUART1(inputString);
  // scanf("%lf %lf %d", &(invp->inv0), &(invp->growth), &(invp->years));
  NU32DIP_ReadUART1(inputString, MSG_LEN);
  sscanf(inputString, "%lf %lf %d", &(invp->inv0), &(invp->growth), &(invp->years));

  valid = (invp->inv0 > 0) && (invp->growth > 0) &&
    (invp->years > 0) && (invp->years <= MAX_YEARS);

  // printf("Inv: %10.2f GR: %10.2f Yrs: %3d\n", invp->inv0, invp->growth, invp->years);
  char invGRYrs[MSG_LEN];
  sprintf(invGRYrs, "Inv: %10.2f GR: %10.2f Yrs: %3d\r\n", invp->inv0, invp->growth, invp->years);
  NU32DIP_WriteUART1(invGRYrs);

  if (!valid) {
    // printf("Invalid input; exiting.\n");
    NU32DIP_WriteUART1("Invalid input, exiting.\r\n");
  }
  else {
    // printf("Valid input, calculating growth.\n");
    NU32DIP_WriteUART1("Valid input, calculating growth.\r\n");
  }
  return(valid);
}

void sendOutput(double* arr, int yrs) {
  char outstring[100];

  // printf("\nRESULTS:\n\n");
  NU32DIP_WriteUART1("\r\nRESULTS:\r\n\r\n");
  for (int i = 0; i <= yrs; i++) {
    sprintf(outstring, "Year %3d:  %10.2f\r\n", i, arr[i]);
    // printf("%s", outstring);
    NU32DIP_WriteUART1(outstring);
  }
  // printf("\n");
  NU32DIP_WriteUART1("\r\n");
}
