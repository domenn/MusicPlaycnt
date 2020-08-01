cmake_minimum_required(VERSION 3.15)
project(musicPlaycnt)

set(CMAKE_CXX_STANDARD 17)
include_directories(${CMAKE_SOURCE_DIR})


if (WIN32)
  option(DCPPSOFTWARE_MSVC_JUST_MY_CODE_DEBUGGING "Allow for \"Just my Code\" in Visual Studio." ON)
  if (DCPPSOFTWARE_MSVC_JUST_MY_CODE_DEBUGGING)
    if (NOT ${CMAKE_VERSION} VERSION_LESS "3.15.0")
      set(CMAKE_VS_JUST_MY_CODE_DEBUGGING ON)
    elseif (MSVC)
      message(WARNING "You are using MSVC. Please upgrade CMake to 3.15 to enable support for Just My Code debugging.")
    endif ()
  endif ()
  
  add_definitions(-DUNICODE -D_UNICODE)
  if (MSVC)
    set(CMAKE_CONFIGURATION_TYPES Debug Release CACHE INTERNAL "Debug and release configurations" FORCE)
    add_definitions(/MP)
    get_directory_property(OUT_CO_TOP COMPILE_OPTIONS)
    if (NOT "/utf-8" IN_LIST OUT_CO_TOP)
      add_compile_options(/utf-8)
    endif ()
  endif ()
endif ()

file(GLOB_RECURSE _source_list *.cc* *.h* *.hpp*)
foreach (_source IN ITEMS ${_source_list})
  get_filename_component(_source_path "${_source}" PATH)
  string(REPLACE "${CMAKE_SOURCE_DIR}" "" _group_path "${_source_path}")
  string(REPLACE "/" "\\" _group_path "${_group_path}")
  source_group("${_group_path}" FILES "${_source}")
endforeach ()

file(GLOB_RECURSE musicPlaycntSRC LIST_DIRECTORIES false
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Res/*.rc"
    "${CMAKE_CURRENT_SOURCE_DIR}/Res/*.hpp")

if (MSVC)
  list(APPEND musicPlaycntSRC ${CMAKE_SOURCE_DIR}/external/StackWalker.cpp)
  list(APPEND musicPlaycntSRC ${CMAKE_SOURCE_DIR}/external/file_watcher/FileWatcherWin32.cpp)
  list(APPEND musicPlaycntSRC ${CMAKE_SOURCE_DIR}/external/file_watcher/FileWatcher.cpp)
endif ()


add_executable(musicPlaycnt ${musicPlaycntSRC})

if (WIN32)
  if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set_target_properties(musicPlaycnt PROPERTIES LINK_FLAGS_DEBUG "/SUBSYSTEM:CONSOLE /ENTRY:wWinMainCRTStartup")
    set_target_properties(musicPlaycnt PROPERTIES COMPILE_DEFINITIONS_DEBUG "_CONSOLE")
    set_target_properties(musicPlaycnt PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS")
    set_target_properties(musicPlaycnt PROPERTIES LINK_FLAGS_MINSIZEREL "/SUBSYSTEM:WINDOWS")
  else ()
    set_target_properties(musicPlaycnt PROPERTIES LINK_FLAGS " -municode")
  endif ()
else ()
endif (WIN32)

target_include_directories(musicPlaycnt PRIVATE ${CMAKE_SOURCE_DIR}/external/include)

find_package(fmt CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(Protobuf REQUIRED)

find_package(GTest CONFIG)
if (GTest_FOUND)
  include(${CMAKE_SOURCE_DIR}/test/gtest_project.cmake)
endif ()


if (COMMAND protobuf_generate)
  protobuf_generate(LANGUAGE cpp TARGET musicPlaycnt PROTOC_OUT_DIR ${CMAKE_CURRENT_SOURCE_DIR} PROTOS
      ${CMAKE_CURRENT_SOURCE_DIR}/src/protobufs/app_config.proto ${CMAKE_CURRENT_SOURCE_DIR}/src/protobufs/songs.proto)
endif ()

if (MSVC)
  find_package(platform_folders REQUIRED HINTS ${CMAKE_SOURCE_DIR}/ext/installs_msvc)
elseif (UNIX)
  find_package(platform_folders REQUIRED HINTS ${CMAKE_SOURCE_DIR}/ext/installs_lnx)
else()
  find_package(platform_folders REQUIRED HINTS ${CMAKE_SOURCE_DIR}/ext/installs)
endif ()
#if(${platform_folders_FOUND})
#  message(STATUS "fnd lib: ${platform_folders_DIR}")
#endif()

target_link_libraries(musicPlaycnt PRIVATE sago::platform_folders spdlog::spdlog protobuf::libprotobuf)

# find_path(plog_inc NAMES "plog/Log.h" HINTS ${CMAKE_SOURCE_DIR}/ext/installs/include)
#message(STATUS ${plog_inc})
#message(STATUS "HINTS ${CMAKE_SOURCE_DIR}/ext/installs/include")
# target_include_directories(musicPlaycnt PRIVATE ${plog_inc})
# target_compile_definitions(musicPlaycnt PRIVATE PLOG_CAPTURE_FILE)


#find_package(protobuf CONFIG REQUIRED)
#target_link_libraries(main PRIVATE protobuf::libprotoc protobuf::libprotobuf protobuf::libprotobuf-lite)

#find_package(protobuf CONFIG REQUIRED)
#target_link_libraries(main PRIVATE protobuf::libprotoc protobuf::libprotobuf protobuf::libprotobuf-lite)