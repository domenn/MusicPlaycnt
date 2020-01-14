@echo off

:: We want cmake in path.
if not defined DevEnvDir (
	call "%~dp0load_vcvarsall.bat"
)

if not exist "%~dp0..\vs_proj" mkdir "%~dp0..\vs_proj"
pushd "%~dp0..\vs_proj"
cmake -DVCPKG_TARGET_TRIPLET=x64-windows-dynamicrt-static -DCMAKE_TOOLCHAIN_FILE=Q:\vcpkg\scripts\buildsystems\vcpkg.cmake ..

:: Pause if run from doubleclick. No-pause in CMD.
IF %0 == "%~0"  pause

EXIT /B %ERRORLEVEL%