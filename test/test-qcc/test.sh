#!/bin/sh
if [ "$1" = "-j" ]; then
	CC="../../kcc -j";
else
	CC="../../kcc -x"
fi

function do_test() {
    $CC -DTEST_NAME=`basename $1` $1
    if [ $? -eq 0 ]; then
        echo Test Passed: $1
    else
        echo Test Failed: $1
    fi
}

pushd `dirname $0`
do_test ary.c
do_test block.c
do_test brk_ctn.c
do_test cast.c
do_test ctrl.c
do_test fcall.c
do_test float.c
do_test for.c
do_test funcptr.c
do_test globalv.c
do_test op.c
do_test ptr.c
do_test sizeof.c
do_test struct.c
do_test ternary.c
do_test var.c
popd



