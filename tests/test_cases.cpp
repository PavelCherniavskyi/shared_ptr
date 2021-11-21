#include <gtest/gtest.h>
#include <spdlog/spdlog.h>

TEST(SharedPtr, SharedPtrTest)
{
  const int a = 5;
  const int b = 5;
  EXPECT_EQ(a, b);

  SPDLOG_INFO("Hello from SPDLOG: {}-{}", a, b);
}