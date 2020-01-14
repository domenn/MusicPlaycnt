
./vcpkg install fmt:x64-windows-dynamicrt-static plog:x64-windows-dynamicrt-static
./vcpkg install protobuf:x64-windows-dynamicrt-static
./vcpkg install nlohmann-json:x64-windows-dynamicrt-static
./vcpkg install magic-enum:x64-windows-dynamicrt-static

FOR msys (mingw64):
pacman -S unzip mingw-w64-x86_64-cmake
