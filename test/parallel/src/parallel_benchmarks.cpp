#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <catch2/catch.hpp>
#include <ien/parallel.hpp>

#include <algorithm>
#include <limits>
#include <cmath>
#include <vector>

static const size_t VEC_SIZE = 10000;

double expensive_op(const double val)
{
    double res = std::asin(val);
    res = std::sqrt(std::pow(val, 10));
    double aux = std::log(val);
    
    for(int i = 0; i < 100; ++i)
    {
        res = std::pow(res, aux);
        res = std::sqrt(res);
        res = std::sqrt(res);
    }
    return res;
}

TEST_CASE("parallel_for benchmarks")
{
    BENCHMARK_ADVANCED("Linear for")(Catch::Benchmark::Chronometer meter)
    {
        std::vector<double> vec;
        vec.resize(VEC_SIZE);
        std::transform(vec.begin(), vec.end(), vec.begin(), [](double i){ return std::rand(); });
        meter.measure([&] (int _i)
        { 
            for(size_t i = 0; i < vec.size(); ++i)
            {
                vec[i] = expensive_op(vec[i]);
            }
        });
        return vec;
    };

    BENCHMARK_ADVANCED("std::transform")(Catch::Benchmark::Chronometer meter)
    {
        std::vector<double> vec;
        vec.resize(VEC_SIZE);
        std::transform(vec.begin(), vec.end(), vec.begin(), [](double i){ return std::rand(); });
        meter.measure([&] (int _i)
        { 
            std::transform(vec.begin(), vec.end(), vec.begin(), [](double i)
            {
                return expensive_op(i);
            });
        });
        return vec;
    };

    BENCHMARK_ADVANCED("ien::parallel_for")(Catch::Benchmark::Chronometer meter)
    {
        std::vector<double> vec;
        vec.resize(VEC_SIZE);
        std::transform(vec.begin(), vec.end(), vec.begin(), [](double i){ return std::rand(); });
        meter.measure([&] (int _i)
        { 
            ien::parallel_for_params pfdesc;
            pfdesc.count = static_cast<long>(vec.size());

            ien::parallel_for(pfdesc, [&](long i)
            {
                vec[i] = expensive_op(i);
            });
        });
        return vec;
    };
}