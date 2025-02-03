#ifndef IO__H__
#define IO__H__

#include <stdio.h>
#include "calculate.h"

#define MSG_LEN 25

int getUserInput(Investment* invp);
void sendOutput(double* arr, int years);

#endif