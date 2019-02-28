@echo off
make
set a=%errorlevel%
echo ------------------------------------------
if %a% neq 0 color 04
if %a% equ 0 color 0a

echo -----------------------------------

%systemroot%\system32\timeout.exe 55
cmd /k
