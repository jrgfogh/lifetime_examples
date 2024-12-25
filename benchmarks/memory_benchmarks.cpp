#include <benchmark/benchmark.h>

#include <barrier>
#include <memory>
#include <thread>
#include <vector>

constexpr size_t GB = 1 << 30;
constexpr size_t elems = GB / (sizeof std::vector<int> + 2 * sizeof(int));

static void BM_Allocate(benchmark::State& state) {
    for (auto _ : state) {
        // We want to time only the allocation, not the deallocation.
        std::vector<std::vector<int>> v{elems / state.threads(), {5, 7}};
        state.PauseTiming();
    }
}

BENCHMARK(BM_Allocate)->Repetitions(10)->ThreadRange(1, 8)->UseRealTime()->MeasureProcessCPUTime();

static void BM_FreeSameThread(benchmark::State& state) {
    for (auto _ : state) {
        state.PauseTiming();
        std::vector<std::vector<int>> v{elems / state.threads(), {5, 7}};
        state.ResumeTiming();
        // We want to time the deallocation, which happens implicitly here.
    }
}

BENCHMARK(BM_FreeSameThread)->Repetitions(10)->ThreadRange(1, 8)->UseRealTime()->MeasureProcessCPUTime();

struct shared_state {
    explicit shared_state(size_t size) :
        vs{size},
        barrier{static_cast<ptrdiff_t>(size)} {}

    std::vector<std::vector<std::vector<int>>> vs;
    std::barrier<> barrier;
};

static void BM_FreeOtherThread(benchmark::State& state) {
    static std::unique_ptr<shared_state> shared;
    if (state.thread_index() == 0) {
        shared = std::make_unique<shared_state>(state.threads());
    }
    auto index = state.thread_index();
    for (auto _ : state) {
        auto &vs = shared->vs;
        auto &barrier = shared->barrier;
        state.PauseTiming();
        vs[index] = {elems / state.threads(), {5, 7}};

        barrier.arrive_and_wait();
        state.ResumeTiming();

        index = (index + 1) % state.threads();
        vs[index].resize(0);
        vs[index].shrink_to_fit();
    }
}

BENCHMARK(BM_FreeOtherThread)->Repetitions(10)->ThreadRange(1, 16)->UseRealTime()->MeasureProcessCPUTime();

// TODO: Missing benchmarks:
// 1. Shuffled data.