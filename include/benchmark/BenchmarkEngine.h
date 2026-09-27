#pragma once

#include "../workload/Trace.h"
#include "../cache/Cache.h"

#include <cstddef>
#include <string>
#include <vector>

struct Statistics {
    std::string policy;
    std::string workload;
    int capacity = 0;
    std::size_t operations = 0;
    std::size_t hits = 0;
    std::size_t misses = 0;
    std::size_t evictions = 0;
    double hitRate = 0.0;
    double missRate = 0.0;
    double executionTimeMs = 0.0;
};

class BenchmarkEngine {
public:
    static Statistics run(
        Cache& cache,
        const std::string& policyName,
        const Trace& trace,
        int capacity
    );

    static void exportCSV(
        const std::string& filename,
        const std::vector<Statistics>& results
    );
};