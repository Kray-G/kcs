// Copyright 2012 Rui Ueyama. Released under the MIT license.

#include "test.h"

int t()
{
    _Static_assert(1, "fail");
    return 1;
}

void testmain() {
    print("static assert");
    _Static_assert(1, "fail");
    expect(t(), 1);

    struct {
        _Static_assert(1, "fail");
    } x;
}
