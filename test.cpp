#include <stdio.h>

void print(void *arg) { printf("clean up: %s\n", (char *)arg); }

int main() {
  const char a[10] = "1";
  printf("%s\n", a);
  print((void *)a);
  return 0;
}
