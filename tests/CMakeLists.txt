add_executable(tests
  gtest_main.cpp
  test_cases.cpp
)

target_link_libraries(tests PRIVATE
  spdlog::spdlog
  GTest::gtest
)
