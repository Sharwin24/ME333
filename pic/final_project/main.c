#include "nu32dip.h"


#define BUF_SIZE 200
int main(void) {
  NU32DIP_Startup();

  char buffer[BUF_SIZE];
  NU32DIP_GREEN = 1;
  NU32DIP_YELLOW = 1;
  __builtin_disable_interrupts();
  // in future, initialize modules or peripherals here
  __builtin_enable_interrupts();
  while (1) {
    NU32DIP_ReadUART1(buffer, BUF_SIZE); // we expect the next character to be a menu command
    NU32DIP_YELLOW = 1; // clear the error LED
    switch (buffer[0]) {
    case 'd': {
      int n = 0;
      NU32DIP_ReadUART1(buffer, BUF_SIZE);
      sscanf(buffer, "%d", &n);
      sprintf(buffer, "%d\r\n", n + 1); // return the number + 1
      NU32DIP_WriteUART1(buffer);
      NU32DIP_GREEN = !NU32DIP_GREEN; // toggle LED1
      break;
    }
    case 'q': {
      // handle q for quit. Later you may want to return to IDLE mode here.
      break;
    }
    default: {
      NU32DIP_YELLOW = 0; // turn on LED2 to indicate an error
      break;
    }
    }
  }
  return 0;
}