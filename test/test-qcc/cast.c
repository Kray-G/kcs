int test() {
  int f = (int)1.32;
  struct { int a, b; } *s;
  void *vs = (void *)s;
  if(f != 1) return 1;
  return 0;
}

#include "main.c"
