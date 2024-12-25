add_executable(
  memory_benchmarks
  memory_benchmarks.cpp
)
target_link_libraries(
  memory_benchmarks
  benchmark::benchmark_main
)