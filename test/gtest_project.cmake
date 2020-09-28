

file(GLOB_RECURSE musicPlaycnt_tests_glob_src LIST_DIRECTORIES false "${CMAKE_CURRENT_SOURCE_DIR}/test/*.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/test/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/test/*.h"
    )

add_executable(musicPlaycnt_tests ${musicPlaycnt_tests_glob_src})
target_compile_definitions(musicPlaycnt_tests PRIVATE SPDLOG_ACTIVE_LEVEL=0)

target_link_libraries(musicPlaycnt_tests PRIVATE GTest::gtest GTest::gmock codeMusicPlaycnt)