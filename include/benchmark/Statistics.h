#pragma once

#include <cstddef>
#include <string>

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