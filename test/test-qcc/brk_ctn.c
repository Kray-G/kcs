int test() {
  int count = 0;
  for(int i = 0; i < 10; i++) {
    if(i == 2) continue;
    if(i == 5) break;
    count++;
  }
  return count != 4;
}

#include "main.c"
