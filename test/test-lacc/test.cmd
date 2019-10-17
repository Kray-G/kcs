@echo off
SET OPT=-x
if NOT "%1" == "" SET OPT=%1
SET CC=..\..\kcc.exe %OPT%
SET GEN=..\..\kcc.exe -j
if NOT "%1" == "" SET GEN=..\..\kcc.exe -x

:CLEANUP
pushd %~dp0
del *.expect > NUL 2>&1
del *.result > NUL 2>&1

:DO_TEST
REM c11
call :TEST c11\alignof.c
call :TEST c11\static-assert.c

REM c99
call :TEST c99\_Pragma.c
call :TEST c99\__func__.c
call :TEST c99\array-param-qualifier.c
call :TEST c99\array-qualifier-static.c
call :TEST c99\bool.c
call :TEST c99\compund-literal.c
call :TEST c99\designator-array.c
call :TEST c99\designator-struct.c
call :TEST c99\designator-union.c
call :TEST c99\flexible-array-arg.c
call :TEST c99\flexible-array.c
call :TEST c99\flexible-union.c
call :TEST c99\for-declaration.c
call :TEST c99\line-comment.c
call :TEST c99\restrict.c
call :TEST c99\va_copy.c
call :TEST c99\variadic-macro.c
REM call :TEST c99\vla-arg.c
REM call :TEST c99\vla-block-declaration.c
REM call :TEST c99\vla-compatibility.c
REM call :TEST c99\vla-matrix.c
REM call :TEST c99\vla-old-param.c
REM call :TEST c99\vla-pointer.c
REM call :TEST c99\vla-sizeof.c
REM call :TEST c99\vla-static-pointer.c
REM call :TEST c99\vla-ternary-length.c
REM call :TEST c99\vla.c

REM gnu
REM call :TEST gnu\alloca.c
call :TEST gnu\assign-constant-float.c
call :TEST gnu\cast-float-overflow.c
call :TEST gnu\large-objects.c
REM call :TEST gnu\long-double-convert.c

REM standard
call :TEST address-deref-offset.c
call :TEST anonymous-members.c
call :TEST anonymous-struct.c
call :TEST array-decay.c
call :TEST array-nested-init.c
call :TEST array-param.c
call :TEST array-registers.c
call :TEST array-reverse-index.c
call :TEST array-zero-length.c
call :TEST array.c
call :TEST assign-deref-float.c
call :TEST assignment-type.c
REM call :TEST bitfield-basic.c
REM call :TEST bitfield-extend.c
REM call :TEST bitfield-immediate-assign.c
REM call :TEST bitfield-initialize-zero.c
REM call :TEST bitfield-load.c
call :TEST bitfield-mask.c
REM call :TEST bitfield-packing.c
call :TEST bitfield-reset-align.c
call :TEST bitfield-trailing-zero.c
REM call :TEST bitfield-types-init.c
REM call :TEST bitfield-types.c
call :TEST bitfield-unsigned-promote.c
REM call :TEST bitfield.c
call :TEST bitwise-complement.c
call :TEST bitwise-constant.c
call :TEST bitwise-expression.c
call :TEST bitwise-sign-extend.c
call :TEST byte-load.c
call :TEST cast-float-union.c
call :TEST cast-float.c
call :TEST cast-function-args.c
call :TEST cast-immediate-truncate.c
call :TEST cast.c
call :TEST comma-side-effects.c
call :TEST comment.c
call :TEST compare.c
call :TEST compound-assignment-basic.c
call :TEST conditional-basic.c
call :TEST conditional-constant.c
call :TEST conditional-void.c
call :TEST conditional.c
call :TEST constant-address-index.c
call :TEST constant-expression.c
call :TEST constant-integer-type.c
call :TEST convert-assign-immediate.c
call :TEST convert-float-double.c
call :TEST convert-float-int.c
REM call :TEST convert-float-unsigned.c
call :TEST convert-int-float.c
call :TEST convert-unsigned-float.c
call :TEST copy-struct.c
call :TEST declaration-default-int.c
call :TEST declarator-complex.c
call :TEST declarator-parens.c
call :TEST declare-auto-func.c
call :TEST deref-address-offset.c
call :TEST deref-array.c
REM call :TEST deref-compare-float.c
call :TEST deref-deep.c
call :TEST deref-store.c
call :TEST deref.c
call :TEST dereference-extern.c
call :TEST directive-number.c
call :TEST do-continue.c
call :TEST do-while.c
call :TEST duffs-device.c
call :TEST enum.c
call :TEST exit.c
call :TEST expression-div-mod.c
call :TEST expression.c
call :TEST fact.c
call :TEST float-arithmetic.c
call :TEST float-branch.c
call :TEST float-compare-equal.c
call :TEST float-compare-nan.c
call :TEST float-compare.c
call :TEST float-function.c
call :TEST float-load-deref.c
call :TEST for-empty-expr.c
call :TEST for.c
call :TEST function-char-args.c
REM call :TEST function-implicit-declare.c
call :TEST function-incomplete.c
call :TEST function-pointer-call.c
call :TEST function-pointer.c
call :TEST function.c
call :TEST goto.c
call :TEST hello.c
call :TEST identifier.c
call :TEST immediate-branch.c
call :TEST immediate-expr.c
REM call :TEST immediate-pointer.c
call :TEST include.c
call :TEST increment.c
call :TEST initialize-address.c
call :TEST initialize-array.c
call :TEST initialize-float.c
call :TEST initialize-null.c
call :TEST initialize-string.c
call :TEST initialize-union.c
call :TEST initialize.c
call :TEST integer-suffix.c
call :TEST ldouble-load-direct.c
call :TEST line-continuation.c
call :TEST line-directive.c
call :TEST linebreak.c
call :TEST liveness-deref-assign.c
call :TEST liveness-global.c
call :TEST liveness-loop.c
call :TEST liveness-pointer.c
call :TEST logical-and-bitwise-false.c
call :TEST logical-operators-basic.c
REM call :TEST long-double-arithmetic.c
REM call :TEST long-double-compare.c
REM call :TEST long-double-function.c
REM call :TEST long-double-load.c
REM call :TEST long-double-struct.c
REM call :TEST long-double-union.c
call :TEST macro-empty-arg.c
call :TEST macro-function-paren.c
call :TEST macro-keyword-define.c
call :TEST macro-name-arg.c
call :TEST macro-param-space.c
call :TEST macro-paste.c
call :TEST macro-predefined.c
call :TEST macro-recursive.c
call :TEST macro-refill-expand.c
call :TEST macro-repeat-expand.c
call :TEST macro-skip-expand.c
call :TEST macro.c
call :TEST main.c
call :TEST negate.c
call :TEST nested-macro.c
call :TEST offsetof.c
call :TEST old-param-decay.c
call :TEST old-style-declaration.c
call :TEST old-style-definition.c
call :TEST padded-initialization.c
call :TEST params-mixed.c
call :TEST params-system-v.c
call :TEST partial-initialization.c
call :TEST pointer-diff.c
call :TEST pointer.c
call :TEST preprocess-expression.c
call :TEST preprocess.c
call :TEST preprocessor-expression.c
REM call :TEST printstr.c
call :TEST promote-unsigned.c
call :TEST prototype-scope-enum.c
call :TEST ptrdiff.c
call :TEST qualifier-repeat.c
call :TEST register-param.c
REM call :TEST return-bitfield.c
call :TEST return-compare-int.c
call :TEST return-float-struct.c
call :TEST return-partial-register.c
call :TEST return-point.c
call :TEST return-struct-basic.c
call :TEST return-struct-integers.c
call :TEST return-struct-mem.c
call :TEST self-referential-struct.c
call :TEST shift-assign.c
call :TEST shift.c
call :TEST short-circuit-comma.c
call :TEST short-circuit.c
call :TEST shortcircuit-loop.c
call :TEST signed-division.c
call :TEST sizeof.c
call :TEST string-addr.c
call :TEST string-concat.c
call :TEST string-escape.c
call :TEST string-index.c
call :TEST string-length.c
REM call :TEST stringify.c
call :TEST strings.c
call :TEST struct-alignment.c
call :TEST struct-assign.c
call :TEST struct-comma-call.c
call :TEST struct-eightbyte-write.c
call :TEST struct-init-swap.c
call :TEST struct-padding.c
call :TEST struct.c
call :TEST switch-basic.c
call :TEST switch-nested.c
call :TEST tag.c
call :TEST tail-compare-jump.c
call :TEST token.c
call :TEST tokenize-partial-keyword.c
call :TEST trigraph.c
REM call :TEST typedef-function.c
call :TEST typedef.c
call :TEST unary-minus-float.c
REM call :TEST unary-plus.c
call :TEST union-bitfield.c
call :TEST union-float-assign.c
call :TEST union-float-param.c
call :TEST union-zero-init.c
call :TEST union.c
call :TEST unsigned-compare-ge.c
call :TEST unsigned-compare.c
call :TEST unsigned-sign-extend.c
call :TEST vararg-complex-1.c
REM call :TEST vararg-complex-2.c
call :TEST vararg-deref-arg.c
call :TEST vararg-deref.c
call :TEST vararg-float.c
call :TEST vararg-param.c
call :TEST vararg.c
call :TEST void-statement.c
call :TEST whitespace.c
call :TEST zero-init.c
popd
exit /b 0

:ERROR_LIST
pushd `dirname $0`
call :TEST anonymous-members.c
call :TEST bitfield-basic.c
call :TEST bitfield-extend.c
call :TEST bitfield-immediate-assign.c
call :TEST bitfield-initialize-zero.c
call :TEST bitfield-load.c
call :TEST bitfield-packing.c
call :TEST bitfield-trailing-zero.c
call :TEST bitfield-types-init.c
call :TEST bitfield-types.c
call :TEST bitfield.c
call :TEST bitwise-expression.c
call :TEST comment.c
call :TEST convert-float-int.c
call :TEST convert-float-unsigned.c
call :TEST convert-unsigned-float.c
call :TEST deref-compare-float.c
call :TEST float-arithmetic.c
call :TEST function-implicit-declare.c
call :TEST function.c
call :TEST immediate-pointer.c
call :TEST initialize-null.c
call :TEST initialize.c
call :TEST integer-suffix.c
call :TEST ldouble-load-direct.c
call :TEST line-continuation.c
call :TEST line-directive.c
call :TEST long-double-arithmetic.c
call :TEST long-double-compare.c
call :TEST long-double-function.c
call :TEST long-double-load.c
call :TEST long-double-struct.c
call :TEST long-double-union.c
call :TEST macro.c
call :TEST offsetof.c
call :TEST old-param-decay.c
call :TEST pointer.c
call :TEST printstr.c
call :TEST return-bitfield.c
call :TEST shift.c
call :TEST sizeof.c
call :TEST stringify.c
call :TEST struct-alignment.c
call :TEST struct-padding.c
call :TEST trigraph.c
call :TEST typedef-function.c
call :TEST unary-plus.c
call :TEST union.c
call :TEST vararg-complex-2.c
popd
exit /b 0

:TEST
REM echo Testing %~dpn1...
if NOT EXIST %~dpn1.expect call :GEN1 %1
echo #include ^<stdio.h^> > test.c
echo #include ^<stdlib.h^> >> test.c
type %~dpn1.c >> test.c
%CC% test.c> result.txt
SET RES=Result:%ERRORLEVEL%
echo %RES%>> result.txt
fc result.txt %~dpn1.expect > NUL 2>&1
if ERRORLEVEL 1 goto ERROR
REM type result.txt
ECHO Test Passed: [%CC%] %1
del result.txt
del %~dpn1.expect
exit /b 0

:ERROR
ECHO ........        Test Failed: %1
REM fc result.txt %~dpn1.expect
REM del %~dpn1.expect
ren result.txt %~n1.result
exit /b 1

:GEN1
echo #include ^<stdio.h^> > test.c
echo #include ^<stdlib.h^> >> test.c
type %~dpn1.c >> test.c
%GEN% test.c> %~dpn1.expect
SET RES=Result:%ERRORLEVEL%
echo %RES%>> %~dpn1.expect
del test.c
exit /b 0
