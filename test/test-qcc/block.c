int test() {
  int i = 2;
  {
    int i = 3;
  }
  if(i == 2) return 0;
  return 1;
}

#include "main.c"
