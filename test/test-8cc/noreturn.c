// Copyright 2014 Rui Ueyama. Released under the MIT license.

#include "test.h"
#include <stdnoreturn.h>

// _Noreturn is ignored
_Noreturn void f1();
noreturn void f2();
inline int f3() { return 1; }

void testmain() {
    print("noreturn");
    expect(1, f3());
}
