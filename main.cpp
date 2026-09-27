#include <iostream>
#include <vector>
#include <string>

#include "include/cache/FIFOCache.h"
#include "include/cache/LRUCache.h"
#include "include/cache/LFUCache.h"
#include "include/cache/ARCCache.h"
#include "include/cache/TinyLFUCache.h"
#include <cctype>

#include "include/workload/WorkloadGenerator.h"
#include "include/benchmark/BenchmarkEngine.h"

int main() {
    std::cout << "========================================\n";
    std::cout << "              CacheLab\n";
    std::cout << "========================================\n\n";

    std::cout << "1. Custom Test\n";
    std::cout << "2. Automated Benchmark\n";
    std::cout << "Select mode: ";

    int mode;
    std::cin >> mode;

    if (mode == 1) {
        std::string policy;
        int capacity;
        int operations;

        std::cout << "\nAvailable policies:\n";
        std::cout << "FIFO\n";
        std::cout << "LRU\n";
        std::cout << "LFU\n";
        std::cout << "ARC\n";
        std::cout << "TinyLFU\n\n";

        std::cout << "Enter policy: ";
        std::cin >> policy;

    for (char& c : policy) {
     c = static_cast<char>(
        std::toupper(static_cast<unsigned char>(c))
       );
     }

        std::cout << "Enter cache capacity: ";
        std::cin >> capacity;

        std::cout << "Enter number of operations: ";
        std::cin >> operations;

        std::vector<std::string> requests;

        std::cout << "\nEnter " << operations << " keys:\n";

        for (int i = 0; i < operations; ++i) {
            std::string key;
            std::cin >> key;
            requests.push_back(key);
        }

        Trace customTrace;
        customTrace.name = "Custom Test";
        customTrace.requests = requests;

        Cache* cache = nullptr;

        if (policy == "FIFO") {
            cache = new FIFOCache(capacity);
        }
        else if (policy == "LRU") {
            cache = new LRUCache(capacity);
        }
        else if (policy == "LFU") {
            cache = new LFUCache(capacity);
        }
        else if (policy == "ARC") {
            cache = new ARCCache(capacity);
        }
        else if (policy == "TINYLFU") {
            cache = new TinyLFUCache(capacity);
        }
        else {
            std::cout << "\nInvalid policy.\n";
            return 1;
        }

        Statistics result = BenchmarkEngine::run(
            *cache,
            policy,
            customTrace,
            capacity
        );

        std::cout << "\n========================================\n";
        std::cout << "           CUSTOM TEST RESULT\n";
        std::cout << "========================================\n";

        std::cout << "Policy:          " << result.policy << "\n";
        std::cout << "Capacity:        " << result.capacity << "\n";
        std::cout << "Operations:      " << result.operations << "\n";
        std::cout << "Hits:            " << result.hits << "\n";
        std::cout << "Misses:          " << result.misses << "\n";
        std::cout << "Evictions:       " << result.evictions << "\n";
        std::cout << "Hit Rate:        " << result.hitRate << "%\n";
        std::cout << "Miss Rate:       " << result.missRate << "%\n";
        std::cout << "Execution Time:  " << result.executionTimeMs << " ms\n";

        std::cout << "========================================\n";

        delete cache;
        return 0;
    }

    if (mode == 2) {
        const std::size_t operations = 100000;
        const int keySpace = 10000;
        const std::vector<int> capacities = {10, 50, 100, 500, 1000};
        const std::uint32_t seed = 42;

        std::vector<Statistics> results;

        for (int capacity : capacities) {
            std::cout << "Capacity: " << capacity << "\n";

            auto uniform =
                WorkloadGenerator::generateUniform(
                    operations, keySpace, seed);

            auto zipfian =
                WorkloadGenerator::generateZipfian(
                    operations, keySpace, seed);

            auto cyclic =
                WorkloadGenerator::generateCyclic(
                    operations, capacity);

            auto phaseShift =
                WorkloadGenerator::generatePhaseShift(
                    operations, keySpace, seed);

            std::vector<Trace> workloads = {
                uniform,
                zipfian,
                cyclic,
                phaseShift
            };

            for (const auto& trace : workloads) {

                {
                    FIFOCache cache(capacity);
                    results.push_back(
                        BenchmarkEngine::run(
                            cache, "FIFO", trace, capacity));
                }

                {
                    LRUCache cache(capacity);
                    results.push_back(
                        BenchmarkEngine::run(
                            cache, "LRU", trace, capacity));
                }

                {
                    LFUCache cache(capacity);
                    results.push_back(
                        BenchmarkEngine::run(
                            cache, "LFU", trace, capacity));
                }

                {
                    ARCCache cache(capacity);
                    results.push_back(
                        BenchmarkEngine::run(
                            cache, "ARC", trace, capacity));
                }

                {
                    TinyLFUCache cache(capacity);
                    results.push_back(
                        BenchmarkEngine::run(
                            cache, "TinyLFU", trace, capacity));
                }
            }
        }

        BenchmarkEngine::exportCSV("results.csv", results);

        std::cout << "\nCACHE BENCHMARK COMPLETE\n";
        std::cout << "Total experiment results: "
                  << results.size() << "\n";
        std::cout << "Results exported to results.csv\n";

        return 0;
    }

    std::cout << "Invalid mode.\n";
    return 1;
}