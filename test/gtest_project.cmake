

file(GLOB_RECURSE musicPlaycnt_tests_glob_src LIST_DIRECTORIES false "${CMAKE_CURRENT_SOURCE_DIR}/test/*.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
    )

add_executable(musicPlaycnt_tests  ${musicPlaycnt_tests_glob_src})

target_link_libraries(musicPlaycnt_tests PRIVATE GTest::gtest sago::platform_folders spdlog::spdlog protobuf::libprotobuf)