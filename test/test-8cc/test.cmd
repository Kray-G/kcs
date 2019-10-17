@echo off
SET OPT=-x
if NOT "%1" == "" SET OPT=%1
SET CC=..\..\kcc.exe %OPT%

pushd %~dp0
ECHO ---- %CC% align.c
%CC% arith.c
%CC% array.c
%CC% assign.c
%CC% bitop.c
%CC% cast.c
%CC% comp.c
%CC% constexpr.c
%CC% control.c
%CC% conversion.c
%CC% decl.c
%CC% enum.c
%CC% extern.c
%CC% float.c
%CC% funcargs.c
%CC% function.c
REM ECHO ---- %CC% generic.c
%CC% global.c
REM ECHO ---- %CC% import.c
REM ECHO ---- %CC% includeguard.c
%CC% initializer.c
%CC% int.c
REM ECHO ---- %CC% iso646.c
REM ECHO ---- %CC% lex.c
%CC% line.c
REM ECHO ---- %CC% literal.c
REM ECHO ---- %CC% macro.c
%CC% noreturn.c
%CC% number.c
%CC% oldstyle.c
%CC% pointer.c
%CC% scope.c
%CC% sizeof.c
REM ECHO ---- %CC% staticassert.c
REM ECHO ---- %CC% stmtexpr.c
%CC% struct.c
%CC% type.c
ECHO ---- %CC% typeof.c
%CC% union.c
%CC% usualconv.c
%CC% varargs.c
popd
exit /b 0
