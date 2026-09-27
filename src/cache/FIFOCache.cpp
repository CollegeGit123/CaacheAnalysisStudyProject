#include "../../include/cache/FIFOCache.h"

#include <sstream>

FIFOCache::FIFOCache(int capacity)
    : capacity(capacity) {}

bool FIFOCache::get(const std::string& key, std::string& value) {
    auto it = cache.find(key);

    if (it == cache.end())
        return false;

    value = it->second;
    return true;
}

void FIFOCache::put(const std::string& key, const std::string& value) {
    if (capacity <= 0)
        return;

    auto it = cache.find(key);

    if (it != cache.end()) {
        it->second = value;
        return;
    }

    if (static_cast<int>(cache.size()) >= capacity) {
        const std::string& oldestKey = order.front();

        cache.erase(oldestKey);
        order.pop_front();

        ++evictionCountValue;
    }

    cache[key] = value;
    order.push_back(key);
}

bool FIFOCache::remove(const std::string& key) {
    auto it = cache.find(key);

    if (it == cache.end())
        return false;

    cache.erase(it);
    order.remove(key);

    return true;
}

int FIFOCache::size() const {
    return static_cast<int>(cache.size());
}

void FIFOCache::clear() {
    cache.clear();
    order.clear();
    resetDiagnostics();
}

std::size_t FIFOCache::evictionCount() const {
    return evictionCountValue;
}

std::string FIFOCache::debugState() const {
    std::ostringstream state;

    state << "FIFO Queue: [";

    bool first = true;

    for (const std::string& key : order) {
        if (!first)
            state << ", ";

        state << key;
        first = false;
    }

    state << "]";

    return state.str();
}

void FIFOCache::resetDiagnostics() {
    evictionCountValue = 0;
}