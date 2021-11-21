#include <spdlog/spdlog.h>
#include "src/shared_ptr.hpp"

int main(int argc, char ** argv)
{
  spdlog::set_pattern("[%^-%L-%$][%S.%e][%s:%# %!] %v");

  int* ptr = new int{5};
  shared_ptr<int> testPtr(ptr);

  SPDLOG_INFO("Hello from SPDLOG: {}", *ptr);
  
  delete ptr;
}