#ifndef UTILITIES__H__
#define UTILITIES__H__

#include "stdint.h"

typedef enum {
  IDLE, PWM, ITEST, HOLD, TRACK
} mode_t;

/**
 * @brief Global Program Mode
 *
 */
static volatile mode_t mode = IDLE;

static inline mode_t get_mode() { return mode; }
static inline void set_mode(mode_t m) { mode = m; }

#endif // !UTILITIES__H__