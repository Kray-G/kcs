int add(int a, int b) {
  return a + b;
}

int test() {
  int (*fnptr)(int, int) = add;
  return fnptr(2, 3) == 5 ? 0 : 1;
}

#include "main.c"
