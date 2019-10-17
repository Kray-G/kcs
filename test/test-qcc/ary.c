int test() {
  int a[10];
  a[1] = 65;
  char s[10];
  s[0] = a[1];
  int b[4] = {1, 3, 5, 12};
  if(b[0] != 1)  return 1;
  if(b[1] != 3)  return 1;
  if(b[2] != 5)  return 1;
  if(b[3] != 12) return 1;
  return 0;
}

#include "main.c"
