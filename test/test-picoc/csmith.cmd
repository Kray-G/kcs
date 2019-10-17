@echo off
SET OPT=-x
if NOT "%1" == "" SET OPT=%1
SET CC=..\..\kcc.exe %OPT%
SET TEST=check.cmd detail
SET FORCE_FAIL=check.cmd fail
SET SKIP=skip.cmd

pushd %~dp0
if "%2" == "" goto TESTALL
call %TEST% csmith\%2
goto END

:TESTALL
call %TEST% csmith\rand0.c
call %TEST% csmith\rand1.c
call %TEST% csmith\rand2.c
call %TEST% csmith\rand3.c
call %TEST% csmith\rand4.c
call %TEST% csmith\rand5.c
call %TEST% csmith\rand6.c
call %TEST% csmith\rand7.c
call %TEST% csmith\rand8.c
call %TEST% csmith\rand9.c
call %TEST% csmith\rand10.c
call %TEST% csmith\rand11.c
call %TEST% csmith\rand12.c
call %TEST% csmith\rand13.c
call %TEST% csmith\rand14.c
call %TEST% csmith\rand15.c
call %TEST% csmith\rand16.c
call %TEST% csmith\rand17.c
call %TEST% csmith\rand18.c
call %TEST% csmith\rand19.c
call %TEST% csmith\rand20.c
call %TEST% csmith\rand21.c
call %TEST% csmith\rand22.c
call %TEST% csmith\rand23.c
call %TEST% csmith\rand24.c
call %TEST% csmith\rand25.c
call %TEST% csmith\rand26.c
call %TEST% csmith\rand27.c
call %TEST% csmith\rand28.c
call %TEST% csmith\rand29.c
call %TEST% csmith\rand30.c
call %TEST% csmith\rand31.c
call %TEST% csmith\rand32.c
call %TEST% csmith\rand33.c
call %TEST% csmith\rand34.c
call %TEST% csmith\rand35.c
call %TEST% csmith\rand36.c
call %TEST% csmith\rand37.c
call %TEST% csmith\rand38.c
call %TEST% csmith\rand39.c
call %TEST% csmith\rand40.c
call %TEST% csmith\rand41.c
call %TEST% csmith\rand42.c
call %TEST% csmith\rand43.c
call %TEST% csmith\rand44.c
call %TEST% csmith\rand45.c
call %TEST% csmith\rand46.c
call %TEST% csmith\rand47.c
call %TEST% csmith\rand48.c
call %TEST% csmith\rand49.c
call %TEST% csmith\rand50.c
call %TEST% csmith\rand51.c
call %TEST% csmith\rand52.c
call %TEST% csmith\rand53.c
call %TEST% csmith\rand54.c
call %TEST% csmith\rand55.c
call %TEST% csmith\rand56.c
call %TEST% csmith\rand57.c
call %TEST% csmith\rand58.c
call %TEST% csmith\rand59.c
call %TEST% csmith\rand60.c
call %TEST% csmith\rand61.c
call %TEST% csmith\rand62.c
call %TEST% csmith\rand63.c
call %TEST% csmith\rand64.c
call %TEST% csmith\rand65.c
call %TEST% csmith\rand66.c
call %TEST% csmith\rand67.c
call %TEST% csmith\rand68.c
call %TEST% csmith\rand69.c
call %TEST% csmith\rand70.c
call %TEST% csmith\rand71.c
call %TEST% csmith\rand72.c
call %TEST% csmith\rand73.c
call %TEST% csmith\rand74.c
call %TEST% csmith\rand75.c
call %TEST% csmith\rand76.c
call %TEST% csmith\rand77.c
call %TEST% csmith\rand78.c
call %TEST% csmith\rand79.c
call %TEST% csmith\rand80.c
call %TEST% csmith\rand81.c
call %TEST% csmith\rand82.c
call %TEST% csmith\rand83.c
call %TEST% csmith\rand84.c
call %TEST% csmith\rand85.c
call %TEST% csmith\rand86.c
call %TEST% csmith\rand87.c
call %TEST% csmith\rand88.c
call %TEST% csmith\rand89.c
call %TEST% csmith\rand90.c
call %TEST% csmith\rand91.c
call %TEST% csmith\rand92.c
call %TEST% csmith\rand93.c
call %TEST% csmith\rand94.c
call %TEST% csmith\rand95.c
call %TEST% csmith\rand96.c
call %TEST% csmith\rand97.c
call %TEST% csmith\rand98.c
call %TEST% csmith\rand99.c
call %TEST% csmith\rand100.c
call %TEST% csmith\rand101.c
call %TEST% csmith\rand102.c
call %TEST% csmith\rand103.c
call %TEST% csmith\rand104.c
call %TEST% csmith\rand105.c
call %TEST% csmith\rand106.c
call %TEST% csmith\rand107.c
call %TEST% csmith\rand108.c
call %TEST% csmith\rand109.c
call %TEST% csmith\rand110.c

:END
popd
exit /b 0
