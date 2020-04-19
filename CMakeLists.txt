cmake_minimum_required(VERSION 3.15)
project(musicPlaycnt)

set(CMAKE_CXX_STANDARD 17)

add_executable(musicPlaycnt src/main.cpp src/misc/plog_util.cc src/misc/plog_util.hpp)

find_package(fmt CONFIG REQUIRED)

if(MSVC)
  find_package(platform_folders REQUIRED HINTS ${CMAKE_SOURCE_DIR}/ext/installs_msvc)
else()
  find_package(platform_folders REQUIRED HINTS ${CMAKE_SOURCE_DIR}/ext/installs)
endif()
#if(${platform_folders_FOUND})
#  message(STATUS "fnd lib: ${platform_folders_DIR}")
#endif()

target_link_libraries(musicPlaycnt PRIVATE fmt::fmt sago::platform_folders)

find_path(plog_inc NAMES "plog/Log.h" HINTS ${CMAKE_SOURCE_DIR}/ext/installs/include)
#message(STATUS ${plog_inc})
#message(STATUS "HINTS ${CMAKE_SOURCE_DIR}/ext/installs/include")
target_include_directories(musicPlaycnt PRIVATE ${plog_inc})
target_compile_definitions(musicPlaycnt PRIVATE PLOG_CAPTURE_FILE)



#find_package(protobuf CONFIG REQUIRED)
#target_link_libraries(main PRIVATE protobuf::libprotoc protobuf::libprotobuf protobuf::libprotobuf-lite)

#find_package(protobuf CONFIG REQUIRED)
#target_link_libraries(main PRIVATE protobuf::libprotoc protobuf::libprotobuf protobuf::libprotobuf-lite)