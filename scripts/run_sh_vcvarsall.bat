@echo off

if not defined DevEnvDir (
	call "%~dp0load_vcvarsall.bat"
)

set sh_fn=%1
if "%sh_fn%"=="" (
    set sh_fn=install_platformdirs.sh
)

set me_where=%~dp0

if exist "%sh_fn%" (
    call "%sh_fn%"
    exit /B 0
) else if exist "libs\%sh_fn%" (
    call "libs\%sh_fn%"
    exit /B 0
) else if exist "%me_where%%sh_fn%" (
    call "%me_where%%sh_fn%"
    exit /B 0
) else if exist "%me_where%libs\%sh_fn%" (
    call "%me_where%libs\%sh_fn%"
    exit /B 0
)
exit /B 1