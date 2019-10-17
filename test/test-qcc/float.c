int test() {
  double x = 2;
  x /= 3;
  int false1 = 0.2 > x;
  int true1  = 0.3 < x;
  int true2  = 0.3 != 3.2;
  return
    (
     true1 &&
     true2 &&
     false1 == 0) ? 0 : 1;
}

#include "main.c"
