add_executable(tests
  test_cases.cpp
)

target_link_libraries(tests PRIVATE
  GTest::gtest
  GTest::gmock
)
