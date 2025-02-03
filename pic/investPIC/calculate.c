#include "calculate.h"

void calculateGrowth(Investment* invp) {
  for (int i = 1; i <= invp->years; i = i + 1) {
    invp->invarray[i] = invp->growth * invp->invarray[i - 1];
  }
}