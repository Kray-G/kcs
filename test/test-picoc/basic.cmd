@echo off
SET OPT=-x
if NOT "%1" == "" SET OPT=%1
SET CC=..\..\kcc.exe %OPT%
SET TEST=check.cmd detail
SET SKIP=skip.cmd

pushd %~dp0
call %TEST% 00_assignment.c
call %TEST% 01_comment.c
call %TEST% 02_printf.c
call %TEST% 03_struct.c
call %TEST% 04_for.c
call %TEST% 05_array.c
call %TEST% 06_case.c
call %TEST% 07_function.c
call %TEST% 08_while.c
call %TEST% 09_do_while.c
call %TEST% 10_pointer.c
call %TEST% 11_precedence.c
call %TEST% 12_hashdefine.c
call %TEST% 13_integer_literals.c
call %TEST% 14_if.c
call %TEST% 15_recursion.c
call %TEST% 16_nesting.c
call %TEST% 17_enum.c
call %TEST% 18_include.c
call %TEST% 19_pointer_arithmetic.c
call %TEST% 20_pointer_comparison.c
call %TEST% 21_char_array.c
call %SKIP% 22_floating_point.c                 "math.h not supported."
call %TEST% 23_type_coercion.c
call %SKIP% 24_math_library.c                   "math.h not supported."
call %TEST% 25_quicksort.c
call %TEST% 26_character_constants.c
call %TEST% 27_sizeof.c
call %TEST% 28_strings.c
call %TEST% 29_array_address.c
call %TEST% 30_hanoi.c
REM call %TEST% 31_args.c arg1 arg2 arg3 arg4
call %TEST% 32_led.c
call %TEST% 33_ternary_op.c
call %TEST% 34_array_assignment.c
call %TEST% 35_sizeof.c
call %TEST% 36_array_initializers.c
call %TEST% 37_sprintf.c
call %TEST% 38_multiple_array_index.c
call %TEST% 39_typedef.c
call %SKIP% 40_stdio.c                          "FILE* with fopen() not supported."
call %TEST% 41_hashif.c
call %TEST% 42_function_pointer.c
call %TEST% 43_void_param.c
call %TEST% 44_scoped_declarations.c
call %TEST% 45_empty_for.c
call %SKIP% 46_grep.c                           "FILE* with fopen() not supported."
call %TEST% 47_switch_return.c
call %TEST% 48_nested_break.c
call %TEST% 49_bracket_evaluation.c
call %TEST% 50_logical_second_arg.c
call %TEST% 51_static.c
call %TEST% 52_unnamed_enum.c
call %TEST% 54_goto.c
call %TEST% 55_array_initializer.c
call %TEST% 56_cross_structure.c
call %TEST% 57_macro_bug.c
call %SKIP% 58_return_outside.c                 "this is for a invalid C code."
call %TEST% 59_break_before_loop.c
call %TEST% 60_local_vars.c
call %TEST% 61_initializers.c
call %TEST% 62_float.c
call %TEST% 63_typedef.c
call %TEST% 64_double_prefix_op.c
call %SKIP% 65_typeless.c                       "this is for a old C code."
call %TEST% 66_printf_undefined.c
call %TEST% 67_macro_crash.c
call %TEST% 68_return.c
call %SKIP% 69_shebang_script.c                 "this is for a non-C code."
popd
exit /b 0
