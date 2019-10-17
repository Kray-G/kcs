@echo off
SET OPT=-x
if NOT "%1" == "" SET OPT=%1
SET CC=..\..\kcc.exe %OPT%
SET TEST=check.cmd detail
SET SKIP=skip.cmd

pushd %~dp0
call %TEST% ary.c
call %TEST% block.c
call %TEST% brk_ctn.c
call %TEST% cast.c
call %TEST% ctrl.c
call %TEST% fcall.c
call %TEST% float.c
call %TEST% for.c
call %TEST% funcptr.c
call %TEST% globalv.c
call %TEST% op.c
call %TEST% ptr.c
call %TEST% sizeof.c
call %TEST% struct.c
call %TEST% ternary.c
call %TEST% var.c
popd
