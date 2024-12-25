#include <benchmark/benchmark.h>

#include <vector>
#include <thread>

constexpr size_t GB = 1 << 30;
constexpr size_t elems = GB / (sizeof std::vector<int> + 2 * sizeof(int));

static void BM_AllocateOneThread(benchmark::State& state) {
    for (auto _ : state) {
        // We want to time only the allocation, not the deallocation.
        std::vector<std::vector<int>> v{elems, {5, 7}};
        state.PauseTiming();
    }
}

// BENCHMARK(BM_AllocateOneThread)->Repetitions(10)->UseRealTime();

static void BM_FreeOneThread(benchmark::State& state) {
    for (auto _ : state) {
        state.PauseTiming();
        std::vector<std::vector<int>> v{elems, {5, 7}};
        state.ResumeTiming();
        // We want to time the deallocation, which happens implicitly here.
    }
}

// BENCHMARK(BM_FreeOneThread)->Repetitions(10)->UseRealTime();

static void BM_AllocateManyThreads(benchmark::State& state) {
    for (auto _ : state) {
        // We want to time only the allocation, not the deallocation.
        std::vector<std::vector<int>> v{elems / state.threads(), {5, 7}};
        state.PauseTiming();
    }
}

BENCHMARK(BM_AllocateManyThreads)->Repetitions(10)->ThreadRange(1, 8)->UseRealTime();

static void BM_FreeManyThreads(benchmark::State& state) {
    for (auto _ : state) {
        state.PauseTiming();
        std::vector<std::vector<int>> v{elems / state.threads(), {5, 7}};
        state.ResumeTiming();
        // We want to time the deallocation, which happens implicitly here.
    }
}

BENCHMARK(BM_FreeManyThreads)->Repetitions(10)->ThreadRange(1, 8)->UseRealTime();

// TODO: Missing benchmarks:
// 1. Shuffled data.
// 2. Several threads:
//   i) No cooperation +
//   ii) A allocates, B frees