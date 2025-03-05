#ifndef UTILITIES__H__
#define UTILITIES__H__

#include "stdint.h"

typedef enum {
  IDLE, PWM, ITEST, HOLD, TRACK
} mode_t;

mode_t get_mode();
void set_mode(mode_t mode);

#endif // !UTILITIES__H__