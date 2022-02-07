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
  // nullptr in constructor
  shared_ptr<int> nullPtr(nullptr);
  EXPECT_FALSE(nullPtr);
  EXPECT_EQ(nullPtr.use_count(), 0);

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

  //move constructor
  shared_ptr<int> originMove(new int(5));
  shared_ptr<int> movePtr(std::move(originMove));
  EXPECT_FALSE(originMove);
  EXPECT_EQ(originMove.use_count(), 0);
  EXPECT_TRUE(movePtr);
  EXPECT_EQ(*movePtr, 5);
  EXPECT_EQ(movePtr.use_count(), 1);

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

TEST(SharedPtr, Reset)
{
  //with no arg
  {
    DeleterMock deleteMock;
    shared_ptr<int> shrd_ptr(new int(5), [&deleteMock](auto* aPtr){ deleteMock.Deleter(aPtr); });
    EXPECT_CALL(deleteMock, Deleter(_)).Times(1);
    shrd_ptr.reset();
    EXPECT_EQ(shrd_ptr.use_count(), 0);
    EXPECT_FALSE(shrd_ptr);
  }

  //with one arg
  {
    DeleterMock deleteMock;
    shared_ptr<int> shrd_ptr(new int(5), [&deleteMock](auto* aPtr){ deleteMock.Deleter(aPtr); });
    EXPECT_CALL(deleteMock, Deleter(_)).Times(1);
    shrd_ptr.reset(new int(10));
    EXPECT_EQ(shrd_ptr.use_count(), 1);
    EXPECT_EQ(*shrd_ptr, 10);
  }

  //with deleter
  DeleterMock deleteMockMainCheck;
  EXPECT_CALL(deleteMockMainCheck, Deleter(_)).Times(1);
  {
    DeleterMock deleteMock;
    shared_ptr<int> shrd_ptr(new int(5), [&deleteMock](auto* aPtr){ deleteMock.Deleter(aPtr); });
    EXPECT_CALL(deleteMock, Deleter(_)).Times(1);
    shrd_ptr.reset(new int(10), [&deleteMockMainCheck](auto* aPtr){ deleteMockMainCheck.Deleter(aPtr); });
    EXPECT_EQ(shrd_ptr.use_count(), 1);
    EXPECT_EQ(*shrd_ptr, 10);
  }

  //shared_ptr count is > 1 with no arg
  {
    shared_ptr<int> origin(new int(5));
    shared_ptr<int> copy(origin);
    EXPECT_EQ(origin.use_count(), 2);
    EXPECT_EQ(copy.use_count(), 2);
    
    origin.reset();
        
    EXPECT_EQ(origin.use_count(), 0);
    EXPECT_EQ(copy.use_count(), 1);
    EXPECT_FALSE(origin);
    EXPECT_EQ(*copy, 5);
  }

  //shared_ptr count is > 1 with one arg
  {
    shared_ptr<int> origin(new int(5));
    shared_ptr<int> copy(origin);
    EXPECT_EQ(origin.use_count(), 2);
    EXPECT_EQ(copy.use_count(), 2);
    
    origin.reset(new int(10));
        
    EXPECT_EQ(origin.use_count(), 1);
    EXPECT_EQ(copy.use_count(), 1);
    EXPECT_EQ(*origin, 10);
    EXPECT_EQ(*copy, 5);
  }

  //shared_ptr count is > 1 with deleter
  DeleterMock deleteMockMainCheck2;
  EXPECT_CALL(deleteMockMainCheck2, Deleter(_)).Times(1);
  {
    DeleterMock deleteMock;
    shared_ptr<int> origin(new int(5), [&deleteMock](auto* aPtr){ deleteMock.Deleter(aPtr); });
    shared_ptr<int> copy(origin);
    EXPECT_EQ(origin.use_count(), 2);
    EXPECT_EQ(copy.use_count(), 2);
    
    EXPECT_CALL(deleteMock, Deleter(_)).Times(0);
    origin.reset(new int(10), [&deleteMockMainCheck2](auto* aPtr){ deleteMockMainCheck2.Deleter(aPtr); });
        
    EXPECT_EQ(origin.use_count(), 1);
    EXPECT_EQ(copy.use_count(), 1);
    EXPECT_EQ(*origin, 10);
    EXPECT_EQ(*copy, 5);
    EXPECT_CALL(deleteMock, Deleter(_)).Times(1);
  }

}

TEST(SharedPtr, Swap)
{
  shared_ptr<int> origin(new int(5));
  shared_ptr<int> copy(new int(10));

  origin.swap(copy);

  EXPECT_EQ(origin.use_count(), 1);
  EXPECT_EQ(copy.use_count(), 1);
  EXPECT_EQ(*origin, 10);
  EXPECT_EQ(*copy, 5);
}

TEST(SharedPtr, MoveAssignOperator)
{
  //count strong ref == 1
  {
    shared_ptr<int> originMove(new int(5));
    shared_ptr<int> movePtr(new int(10));
    movePtr = std::move(originMove);

    EXPECT_FALSE(originMove);
    EXPECT_EQ(originMove.use_count(), 0);
    EXPECT_TRUE(movePtr);
    EXPECT_EQ(*movePtr, 5);
    EXPECT_EQ(movePtr.use_count(), 1);
  }
  

  //count strong ref > 1
  {
    shared_ptr<int> originMove(new int(5));
    shared_ptr<int> movePtr(new int(10));
    shared_ptr<int> copy(movePtr);
    EXPECT_EQ(copy.use_count(), 2);
    EXPECT_EQ(movePtr.use_count(), 2);
    
    movePtr = std::move(originMove);

    EXPECT_FALSE(originMove);
    EXPECT_EQ(originMove.use_count(), 0);
    EXPECT_TRUE(movePtr);
    EXPECT_EQ(*movePtr, 5);
    EXPECT_EQ(movePtr.use_count(), 1);
    EXPECT_TRUE(copy);
    EXPECT_EQ(*copy, 10);
    EXPECT_EQ(copy.use_count(), 1);
  }
}

TEST(SharedPtr, Get)
{
  shared_ptr<int> ptr(new int(5));
  EXPECT_EQ(*ptr.get(), 5);
  ptr.reset();
  EXPECT_EQ(ptr.get(), nullptr);
}