
./vcpkg install fmt:x64-windows-dynamicrt-static plog:x64-windows-dynamicrt-static
./vcpkg install protobuf:x64-windows-dynamicrt-static
./vcpkg install nlohmann-json:x64-windows-dynamicrt-static
./vcpkg install magic-enum:x64-windows-dynamicrt-static

FOR msys (mingw64):
pacman -S unzip mingw-w64-x86_64-cmake

There are ussues with using lib in vs and other at the same time ... For example, when I build platform_folders lib
for gcc, it overwrites the msvc one (musicPlaycnt/ext/installs/cmake). So I should ... create one (run .sh), do cmake generate.
Then rename the cmake (previously noted path) dir and run the sh with other compiler (gcc/msvc) then run cmake for it.
Not convenient at all ... but I will need to rework a bit to make all work transparently.
UPDATE: I have written a workaround for this.