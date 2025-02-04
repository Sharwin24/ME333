#include "io.h"
#include "calculate.h"
#include "nu32dip.h"

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