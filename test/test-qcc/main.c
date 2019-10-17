#include <stdio.h>

#define TNS(name)   "test " #name
#define TN(name)    TNS(name)

int main(void) {
  int ret_code = test();
//   if(ret_code == 0)
//     printf(TN(TEST_NAME) " ... OK\n");
//   else
//     printf(TN(TEST_NAME) " ... FAILED\n");
  return ret_code;
}
