#include <benchmark/benchmark.h>

static void Benchmark(benchmark::State& state)
{
    uint64_t a = 0;
    for (auto _ : state)
    {
        a++;
    }
}
BENCHMARK(Benchmark)->Unit(benchmark::kMillisecond);

BENCHMARK_MAIN();