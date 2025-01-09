#include <stdio.h>

void exercise_21() {
  unsigned char i, j, k;
  i = 60;
  j = 80;
  k = 200;

  unsigned char sum;
  sum = i + j; // (a)
  printf("sum = %d\n", sum);
  sum = i + k; // (b)
  printf("sum = %d\n", sum);
  sum = j + k; // (c)
  printf("sum = %d\n", sum);
}

void exercise_22() {
  int a = 2, b = 3, c;
  float d = 1.0, e = 3.5, f;

  f = a / b;           // (a)
  f = ((float)a) / b; // (b)
  f = (float)(a / b); // (c)
  c = e / d;           // (d)
  c = (int)(e / d);   // (e)
  f = ((int)e) / d;   // (f)
}

void exercise_30() {
  int x[4] = { 4, 3, 2, 1 };

  int a, b, c, d, f, g;
  a = x[1];           // (a)
  b = *x;             // (b)
  c = *(x + 2);         // (c)
  d = (*x) + 2;       // (d)
  //*x[3];          // (e)
  f = x[4];           // (f)
  g = *(&(x[1]) + 1); // (g)

  printf("a = %d, b = %d, c = %d, d = %d, f = %d, g = %d\n", a, b, c, d, f, g);
}

void exercise_31() {
  int i, k = 6;
  i = 3 * (5 > 1) + (k = 2) + (k == 6);
  printf("i = %d,", i);
}

void exercise_32() {
  unsigned char a = 0x00, b = 0x03, c;
  c = ~a;     // (a)
  printf("c = 0x%03X \n", c);
  c = a & b;  // (b)
  printf("c = 0x%03X \n", c);
  c = a | b;  // (c)
  printf("c = 0x%03X \n", c);
  c = a ^ b;  // (d)
  printf("c = 0x%03X \n", c);
  c = a >> 3; // (e)
  printf("c = 0x%03X \n", c);
  c = a << 3; // (f)
  printf("c = 0x%03X \n", c);
  c &= b;     // (g)
  printf("c = 0x%03X \n", c);
}

int main(void) {
  // exercise_21();
  // exercise_22();
  // exercise_30();
  // exercise_31();
  exercise_32();
  // bitwise();

  return 0;
}