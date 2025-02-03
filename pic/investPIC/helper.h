#ifndef HELPER__H__
#define HELPER__H__

#include <stdio.h>

#define MAX_YEARS 100
#define MSG_LEN 25

typedef struct {
  double inv0;
  double growth;
  int years;
  double invarray[MAX_YEARS + 1];
} Investment;

int getUserInput(Investment* invp);
void calculateGrowth(Investment* invp);
void sendOutput(double* arr, int years);

#endif