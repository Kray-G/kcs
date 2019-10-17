#!/bin/sh
if [ "$1" = "-j" ]; then
	CC="../../kcc -j";
else
	CC="../../kcc -x"
fi

pushd `dirname $0`
echo ---- $CC align.c
$CC arith.c
$CC array.c
$CC assign.c
$CC bitop.c
$CC cast.c
$CC comp.c
$CC constexpr.c
$CC control.c
$CC conversion.c
$CC decl.c
$CC enum.c
$CC extern.c
$CC float.l.c
$CC funcargs.c
$CC function.c
# echo ---- $CC generic.c
$CC global.c
# echo ---- $CC import.c
# echo ---- $CC includeguard.c
$CC initializer.c
$CC int.c
# echo ---- $CC iso646.c
# echo ---- $CC lex.c
$CC line.c
# echo ---- $CC literal.c
# echo ---- $CC macro.c
$CC noreturn.c
$CC number.c
$CC oldstyle.c
$CC pointer.c
$CC scope.c
$CC sizeof.c
# echo ---- $CC staticassert.c
# echo ---- $CC stmtexpr.c
$CC struct.c
$CC type.c
echo ---- $CC typeof.c
$CC union.c
$CC usualconv.c
$CC varargs.c
popd
exit 0


