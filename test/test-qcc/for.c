void loop() {
  for(int i = 0; i < 10; i++)
    i + 2;
  return;
}

int test() {
  int i, sum;
  sum = 0;
  loop();
  for(i = 1; i <= 10; i += 1)
    sum += i;
  return 0;
}

#include "main.c"
