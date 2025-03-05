#include "utilities.h"

/**
 * @brief Global Program Mode
 *
 */
volatile mode_t mode;

/**
 * @brief Get the mode object
 *
 * @return mode_t
 */
mode_t get_mode() {
  return mode;
}

/**
 * @brief Set the mode object
 *
 * @param mode_t
 */
void set_mode(mode_t new_mode) {
  mode = new_mode;
}