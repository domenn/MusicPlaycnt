cmake_minimum_required(VERSION 3.15)
project(musicPlaycnt)

set(CMAKE_CXX_STANDARD 17)

add_executable(musicPlaycnt main.cpp)

find_package(fmt CONFIG REQUIRED)
target_link_libraries(musicPlaycnt PRIVATE fmt::fmt)

find_path(plog_inc NAMES "plog/Log.h" HINTS ${CMAKE_SOURCE_DIR}/ext/installs/include)
message(STATUS ${plog_inc})
message(STATUS "HINTS ${CMAKE_SOURCE_DIR}/ext/installs/include")
target_include_directories(musicPlaycnt PRIVATE ${plog_inc})

#find_package(protobuf CONFIG REQUIRED)
#target_link_libraries(main PRIVATE protobuf::libprotoc protobuf::libprotobuf protobuf::libprotobuf-lite)

#find_package(protobuf CONFIG REQUIRED)
#target_link_libraries(main PRIVATE protobuf::libprotoc protobuf::libprotobuf protobuf::libprotobuf-lite)