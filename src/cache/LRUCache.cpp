#include "../../include/cache/LRUCache.h"

#include <sstream>

LRUCache::LRUCache(int capacity)
    : capacity(capacity) {}

bool LRUCache::get(const std::string& key, std::string& value) {
    auto it = cache.find(key);

    if (it == cache.end())
        return false;

    order.splice(order.begin(), order, it->second);
    value = it->second->value;

    return true;
}

void LRUCache::put(const std::string& key, const std::string& value) {
    if (capacity <= 0)
        return;

    auto it = cache.find(key);

    if (it != cache.end()) {
        it->second->value = value;
        order.splice(order.begin(), order, it->second);
        return;
    }

    if (static_cast<int>(cache.size()) >= capacity) {
        auto last = std::prev(order.end());

        cache.erase(last->key);
        order.pop_back();

        ++evictionCountValue;
    }

    order.push_front({key, value});
    cache[key] = order.begin();
}

bool LRUCache::remove(const std::string& key) {
    auto it = cache.find(key);

    if (it == cache.end())
        return false;

    order.erase(it->second);
    cache.erase(it);

    return true;
}

int LRUCache::size() const {
    return static_cast<int>(cache.size());
}

void LRUCache::clear() {
    cache.clear();
    order.clear();
    resetDiagnostics();
}

std::size_t LRUCache::evictionCount() const {
    return evictionCountValue;
}

std::string LRUCache::debugState() const {
    std::ostringstream state;

    state << "LRU Order (MRU -> LRU): [";

    bool first = true;

    for (const Node& node : order) {
        if (!first)
            state << ", ";

        state << node.key;
        first = false;
    }

    state << "]";

    return state.str();
}

void LRUCache::resetDiagnostics() {
    evictionCountValue = 0;
}