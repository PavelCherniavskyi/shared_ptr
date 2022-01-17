#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "../src/shared_ptr.hpp"

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

class DeleterMock
{ 
public:
  MOCK_METHOD(void, Deleter, (int* val));
};

using ::testing::_;

TEST(SharedPtr, SharedPtrConstructorsTest)
{
  // empty constructor
  shared_ptr<int> emptyPtr;
  EXPECT_FALSE(emptyPtr);

  // pointer constructor
  shared_ptr<int> oneParamPtr(new int(5));
  EXPECT_TRUE(oneParamPtr);
  EXPECT_EQ(*oneParamPtr, 5);
  EXPECT_EQ(oneParamPtr.use_count(), 1);
  
  //constructor with deleter
  {
    DeleterMock deleteMock;
    shared_ptr<int> withDeleter(new int(5), [&deleteMock](auto* aPtr){ deleteMock.Deleter(aPtr); });
    EXPECT_TRUE(withDeleter);
    EXPECT_EQ(*withDeleter, 5);
    EXPECT_EQ(withDeleter.use_count(), 1);
    EXPECT_CALL(deleteMock, Deleter(_)).Times(1);
  }
  
  //copy constructor
  shared_ptr<int> origin(new int(7));
  shared_ptr<int> copy(origin);
  EXPECT_EQ(*origin, 7);
  EXPECT_EQ(*copy, 7);
  EXPECT_EQ(origin.use_count(), 2);
  EXPECT_EQ(copy.use_count(), 2);

}

TEST(SharedPtr, AssignOperator)
{
  shared_ptr<int> origin(new int(5));
  shared_ptr<int> copy(origin);
  copy = origin;
  EXPECT_EQ(copy.use_count(), 2);
  EXPECT_EQ(origin.use_count(), 2);
  
  shared_ptr<int> clone;
  clone = copy;
  EXPECT_EQ(copy.use_count(), 3);
  EXPECT_EQ(origin.use_count(), 3);
  EXPECT_EQ(clone.use_count(), 3);

  DeleterMock deleteMock;
  shared_ptr<int> cloneWithValue(new int(5), [&deleteMock](auto* aPtr){ deleteMock.Deleter(aPtr); });
  EXPECT_CALL(deleteMock, Deleter(_)).Times(1);
  cloneWithValue = copy;

  EXPECT_EQ(copy.use_count(), 4);
  EXPECT_EQ(origin.use_count(), 4);
  EXPECT_EQ(clone.use_count(), 4);
  EXPECT_EQ(cloneWithValue.use_count(), 4);

  shared_ptr<int> anotherOrigin(new int(10));
  origin = anotherOrigin;
  EXPECT_EQ(origin.use_count(), 2);
  EXPECT_EQ(anotherOrigin.use_count(), 2);
  EXPECT_EQ(copy.use_count(), 3);
  EXPECT_EQ(clone.use_count(), 3);
  EXPECT_EQ(cloneWithValue.use_count(), 3);
}

struct DummyStruct
{
   DummyStruct(int val) : A(val) {}
   int A;
};

TEST(SharedPtr, DereferenceOperators)
{
  shared_ptr<DummyStruct> ptr(new DummyStruct(5));
  EXPECT_EQ(ptr->A, 5);
  EXPECT_EQ((*ptr).A, 5);
}

TEST(SharedPtr, BoolOperator)
{
  shared_ptr<int> hasValue(new int(5));
  shared_ptr<int> empty;
  EXPECT_TRUE(hasValue);
  EXPECT_FALSE(empty);
}

TEST(SharedPtr, UseCount)
{
  shared_ptr<int> origin(new int(5));
  EXPECT_EQ(origin.use_count(), 1);

  {
    shared_ptr<int> copy(origin);
    EXPECT_EQ(copy.use_count(), 2);
  }

  EXPECT_EQ(origin.use_count(), 1);
}