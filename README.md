Here’s the project-ready README based on the implementation we finished:

# CacheLab — Comparative In-Memory Cache Simulator

## Study and Simulation of In-Memory Cache Replacement Algorithms

CacheLab is a C++17-based in-memory cache simulator designed to study and compare different cache replacement and admission strategies under controlled workloads.

The project implements five cache policies:

* FIFO — First-In, First-Out
* LRU — Least Recently Used
* LFU — Least Frequently Used
* ARC — Adaptive Replacement Cache
* TinyLFU — Tiny Least Frequently Used

The simulator evaluates their behavior using multiple workload patterns and cache capacities, measuring hit rate, miss rate, evictions, and execution time.

A Streamlit dashboard is provided for interactive visualization and custom cache simulations.

---

## Objectives

* Implement multiple cache replacement algorithms from scratch.
* Compare their behavior under different access patterns.
* Study the effect of cache capacity on hit rate.
* Evaluate cache performance using controlled and repeatable workloads.
* Analyze workloads with different locality characteristics.
* Provide an interactive interface for custom cache traces.
* Visualize benchmark results through an interactive dashboard.

---

## Algorithms

### FIFO

FIFO evicts the entry that has been present in the cache for the longest time.

**Data structure:** Queue + hash-based lookup

**Expected access complexity:** O(1)

---

### LRU

LRU evicts the entry that has not been accessed for the longest period.

**Data structure:** Hash Map + Doubly Linked List

**Expected access complexity:** O(1)

---

### LFU

LFU evicts the entry with the lowest access frequency.

The implementation maintains frequency-based lists and uses recency within a frequency bucket as a tie-breaker.

**Data structure:** Hash Map + Frequency Lists

**Expected access complexity:** O(1) expected

---

### ARC

ARC (Adaptive Replacement Cache) maintains separate structures for recently accessed and frequently accessed entries.

It uses:

* T1 — recent entries
* T2 — frequently accessed entries
* B1 — ghost entries corresponding to T1
* B2 — ghost entries corresponding to T2

ARC dynamically adjusts its target balance between recency and frequency based on workload behavior.

**Expected access complexity:** O(1) expected

---

### TinyLFU

TinyLFU uses frequency-based admission control to determine whether a new item should enter the cache.

A Count-Min Sketch is used to maintain approximate request-frequency information.

The frequency estimator:

* Updates on requests
* Tracks frequencies of requested keys
* Uses bounded memory
* Supports approximate frequency comparison

This allows TinyLFU to reject low-frequency candidates instead of automatically replacing an existing cache entry.

**Data structure:** Count-Min Sketch + Cache

**Expected access complexity:** O(1) expected for fixed sketch dimensions

---

## Workloads

Four workload generators are implemented.

### Uniform Random

Requests are distributed approximately uniformly across the configured key space.

This workload provides a baseline with relatively little locality.

### Zipfian

Requests follow a Zipfian distribution with a configurable skew parameter.

The implementation uses:

```text
alpha = 1.2
```

This creates a workload where some keys are requested substantially more frequently than others.

### Cyclic Scan

The workload repeatedly scans a set of keys whose size is:

```text
cache capacity + 1
```

This produces a scan pattern that continuously introduces one more key than the cache can hold and is useful for studying scan behavior.

### Dynamic Phase-Shift

The workload changes its access pattern during execution.

This is used to study how cache policies adapt when workload characteristics change over time.

---

## Experimental Setup

The automated benchmark uses:

| Parameter            |                  Value |
| -------------------- | ---------------------: |
| Cache policies       |                      5 |
| Workloads            |                      4 |
| Cache capacities     |                      5 |
| Capacities           | 10, 50, 100, 500, 1000 |
| Operations per trace |                100,000 |
| Key space            |                 10,000 |
| Random seed          |                     42 |
| Total experiments    |                    100 |

For each capacity and workload, the **same generated trace is supplied to all five cache policies**.

This keeps the workload identical across algorithms and makes the comparison consistent.

### Total experiment count

```text
4 workloads × 5 capacities × 5 policies = 100 experiments
```

The benchmark results are exported to:

```text
results.csv
```

---

## Metrics

The simulator records:

### Hit Rate

Percentage of requests successfully served from the cache.

```text
Hit Rate = Hits / Total Operations × 100
```

### Miss Rate

Percentage of requests not found in the cache.

```text
Miss Rate = Misses / Total Operations × 100
```

### Evictions

Number of existing cache entries removed to make room for another entry.

### Execution Time

Time required to process the complete workload trace for a cache policy.

---

## Project Structure

```text
DSA Project/
│
├── dashboard/
│   └── app.py
│
├── include/
│   ├── cache/
│   │   ├── Cache.h
│   │   ├── FIFO.h
│   │   ├── LRU.h
│   │   ├── LFU.h
│   │   ├── ARC.h
│   │   ├── TinyLFU.h
│   │   └── CountMinSketch.h
│   │
│   ├── workload/
│   │   ├── Trace.h
│   │   └── WorkloadGenerator.h
│   │
│   └── benchmark/
│       └── BenchmarkEngine.h
│
├── src/
│   ├── cache/
│   ├── workload/
│   └── benchmark/
│
├── main.cpp
├── results.csv
├── .gitignore
└── README.md
```

---

## Running the C++ Simulator

### Requirements

* C++ compiler with C++17 support
* GCC / MinGW recommended on Windows

### Compilation

From the project root:

```powershell
g++ -std=c++17 main.cpp src/cache/*.cpp src/workload/*.cpp src/benchmark/*.cpp -Iinclude -o cache_benchmark
```

### Run

```powershell
.\cache_benchmark.exe
```

The simulator provides two modes:

```text
1. Custom Test
2. Automated Benchmark
```

---

## Custom Test

Custom Test allows a user to manually provide:

* Cache policy
* Cache capacity
* Number of operations
* Request trace

Example:

```text
Enter policy: LRU
Enter cache capacity: 3
Enter number of operations: 5
Enter 5 keys:
A
A
B
C
D
```

The simulator reports:

* Hits
* Misses
* Hit rate
* Miss rate
* Evictions
* Execution time

This mode is useful for manually examining cache behavior on small traces.

---

## Automated Benchmark

Selecting Automated Benchmark executes the complete experiment matrix.

The benchmark:

1. Generates the workload trace.
2. Creates the selected cache policy.
3. Processes the same trace.
4. Records performance statistics.
5. Repeats the process for every policy and capacity.
6. Exports all results to `results.csv`.

Expected output:

```text
CACHE BENCHMARK COMPLETE
Total experiment results: 100
Results exported to results.csv
```

---

# Streamlit Dashboard

The project includes an interactive Streamlit dashboard for analyzing benchmark results and running custom simulations.

### Python Requirements

The dashboard uses:

* Python
* Streamlit
* Pandas
* Plotly

### Installation

Create and activate a virtual environment:

```powershell
python -m venv DSAvenv
.\DSAvenv\Scripts\Activate.ps1
```

Install the required packages:

```powershell
pip install streamlit plotly pandas
```

### Launch Dashboard

From the project root:

```powershell
streamlit run dashboard/app.py
```

The dashboard will open in the browser.

---

## Dashboard Features

### Project & Experiment Overview

Displays the main experimental configuration:

* Number of cache policies
* Number of workloads
* Number of capacities
* Operations per trace
* Total experiments

### Summary Metrics

Provides an overview of the selected benchmark results.

### Hit Rate by Policy

Compares average hit rates between cache replacement policies.

### Hit Rate vs Cache Capacity

Shows how cache capacity affects hit rate.

### Workload × Policy Heatmap

Provides a visual comparison of policy performance across workloads.

### Execution Time

Compares the execution time of the different policies.

### Eviction Count

Shows how frequently each policy removes cache entries.

### Detailed Benchmark Results

Displays the underlying benchmark records after applying the selected filters.

### Algorithm Overview

Provides an explanation of each implemented policy, its strategy, primary data structure, and expected complexity.

### Findings & Observations

Generates descriptive observations based on the currently selected benchmark filters.

---

# Custom Simulation

The dashboard also provides an interactive Custom Simulation interface.

Users can select:

* Cache policy
* Cache capacity
* Request trace

The trace can be entered as space-separated or comma-separated requests.

Example:

```text
A A A B C
```

The simulation displays:

* Hit count
* Miss count
* Hit rate
* Eviction count
* Step-by-step execution
* Cache state after each request
* Final cache state

This makes it possible to visually inspect how cache contents change after every request.

---

## Reproducibility

The automated benchmark uses a fixed random seed:

```text
42
```

The same workload trace is provided to every policy for a given workload and capacity.

This ensures that differences in measured performance are associated with the cache policies rather than different request sequences.

---

## Technologies Used

### Core Simulator

* C++
* C++17 STL
* `unordered_map`
* `list`
* `vector`
* `queue`
* `chrono`
* Random number generation

### Visualization

* Python
* Streamlit
* Pandas
* Plotly

---

## Scope

The project focuses on **in-memory cache replacement and admission strategies**.

The simulator does not implement:

* Distributed caching
* Networked cache servers
* Persistent cache storage
* Redis protocol compatibility
* Multithreaded cache access
* Distributed coherence
* OS-level page caching

The purpose is to provide a controlled environment for studying cache replacement behavior and comparing different algorithms.

---

## Conclusion

CacheLab provides an experimental environment for studying how different cache replacement and admission strategies behave under varying workloads and cache capacities.

By implementing FIFO, LRU, LFU, ARC, and TinyLFU and evaluating them using uniform, Zipfian, cyclic-scan, and dynamic phase-shift workloads, the project provides a basis for analyzing the relationship between workload characteristics, cache capacity, replacement strategy, and measured cache performance.
