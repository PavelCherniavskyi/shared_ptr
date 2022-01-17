#pragma once

#include <functional>

template<typename T>
class shared_ptr
{
public:
  shared_ptr()
    : mValue(nullptr)
    , mCtrlBlock(nullptr)
  {
  }

  explicit shared_ptr(T* aPtr)
    : mCtrlBlock(new control_block(1, 0))
  {
    mValue = aPtr;
  }

  explicit shared_ptr(T* aPtr, std::function<void(T*)> aDeleter)
    : mCtrlBlock(new control_block(1, 0, aDeleter))
  {
    mValue = aPtr;
  }

  shared_ptr(const shared_ptr& aPtr) noexcept
  {
    if (aPtr.mCtrlBlock)
    {
      aPtr.mCtrlBlock->StrongRef++;
    }

    mValue = aPtr.mValue;
    mCtrlBlock = aPtr.mCtrlBlock;
  }

  shared_ptr& operator=(const shared_ptr<T>& aPtr) noexcept
  {
    if (mCtrlBlock == aPtr.mCtrlBlock && mValue == aPtr.mValue)
    {
      return *this;
    }

    // assuming if mValue != nullptr means that mCtrlBlock can't be nullptr as well
    if (mValue)
    {
      if (mCtrlBlock->StrongRef <= 1)
      {
        mCtrlBlock->Deleter ? mCtrlBlock->Deleter(mValue) : delete mValue;
      }
      else
      {
        mCtrlBlock->StrongRef--;
      }
    }

    aPtr.mCtrlBlock->StrongRef++;
    mValue = aPtr.mValue;
    mCtrlBlock = aPtr.mCtrlBlock;
    return *this;
  }

  T& operator*() const noexcept { return *mValue; }
  T* operator->() const noexcept { return mValue; }
  explicit operator bool() const noexcept { return mValue ? true : false; }

  ~shared_ptr()
  {
    if (mValue)
    {
      if (mCtrlBlock->StrongRef <= 1)
      {
        mCtrlBlock->Deleter ? mCtrlBlock->Deleter(mValue) : delete mValue;
        delete mCtrlBlock;
        mCtrlBlock = nullptr;
        mValue = nullptr;
      }
      else
      {
        mCtrlBlock->StrongRef--;
      }
    }
  }

  int use_count() { return mCtrlBlock ? mCtrlBlock->StrongRef : 0; }

private:
  struct control_block
  {
    explicit control_block(int aStrong, int aWeek)
      : control_block(aStrong, aWeek, nullptr)
    {
    }
    explicit control_block(int aStrong, int aWeek, std::function<void(T*)> aDel)
      : StrongRef(aStrong)
      , WeekRef(aWeek)
      , Deleter(aDel)
    {
    }
    int StrongRef;
    int WeekRef;
    std::function<void(T*)> Deleter;
  };

private:
  T* mValue;
  control_block* mCtrlBlock;
};
