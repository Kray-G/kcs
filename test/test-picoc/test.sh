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
    EXPECT=`basename ${TARGET%.*}`.expect
    $CC $TARGET > result.txt
    diff --strip-trailing-cr $EXPECT result.txt > /dev/null 2>&1
    if [ $? -eq 1 ]; then
        diff $EXPECT result.txt
    else
        echo Test Passed: $TARGET
    fi
}

pushd `dirname $0`
do_test 00_assignment.c
do_test 01_comment.c
do_test 02_printf.c
do_test 03_struct.c
do_test 04_for.c
do_test 05_array.c
do_test 06_case.c
do_test 07_function.c
do_test 08_while.c
do_test 09_do_while.c
do_test 10_pointer.c
do_test 11_precedence.c
do_test 12_hashdefine.c
do_test 13_integer_literals.c
do_test 14_if.c
do_test 15_recursion.c
do_test 16_nesting.c
do_test 17_enum.c
do_test 18_include.c
do_test 19_pointer_arithmetic.c
do_test 20_pointer_comparison.c
do_test 21_char_array.c
# do_skip 22_floating_point.c                 "math.h not supported."
do_test 23_type_coercion.c
# do_skip 24_math_library.c                   "math.h not supported."
do_test 25_quicksort.c
do_test 26_character_constants.c
do_test 27_sizeof.c
do_test 28_strings.c
do_test 29_array_address.c
do_test 30_hanoi.c
# do_test 31_args.c arg1 arg2 arg3 arg4
do_test 32_led.c
do_test 33_ternary_op.c
do_test 34_array_assignment.c
do_test 35_sizeof.c
do_test 36_array_initializers.c
do_test 37_sprintf.c
do_test 38_multiple_array_index.c
do_test 39_typedef.c
# do_skip 40_stdio.c                          "FILE* with fopen() not supported."
do_test 41_hashif.c
do_test 42_function_pointer.c
do_test 43_void_param.c
do_test 44_scoped_declarations.c
do_test 45_empty_for.c
# do_skip 46_grep.c                           "FILE* with fopen() not supported."
do_test 47_switch_return.c
do_test 48_nested_break.c
do_test 49_bracket_evaluation.c
do_test 50_logical_second_arg.c
do_test 51_static.c
do_test 52_unnamed_enum.c
do_test 54_goto.c
do_test 55_array_initializer.c
do_test 56_cross_structure.c
do_test 57_macro_bug.c
# do_skip 58_return_outside.c                 "this is for a invalid C code."
do_test 59_break_before_loop.c
do_test 60_local_vars.c
do_test 61_initializers.c
do_test 62_float.c
do_test 63_typedef.c
do_test 64_double_prefix_op.c
# do_skip 65_typeless.c                       "this is for a old C code."
do_test 66_printf_undefined.c
do_test 67_macro_crash.c
do_test 68_return.c
# do_skip 69_shebang_script.c                 "this is for a non-C code."
popd

