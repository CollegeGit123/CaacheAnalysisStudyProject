#pragma once

#include "Trace.h"

#include <cstddef>
#include <cstdint>

class WorkloadGenerator {
public:

    // Uniform random workload.
    static Trace generateUniform(
        std::size_t operations,
        int keySpace,
        std::uint32_t seed
    );

    // 80/20 Zipfian workload.
    static Trace generateZipfian(
        std::size_t operations,
        int keySpace,
        std::uint32_t seed
    );

    // Sequential/cyclic scan.
    // Loop size = cache capacity + 1.
    static Trace generateCyclic(
        std::size_t operations,
        int capacity
    );

    // Dynamic phase-shift workload.
    static Trace generatePhaseShift(
        std::size_t operations,
        int keySpace,
        std::uint32_t seed
    );
};