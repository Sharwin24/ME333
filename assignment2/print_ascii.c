#include <stdio.h>

int main() {
  printf("ASCII table (33 to 127)\n");
  for (int i = 33; i <= 127; i++) {
    if (i % 10 == 2) {
      printf("\n");
    }
    else {
      printf("%4d: %c", i, i);
    }
  }
  printf("\n");
}