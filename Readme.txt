
./vcpkg install fmt:x64-windows-static-md plog:x64-windows-static-md
./vcpkg install spdlog:x64-windows-static-md
./vcpkg install protobuf:x64-windows-static-md
./vcpkg install nlohmann-json:x64-windows-static-md
./vcpkg install magic-enum:x64-windows-static-md

FOR msys (mingw64):
pacman -S unzip mingw-w64-x86_64-cmake

There are ussues with using lib in vs and other at the same time ... For example, when I build platform_folders lib
for gcc, it overwrites the msvc one (musicPlaycnt/ext/installs/cmake). So I should ... create one (run .sh), do cmake generate.
Then rename the cmake (previously noted path) dir and run the sh with other compiler (gcc/msvc) then run cmake for it.
Not convenient at all ... but I will need to rework a bit to make all work transparently.
UPDATE: I have written a workaround for this.


"C:\Program Files\CMake\bin\cmake.exe" -DVCPKG_TARGET_TRIPLET=x64-windows-static-md -DCMAKE_TOOLCHAIN_FILE=C:\vcpkg\scripts\buildsystems\vcpkg.cmake ..
(IN GIT BASH) 'C:\Program Files\CMake\bin\cmake.exe' -DVCPKG_TARGET_TRIPLET=x64-windows -DCMAKE_TOOLCHAIN_FILE='C:\vcpkg\scripts\buildsystems\vcpkg.cmake' ..

To compile externals (might not always be needed):
From CMD (maybe other shells also work): <whatever>\musicPlaycntPLOG\scripts>run_sh_vcvarsall.bat libs\install_platformdirs.sh

alternative (VS generate):
"C:\Program Files\CMake\bin\cmake.exe" -DVCPKG_TARGET_TRIPLET=x64-windows -DCMAKE_TOOLCHAIN_FILE=C:\vcpkg\scripts\buildsystems\vcpkg.cmake ..