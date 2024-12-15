add_executable(
  lifetime_examples_tests
  lifetime_examples_tests.cpp
)
target_link_libraries(
  lifetime_examples_tests
  GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(lifetime_examples_tests)