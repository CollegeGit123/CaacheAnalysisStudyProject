#include "../../include/workload/WorkloadGenerator.h"

#include <random>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>
#include <iterator>

Trace WorkloadGenerator::generateUniform(
    std::size_t operations,
    int keySpace,
    std::uint32_t seed
) {
    Trace trace;
    trace.name = "Uniform Random";

    if (keySpace <= 0) {
        return trace;
    }

    std::mt19937 generator(seed);
    std::uniform_int_distribution<int> distribution(
        0,
        keySpace - 1
    );

    trace.requests.reserve(operations);

    for (std::size_t i = 0; i < operations; ++i) {
        trace.requests.push_back(
            "K" + std::to_string(distribution(generator))
        );
    }

    return trace;
}

Trace WorkloadGenerator::generateZipfian(
    std::size_t operations,
    int keySpace,
    std::uint32_t seed
) {
    Trace trace;
    trace.name = "Zipfian";

    if (keySpace <= 0) {
        return trace;
    }

    std::mt19937 generator(seed);

    // Zipfian exponent.
    // Higher values create stronger concentration on popular keys.
    const double alpha = 1.2;

    // Build the cumulative probability distribution.
    std::vector<double> cumulative;
    cumulative.reserve(keySpace);

    double normalization = 0.0;

    for (int i = 1; i <= keySpace; ++i) {
        normalization += 1.0 / std::pow(static_cast<double>(i), alpha);
    }

    double cumulativeProbability = 0.0;

    for (int i = 1; i <= keySpace; ++i) {
        cumulativeProbability +=
            (1.0 / std::pow(static_cast<double>(i), alpha))
            / normalization;

        cumulative.push_back(cumulativeProbability);
    }

    std::uniform_real_distribution<double> distribution(0.0, 1.0);

    trace.requests.reserve(operations);

    for (std::size_t i = 0; i < operations; ++i) {
        double randomValue = distribution(generator);

        auto it = std::lower_bound(
            cumulative.begin(),
            cumulative.end(),
            randomValue
        );

        int key = static_cast<int>(
            std::distance(cumulative.begin(), it)
        );

        trace.requests.push_back(
            "K" + std::to_string(key)
        );
    }

    return trace;
}
Trace WorkloadGenerator::generateCyclic(
    std::size_t operations,
    int capacity
) {
    Trace trace;
    trace.name = "Cyclic Scan";

    if (capacity < 0) {
        return trace;
    }

    // Required loop size = capacity + 1.
    int loopSize = capacity + 1;

    trace.requests.reserve(operations);

    for (std::size_t i = 0; i < operations; ++i) {
        int key = static_cast<int>(i % loopSize);

        trace.requests.push_back(
            "K" + std::to_string(key)
        );
    }

    return trace;
}

Trace WorkloadGenerator::generatePhaseShift(
    std::size_t operations,
    int keySpace,
    std::uint32_t seed
) {
    Trace trace;
    trace.name = "Dynamic Phase-Shift";

    if (keySpace <= 0) {
        return trace;
    }

    std::mt19937 generator(seed);

    trace.requests.reserve(operations);

    // Divide the trace into four phases.
    std::size_t phaseSize = operations / 4;

    for (std::size_t i = 0; i < operations; ++i) {

        std::size_t phase = 0;

        if (phaseSize > 0) {
            phase = i / phaseSize;
        }

        if (phase > 3) {
            phase = 3;
        }

        int start;
        int end;

        // Each phase favors a different region
        // of the key space.
        switch (phase) {

        case 0:
            start = 0;
            end = keySpace / 4;
            break;

        case 1:
            start = keySpace / 4;
            end = keySpace / 2;
            break;

        case 2:
            start = keySpace / 2;
            end = (3 * keySpace) / 4;
            break;

        default:
            start = (3 * keySpace) / 4;
            end = keySpace;
            break;
        }

        if (end <= start) {
            end = start + 1;
        }

        std::uniform_int_distribution<int> distribution(
            start,
            end - 1
        );

        trace.requests.push_back(
            "K" + std::to_string(distribution(generator))
        );
    }

    return trace;
}