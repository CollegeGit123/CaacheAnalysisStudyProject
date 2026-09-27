#include "../../include/cache/TinyLFUCache.h"

#include <algorithm>
#include <functional>
#include <sstream>
#include <climits>

TinyLFUCache::CountMinSketch::CountMinSketch(int width, int depth)
    : width(width),
      depth(depth),
      table(depth, std::vector<uint32_t>(width, 0)) {
}

uint64_t TinyLFUCache::CountMinSketch::hash(
    const std::string& key,
    int seed
) const {
    std::hash<std::string> hasher;

    uint64_t h = hasher(key);

    h ^= static_cast<uint64_t>(seed + 1)
         * 0x9e3779b97f4a7c15ULL;

    h ^= h >> 30;
    h *= 0xbf58476d1ce4e5b9ULL;
    h ^= h >> 27;

    return h;
}

void TinyLFUCache::CountMinSketch::increment(
    const std::string& key
) {
    for (int i = 0; i < depth; ++i) {
        int index =
            static_cast<int>(hash(key, i) % width);

        if (table[i][index] < UINT32_MAX)
            ++table[i][index];
    }
}

uint32_t TinyLFUCache::CountMinSketch::estimate(
    const std::string& key
) const {
    uint32_t result = UINT32_MAX;

    for (int i = 0; i < depth; ++i) {
        int index =
            static_cast<int>(hash(key, i) % width);

        result = std::min(
            result,
            table[i][index]
        );
    }

    return result;
}

void TinyLFUCache::CountMinSketch::age() {
    for (auto& row : table) {
        for (auto& counter : row) {
            counter /= 2;
        }
    }
}

void TinyLFUCache::CountMinSketch::reset() {
    for (auto& row : table) {
        std::fill(row.begin(), row.end(), 0);
    }
}

TinyLFUCache::TinyLFUCache(int capacity)
    : capacity(capacity),
      windowCapacity(std::max(1, capacity / 5)),
      mainCapacity(
          std::max(
              0,
              capacity - std::max(1, capacity / 5)
          )
      ),
      evictionCountValue(0),
      sketch(1000, 4) {
}

void TinyLFUCache::touchWindow(NodeIterator it) {
    windowList.splice(
        windowList.begin(),
        windowList,
        it
    );
}

void TinyLFUCache::touchMain(NodeIterator it) {
    mainList.splice(
        mainList.begin(),
        mainList,
        it
    );
}

void TinyLFUCache::promoteFromWindow() {
    if (windowList.empty())
        return;

    Node candidate = std::move(windowList.back());

    windowList.pop_back();
    windowMap.erase(candidate.key);

    // Main cache has available space.
    if (static_cast<int>(mainList.size()) < mainCapacity) {

        mainList.push_front(std::move(candidate));

        mainMap[mainList.front().key] =
            mainList.begin();

        return;
    }

    // No main cache available.
    if (mainCapacity <= 0)
        return;

    const std::string victimKey =
        mainList.back().key;

    uint32_t candidateFrequency =
        sketch.estimate(candidate.key);

    uint32_t victimFrequency =
        sketch.estimate(victimKey);

    // Candidate wins admission.
    if (candidateFrequency >= victimFrequency) {

        mainMap.erase(victimKey);
        mainList.pop_back();

        ++evictionCountValue;

        mainList.push_front(std::move(candidate));

        mainMap[mainList.front().key] =
            mainList.begin();
    }
}

bool TinyLFUCache::get(
    const std::string& key,
    std::string& value
) {
    sketch.increment(key);

    ++requestCount;
if (requestCount >= agingInterval) {
    sketch.age();
    requestCount = 0;
 }

    auto windowIt = windowMap.find(key);

    if (windowIt != windowMap.end()) {
        value = windowIt->second->value;

        touchWindow(windowIt->second);

        return true;
    }

    auto mainIt = mainMap.find(key);

    if (mainIt != mainMap.end()) {
        value = mainIt->second->value;

        touchMain(mainIt->second);

        return true;
    }

    return false;
}

void TinyLFUCache::put(
    const std::string& key,
    const std::string& value
) {
    if (capacity <= 0)
        return;

    sketch.increment(key);

    auto windowIt = windowMap.find(key);

    if (windowIt != windowMap.end()) {
        windowIt->second->value = value;

        touchWindow(windowIt->second);

        return;
    }

    auto mainIt = mainMap.find(key);

    if (mainIt != mainMap.end()) {
        mainIt->second->value = value;

        touchMain(mainIt->second);

        return;
    }

    windowList.push_front({key, value});

    windowMap[key] = windowList.begin();

    if (static_cast<int>(windowList.size()) >
        windowCapacity) {

        promoteFromWindow();
    }
}

bool TinyLFUCache::remove(const std::string& key) {
    auto windowIt = windowMap.find(key);

    if (windowIt != windowMap.end()) {
        windowList.erase(windowIt->second);
        windowMap.erase(windowIt);

        return true;
    }

    auto mainIt = mainMap.find(key);

    if (mainIt != mainMap.end()) {
        mainList.erase(mainIt->second);
        mainMap.erase(mainIt);

        return true;
    }

    return false;
}

int TinyLFUCache::size() const {
    return static_cast<int>(
        windowMap.size() + mainMap.size()
    );
}

void TinyLFUCache::clear() {
    windowList.clear();
    mainList.clear();
    windowMap.clear();
    mainMap.clear();
    sketch.reset();
    requestCount = 0;
    resetDiagnostics();
}
    
    

std::size_t TinyLFUCache::evictionCount() const {
    return evictionCountValue;
}

std::string TinyLFUCache::debugState() const {
    std::ostringstream state;

    state << "TinyLFU State:\n";

    state << "Window Capacity: "
          << windowCapacity << "\n";

    state << "Main Capacity: "
          << mainCapacity << "\n";

    state << "Window (MRU -> LRU): [";

    bool first = true;

    for (const Node& node : windowList) {
        if (!first)
            state << ", ";

        state << node.key;

        first = false;
    }

    state << "]\n";

    state << "Main (MRU -> LRU): [";

    first = true;

    for (const Node& node : mainList) {
        if (!first)
            state << ", ";

        state << node.key;

        first = false;
    }

    state << "]";

    return state.str();
}

void TinyLFUCache::resetDiagnostics() {
    evictionCountValue = 0;
}