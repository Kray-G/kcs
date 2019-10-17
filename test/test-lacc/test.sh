#!/bin/sh

CC="../../kcc -x"
if [ "$CC" = "" ]; then
    exit 1
fi

do_test() {
    TARGET=$1
    EXPECT=`basename ${TARGET%.*}`.expect
    if [ ! -f $EXPECT ]; then
        gcc $TARGET -o expect.exe > /dev/null 2>&1
        ./expect.exe > $EXPECT
        echo Result:$?>> $EXPECT
    fi
    echo "#include <stdio.h>" > code.c
    echo "#include <stdlib.h>" >> code.c
    cat $TARGET >> code.c
    $CC -x code.c > result.txt
    echo Result:$?>> result.txt
    diff --strip-trailing-cr $EXPECT result.txt > /dev/null 2>&1
    if [ $? -eq 1 ]; then
        echo Error: $TARGET
        # diff $EXPECT result.txt
        rm -f $EXPECT
    else
        echo Test Passed: $TARGET
    fi
}

pushd `dirname $0`
do_test address-deref-offset.c
# do_test anonymous-members.c
do_test anonymous-struct.c
do_test array-decay.c
do_test array-nested-init.c
do_test array-param.c
do_test array-registers.c
do_test array-reverse-index.c
do_test array-zero-length.c
do_test array.c
do_test assign-deref-float.c
do_test assignment-type.c
# do_test bitfield-basic.c
# do_test bitfield-extend.c
# do_test bitfield-immediate-assign.c
# do_test bitfield-initialize-zero.c
# do_test bitfield-load.c
do_test bitfield-mask.c
# do_test bitfield-packing.c
do_test bitfield-reset-align.c
# do_test bitfield-trailing-zero.c
# do_test bitfield-types-init.c
# do_test bitfield-types.c
do_test bitfield-unsigned-promote.c
# do_test bitfield.c
do_test bitwise-complement.c
do_test bitwise-constant.c
# do_test bitwise-expression.c
do_test bitwise-sign-extend.c
do_test byte-load.c
do_test cast-float-union.c
do_test cast-float.c
do_test cast-function-args.c
do_test cast-immediate-truncate.c
do_test cast.c
do_test comma-side-effects.c
# do_test comment.c
do_test compare.c
do_test compound-assignment-basic.c
do_test conditional-basic.c
do_test conditional-constant.c
do_test conditional-void.c
do_test conditional.c
do_test constant-address-index.c
do_test constant-expression.c
do_test constant-integer-type.c
do_test convert-assign-immediate.c
do_test convert-float-double.c
# do_test convert-float-int.c
# do_test convert-float-unsigned.c
do_test convert-int-float.c
# do_test convert-unsigned-float.c
do_test copy-struct.c
do_test declaration-default-int.c
do_test declarator-complex.c
do_test declarator-parens.c
do_test declare-auto-func.c
do_test deref-address-offset.c
do_test deref-array.c
# do_test deref-compare-float.c
do_test deref-deep.c
do_test deref-store.c
do_test deref.c
do_test dereference-extern.c
do_test directive-number.c
do_test do-continue.c
do_test do-while.c
do_test duffs-device.c
do_test enum.c
do_test exit.c
do_test expression-div-mod.c
do_test expression.c
do_test fact.c
# do_test float-arithmetic.c
do_test float-branch.c
do_test float-compare-equal.c
do_test float-compare-nan.c
do_test float-compare.c
do_test float-function.c
do_test float-load-deref.c
do_test for-empty-expr.c
do_test for.c
do_test function-char-args.c
# do_test function-implicit-declare.c
do_test function-incomplete.c
do_test function-pointer-call.c
do_test function-pointer.c
# do_test function.c
do_test goto.c
do_test hello.c
do_test identifier.c
do_test immediate-branch.c
do_test immediate-expr.c
# do_test immediate-pointer.c
do_test include.c
do_test increment.c
do_test initialize-address.c
do_test initialize-array.c
do_test initialize-float.c
# do_test initialize-null.c
do_test initialize-string.c
do_test initialize-union.c
# do_test initialize.c
# do_test integer-suffix.c
# do_test ldouble-load-direct.c
# do_test line-continuation.c
# do_test line-directive.c
do_test linebreak.c
do_test liveness-deref-assign.c
do_test liveness-global.c
do_test liveness-loop.c
do_test liveness-pointer.c
do_test logical-and-bitwise-false.c
do_test logical-operators-basic.c
# do_test long-double-arithmetic.c
# do_test long-double-compare.c
# do_test long-double-function.c
# do_test long-double-load.c
# do_test long-double-struct.c
# do_test long-double-union.c
do_test macro-empty-arg.c
do_test macro-function-paren.c
do_test macro-keyword-define.c
do_test macro-name-arg.c
do_test macro-param-space.c
do_test macro-paste.c
do_test macro-predefined.c
do_test macro-recursive.c
do_test macro-refill-expand.c
do_test macro-repeat-expand.c
do_test macro-skip-expand.c
# do_test macro.c
do_test main.c
do_test negate.c
do_test nested-macro.c
# do_test offsetof.c
# do_test old-param-decay.c
do_test old-style-declaration.c
do_test old-style-definition.c
do_test padded-initialization.c
do_test params-mixed.c
do_test params-system-v.c
do_test partial-initialization.c
do_test pointer-diff.c
# do_test pointer.c
do_test preprocess-expression.c
do_test preprocess.c
do_test preprocessor-expression.c
# do_test printstr.c
do_test promote-unsigned.c
do_test prototype-scope-enum.c
do_test ptrdiff.c
do_test qualifier-repeat.c
do_test register-param.c
# do_test return-bitfield.c
do_test return-compare-int.c
do_test return-float-struct.c
do_test return-partial-register.c
do_test return-point.c
do_test return-struct-basic.c
do_test return-struct-integers.c
do_test return-struct-mem.c
do_test self-referential-struct.c
do_test shift-assign.c
# do_test shift.c
do_test short-circuit-comma.c
do_test short-circuit.c
do_test shortcircuit-loop.c
do_test signed-division.c
# do_test sizeof.c
do_test string-addr.c
do_test string-concat.c
do_test string-escape.c
do_test string-index.c
do_test string-length.c
# do_test stringify.c
do_test strings.c
# do_test struct-alignment.c
do_test struct-assign.c
do_test struct-comma-call.c
do_test struct-eightbyte-write.c
do_test struct-init-swap.c
# do_test struct-padding.c
do_test struct.c
do_test switch-basic.c
do_test switch-nested.c
do_test tag.c
do_test tail-compare-jump.c
do_test token.c
do_test tokenize-partial-keyword.c
# do_test trigraph.c
# do_test typedef-function.c
do_test typedef.c
do_test unary-minus-float.c
# do_test unary-plus.c
do_test union-bitfield.c
do_test union-float-assign.c
do_test union-float-param.c
do_test union-zero-init.c
# do_test union.c
do_test unsigned-compare-ge.c
do_test unsigned-compare.c
do_test unsigned-sign-extend.c
do_test vararg-complex-1.c
# do_test vararg-complex-2.c
do_test vararg-deref-arg.c
do_test vararg-deref.c
do_test vararg-float.c
do_test vararg-param.c
do_test vararg.c
do_test void-statement.c
do_test whitespace.c
do_test zero-init.c
popd
exit 0

:ERROR_LIST
pushd `dirname $0`
do_test anonymous-members.c
do_test bitfield-basic.c
do_test bitfield-extend.c
do_test bitfield-immediate-assign.c
do_test bitfield-initialize-zero.c
do_test bitfield-load.c
do_test bitfield-packing.c
do_test bitfield-trailing-zero.c
do_test bitfield-types-init.c
do_test bitfield-types.c
do_test bitfield.c
do_test bitwise-expression.c
do_test comment.c
do_test convert-float-int.c
do_test convert-float-unsigned.c
do_test convert-unsigned-float.c
do_test deref-compare-float.c
do_test float-arithmetic.c
do_test function-implicit-declare.c
do_test function.c
do_test immediate-pointer.c
do_test initialize-null.c
do_test initialize.c
do_test integer-suffix.c
do_test ldouble-load-direct.c
do_test line-continuation.c
do_test line-directive.c
do_test long-double-arithmetic.c
do_test long-double-compare.c
do_test long-double-function.c
do_test long-double-load.c
do_test long-double-struct.c
do_test long-double-union.c
do_test macro.c
do_test offsetof.c
do_test old-param-decay.c
do_test pointer.c
do_test printstr.c
do_test return-bitfield.c
do_test shift.c
do_test sizeof.c
do_test stringify.c
do_test struct-alignment.c
do_test struct-padding.c
do_test trigraph.c
do_test typedef-function.c
do_test unary-plus.c
do_test union.c
do_test vararg-complex-2.c
popd
exit 0
