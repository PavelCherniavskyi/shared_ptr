#include "src/shared_ptr.hpp"

int main(int argc, char ** argv)
{
  int* ptr = new int{5};
  shared_ptr<int> testPtr(ptr);

  delete ptr;
}
