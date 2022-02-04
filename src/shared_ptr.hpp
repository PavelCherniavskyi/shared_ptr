#pragma once

#include <functional>

template<typename T>
class shared_ptr
{
public:
  shared_ptr() { init(nullptr); }

  explicit shared_ptr(T* aPtr) { init(aPtr); }

  explicit shared_ptr(T* aPtr, std::function<void(T*)> aDeleter) { init(aPtr, std::move(aDeleter)); }

  shared_ptr(const shared_ptr& aPtr) noexcept
    : mValue(aPtr.mValue)
    , mCtrlBlock(aPtr.mCtrlBlock)
  {
    if (mValue && aPtr.mCtrlBlock)
    {
      aPtr.mCtrlBlock->StrongRef++;
    }
  }

  shared_ptr& operator=(const shared_ptr<T>& aPtr) noexcept
  {
    if (mCtrlBlock == aPtr.mCtrlBlock && mValue == aPtr.mValue)
    {
      return *this;
    }

    customDelete();

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
    customDelete();
  }

  int use_count() { return mCtrlBlock ? mCtrlBlock->StrongRef : 0; }

private:
  struct control_block
  {
    explicit control_block(int aStrong, int aWeek, std::function<void(T*)>&& aDel)
      : StrongRef(aStrong)
      , WeekRef(aWeek)
      , Deleter(std::move(aDel))
    {
    }
    int StrongRef;
    int WeekRef;
    std::function<void(T*)> Deleter;
  };

  void customDelete()
  {
    if (mCtrlBlock)
    {
      if (mCtrlBlock->StrongRef == 1)
      {
        if(mValue)
        {
          mCtrlBlock->Deleter ? mCtrlBlock->Deleter(mValue) : delete mValue;
        }
        
        mValue = nullptr;

        if (mCtrlBlock->WeekRef == 0)
        {
          delete mCtrlBlock;
          mCtrlBlock = nullptr;
        }
      }
      else
      {
        mCtrlBlock->StrongRef--;
      }
    }
  }

  void init(T* aPtr, std::function<void(T*)>&& aDeleter = nullptr)
  {
    if (!aPtr)
    {
      mValue = nullptr;
      mCtrlBlock = nullptr;
      return;
    }

    mValue = aPtr;
    mCtrlBlock = new control_block(1, 0, std::move(aDeleter));
  }

private:
  T* mValue;
  control_block* mCtrlBlock;
};
