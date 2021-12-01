#include <gtest/gtest.h>

int main(int argc, char **argv) 
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

TEST(SharedPtr, SharedPtrTest)
{
  const int a = 5;
  const int b = 5;
  EXPECT_EQ(a, b);

}
