@echo off
if "%1"=="fail" goto SHIFTED_ERROR
SET DETAIL=false
if "%1"=="detail" goto SHIFTED
goto TEST_CHECK

:SHIFTED_ERROR
shift
goto ERROR

:SHIFTED
SET DETAIL=true
shift

:TEST_CHECK
%CC% %1 %2 %3 %4 %5 %6 %7 %8 %9 > result.txt
fc result.txt %~dpn1.expect > NUL 2>&1
if ERRORLEVEL 1 goto ERROR
REM type result.txt
ECHO Test Passed: %~n1
del result.txt
exit /b 0

:ERROR
ECHO *** Test Failed: %~n1
if "%DETAIL%"=="true" fc result.txt %~dpn1.expect
exit /b 1
