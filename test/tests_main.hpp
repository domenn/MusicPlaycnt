#pragma once

#include <src/win/windows_headers.hpp>


#include <gtest/gtest.h>
#include <src/misc/custom_include_spdlog.hpp>

TEST(One, Two) {
  ASSERT_NE(1, 2);
  ASSERT_EQ(42, 42);
}