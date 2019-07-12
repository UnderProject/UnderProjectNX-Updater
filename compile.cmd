@echo off
set hay=0
set filename=devkitpro.zip
set filed=1EMJOgf32G6OueTOY4abfokQg3LLdN5Vv

		FOR /F "delims=" %%i in ('unzip -h') do (
		set /a hay+=1
		)
		echo --%hay%
		if "%hay%" LSS "2" (
		Title instalando dependencias
		echo Es Nesesaria la lib unzip
		pacman -Sy unzip)

if not exist devkitPro\libnx\switch_rules (
	if not exist devkitpro.zip call:main

		if exist devkitpro.zip (
		Title Descomprimiendo
		unzip -o %filename%
		del %filename%
		)
)
:create
Title Compilando
make
set a=%errorlevel%
echo ------------------------------------------
if %a% neq 0 color 04
if %a% equ 0 color 0a

echo -----------------------------------

%systemroot%\system32\timeout.exe 55
cmd /k


exit

:main
Title Descargando
echo Voy A desacargar la lib
		wget --quiet --save-cookies cookies.txt --keep-session-cookies --no-check-certificate "https://docs.google.com/uc?export=download&id=%filed%" -O- | sed -rn "s/.*confirm=([0-9A-Za-z_]+).*/\1\n/p">save
		FOR /F "delims=" %%i in (save) do (
		wget --load-cookies cookies.txt "https://docs.google.com/uc?export=download&confirm=%%i&id=%filed%" -O FILENAME && rm -rf cookies.txt && rm -rf save
		rename FILENAME %filename%
		)
