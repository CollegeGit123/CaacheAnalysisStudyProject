#include "../../include/benchmark/BenchmarkEngine.h"

#include <chrono>
#include <fstream>
#include <iomanip>

Statistics BenchmarkEngine::run(
    Cache& cache,
    const std::string& policyName,
    const Trace& trace,
    int capacity
) {
    Statistics stats;

    stats.policy = policyName;
    stats.workload = trace.name;
    stats.capacity = capacity;
    stats.operations = trace.requests.size();

    cache.clear();

    std::string value;

    auto start = std::chrono::high_resolution_clock::now();

    for (const std::string& key : trace.requests) {

        if (cache.get(key, value)) {
            ++stats.hits;
        }
        else {
            ++stats.misses;
            cache.put(key, key);
        }
    }

    auto end = std::chrono::high_resolution_clock::now();

    stats.executionTimeMs =
        std::chrono::duration<double, std::milli>(
            end - start
        ).count();

    stats.evictions = cache.evictionCount();

    if (stats.operations > 0) {

        stats.hitRate =
            static_cast<double>(stats.hits)
            / stats.operations
            * 100.0;

        stats.missRate =
            static_cast<double>(stats.misses)
            / stats.operations
            * 100.0;
    }

    return stats;
}

void BenchmarkEngine::exportCSV(
    const std::string& filename,
    const std::vector<Statistics>& results
) {
    std::ofstream file(filename);

    if (!file.is_open())
        return;

    file << "Policy,Workload,Capacity,Operations,"
         << "Hits,Misses,Evictions,HitRate,MissRate,"
         << "ExecutionTimeMs\n";

    file << std::fixed << std::setprecision(4);

    for (const Statistics& stats : results) {

        file << stats.policy << ","
             << stats.workload << ","
             << stats.capacity << ","
             << stats.operations << ","
             << stats.hits << ","
             << stats.misses << ","
             << stats.evictions << ","
             << stats.hitRate << ","
             << stats.missRate << ","
             << stats.executionTimeMs
             << "\n";
    }
}