#ifndef CALCULATE__H__
#define CALCULATE__H__

#define MAX_YEARS 100

typedef struct {
  double inv0;
  double growth;
  int years;
  double invarray[MAX_YEARS + 1];
} Investment;

void calculateGrowth(Investment* invp);

#endif