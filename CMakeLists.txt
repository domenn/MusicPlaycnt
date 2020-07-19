cmake_minimum_required(VERSION 3.15)
project(musicPlaycnt)

set(CMAKE_CXX_STANDARD 17)
include_directories(${CMAKE_SOURCE_DIR})

file(GLOB_RECURSE _source_list *.cc* *.h* *.hpp*)
foreach(_source IN ITEMS ${_source_list})
  get_filename_component(_source_path "${_source}" PATH)
  string(REPLACE "${CMAKE_SOURCE_DIR}" "" _group_path "${_source_path}")
  string(REPLACE "/" "\\" _group_path "${_group_path}")
  source_group("${_group_path}" FILES "${_source}")
endforeach()

add_executable(musicPlaycnt
    src/main.cpp
#    src/misc/spdlog_colorful_sink_factory_singleton.cc
#    src/misc/spdlog_colorful_sink_factory_singleton.hpp
    src/misc/spd_logging.cc src/misc/spd_logging.hpp
    src/ostream_logging.cc src/ostream_logging.hpp src/model/song.cc src/model/song.hpp src/misc/protobuf_read_write_disk.cc src/misc/protobuf_read_write_disk.hpp
    src/win/encoding.cc src/win/encoding.hpp src/misc/custom_include_spdlog.hpp src/model/song_list.cc src/model/song_list.hpp src/model/serializable.cc src/model/serializable.hpp)

# find_package(fmt CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(Protobuf REQUIRED)


if (COMMAND protobuf_generate)
  message(STATUS gening ${CMAKE_CURRENT_SOURCE_DIR})
  protobuf_generate(LANGUAGE cpp TARGET musicPlaycnt PROTOC_OUT_DIR ${CMAKE_CURRENT_SOURCE_DIR} PROTOS
      ${CMAKE_CURRENT_SOURCE_DIR}/src/protobufs/app_config.proto ${CMAKE_CURRENT_SOURCE_DIR}/src/protobufs/songs.proto)
endif ()

if (MSVC)
  find_package(platform_folders REQUIRED HINTS ${CMAKE_SOURCE_DIR}/ext/installs_msvc)
else ()
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