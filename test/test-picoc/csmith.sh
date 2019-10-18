#!/bin/sh
if [ "$1" = "-j" ]; then
	CC="../../kcc -j";
else
	CC="../../kcc -x"
fi

if [ "$CC" = "" ]; then
    exit 1
fi

do_test() {
    TARGET=$1
    EXPECT=csmith/`basename ${TARGET%.*}`.expect
    $CC csmith/$TARGET > csmith/result.txt
    diff --strip-trailing-cr $EXPECT csmith/result.txt > /dev/null 2>&1
    if [ $? -eq 1 ]; then
        diff $EXPECT csmith/result.txt
    else
        echo Test Passed: $TARGET
    fi
}

pushd `dirname $0`
for i in $(seq 0 110); do
    do_test rand$i.c
done
rm -f csmith/result.txt
popd

