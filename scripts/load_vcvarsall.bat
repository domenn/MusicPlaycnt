@echo off

set internal_the_fn=%~dp0

set cmd=powershell -ExecutionPolicy Bypass -File "%internal_the_fn%vsvars.ps1"
FOR /F "tokens=* USEBACKQ" %%F IN (`%cmd%`) DO (
	SET vsvarsbat=%%F
)
call "%vsvarsbat%"
EXIT /B 0