@ECHO OFF

cd /d %~dp0

ECHO ============================
ECHO MAX32664 Flashing msbl file
ECHO ============================


SET index=1

SETLOCAL ENABLEDELAYEDEXPANSION
FOR %%f IN (*.msbl) DO (
   SET file!index!=%%f
   ECHO !index! - %%f
   SET /A index=!index!+1
)

SETLOCAL DISABLEDELAYEDEXPANSION

SET /P selection="select the .mbsl file you want to flash by number:"

SET file%selection% >nul 2>&1

IF ERRORLEVEL 1 (
   ECHO invalid number selected   
   EXIT /B 1
)

CALL :RESOLVE %%file%selection%%%
CSCRIPT //NoLogo //B C:\FCLoader\sendkeys.vbs


set /p COMPORT=Enter the COM port assigned to the MAX32630FTHR, i.e. COM4: 

start cmd.exe /k "flash.exe -f "%file_name%" -p "%COMPORT%"" 

GOTO :EOF

:RESOLVE
SET file_name=%1
GOTO :EOF