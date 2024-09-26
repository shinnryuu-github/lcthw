#include "dbg.h"
#include <ctype.h>
#include <stdio.h>

int print_a_message(const char *msg) {
  printf("A STRING: %s\n", msg);
  return 0;
}

int uppercase(const char *msg, size_t len) {
  for (size_t i = 0; i < len; i++) {
    printf("%c", toupper(msg[i]));
  }
  printf("\n");
  return 0;
}

int lowercase(const char *msg, size_t len) {
  for (size_t i = 0; i < len; i++) {
    printf("%c", tolower(msg[i]));
  }
  printf("\n");
  return 0;
}

int fail_on_purpose(const char *msg) { return 1; }
