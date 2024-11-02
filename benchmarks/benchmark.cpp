/**
 * @file benchmark.cpp, part of the project MIOCSV under Apache License 2.0
 * @author jdlph (jdlph@hotmail.com)
 * @brief Benchmark the readers from project MIOCSV and std::getline() 
 *
 * @copyright Copyright (c) 2024 - Present Peiheng Li, Ph.D.
 *
 */

#include <benchmark/benchmark.h>

#include <stdcsv.h>
#include <miocsv.h>

void run_Reader()
{
    auto reader = miocsv::Reader {"data/csvreader.csv"};
    for (const auto& line: reader)
    {
        // do nothing
    }
}

void run_DictReader()
{
    auto reader = miocsv::DictReader {"data/csvreader.csv"};
    for (const auto& line: reader)
    {
        // do nothing
    }
}

void run_MIOReader()
{
    auto mioreader = miocsv::MIOReader {"data/csvreader.csv"};
    for (const auto& line: mioreader)
    {
        // do nothing
    }
}

void run_MIODictReader()
{
    auto mioreader = miocsv::MIODictReader {"data/csvreader.csv"};
    for (const auto& line: mioreader)
    {
        // do nothing
    }
}

void run_getline()
{
    std::ifstream ist {"data/csvreader.csv"};
    for (std::string s; std::getline(ist, s);)
    {
        // do nothing
    }
}

static void BM_run_Reader(benchmark::State& state)
{
    for (auto _ : state)
        run_Reader();
}

static void BM_run_DictReader(benchmark::State& state)
{
    for (auto _ : state)
        run_DictReader();
}

static void BM_run_MIOReader(benchmark::State& state)
{
    for (auto _ : state)
        run_MIOReader();
}

static void BM_run_MIODictReader(benchmark::State& state)
{
    for (auto _ : state)
        run_MIODictReader();
}

static void BM_run_getline(benchmark::State& state)
{
    for (auto _ : state)
        run_getline();
}

constexpr auto ITERATION_NUM = 20;

BENCHMARK(BM_run_Reader)->Iterations(ITERATION_NUM);
BENCHMARK(BM_run_MIOReader)->Iterations(ITERATION_NUM);
BENCHMARK(BM_run_DictReader)->Iterations(ITERATION_NUM);
BENCHMARK(BM_run_MIODictReader)->Iterations(ITERATION_NUM);
BENCHMARK(BM_run_getline)->Iterations(ITERATION_NUM);

BENCHMARK_MAIN();