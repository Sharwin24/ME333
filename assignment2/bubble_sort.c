#include <stdio.h>
#include <string.h>

// max length of string input
#define MAXLENGTH 100

/**
 * @brief Asks the user for a string and places it in the str variable.
 *
 * @param str The string to store the user input as a char array.
 */
void getString(char* str);

/**
 * @brief Given a string, print it to the terminal.
 *
 * @param str the string to print.
 */
void printResult(char* str);

/**
 * @brief Determines if the given character ch1 is greater than ch2.
 *
 * @param ch1 The first character to compare.
 * @param ch2 The second character to compare.
 * @return int 1 if ch1 is greater than ch2, 0 otherwise.
 */
int greaterThan(char ch1, char ch2);

/**
 * @brief Given a string, swap the characters at index1 and index2.
 *
 * @note This function assumes the given indices are valid.
 *
 * @param str the string as a char array.
 * @param index1 the index of the first character to swap.
 * @param index2 the index of the second character to swap.
 */
void swap(char* str, int index1, int index2);

/**
 * @brief Performs bubble sort on the given string with the given length.
 *
 * @param str The string to sort as a char array.
 * @param len The length of the string.
 */
void bubble_sort(char* str, int len) {
  for (int i = 0; i < len; i++) {
    for (int j = 0; j < len - i - 1; j++) {
      if (greaterThan(str[j], str[j + 1])) {
        swap(str, j, j + 1);
      }
    }
  }
}


int main(void) {
  int len;
  // length of the entered string
  char str[MAXLENGTH];
  // input should be no longer than MAXLENGTH
  // here, any other variables you need
  getString(str);
  len = strlen(str);
  // get length of the string, from string.h
  // put nested loops here to put the string in sorted order
  bubble_sort(str, len);
  printResult(str);
  return(0);
}
// helper functions go here

void getString(char* str) {
  printf("Enter a string with underscores instead of spaces: ");
  scanf("%s", str);
}

void printResult(char* str) {
  printf("Sorted string: %s\n", str);
}

int greaterThan(char ch1, char ch2) { return ch1 > ch2; }

void swap(char* str, int index1, int index2) {
  char temp = str[index1];
  str[index1] = str[index2];
  str[index2] = temp;
}