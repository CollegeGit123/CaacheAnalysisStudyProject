#include "../../include/cache/ARCCache.h"

#include <algorithm>
#include <sstream>

ARCCache::ARCCache(int capacity)
    : capacity(capacity),
      p(0),
      evictionCountValue(0) {
}

void ARCCache::replace(const std::string& key) {
    if (T1.empty() && T2.empty())
        return;

    if (!T1.empty() &&
        (static_cast<int>(T1.size()) > p ||
         (ghostB2.find(key) != ghostB2.end() &&
          static_cast<int>(T1.size()) == p))) {

        Node victim = std::move(T1.back());
        T1.pop_back();

        cache.erase(victim.key);

        ++evictionCountValue;

        B1.push_front(victim.key);
        ghostB1[victim.key] = B1.begin();

    } else if (!T2.empty()) {

        Node victim = std::move(T2.back());
        T2.pop_back();

        cache.erase(victim.key);

        ++evictionCountValue;

        B2.push_front(victim.key);
        ghostB2[victim.key] = B2.begin();
    }
}

void ARCCache::moveToT2(
    const std::string& key,
    const std::string& value
) {
    auto it = cache.find(key);

    if (it == cache.end())
        return;

    Entry& entry = it->second;

    if (entry.inT2) {
        entry.iterator->value = value;

        T2.splice(
            T2.begin(),
            T2,
            entry.iterator
        );

        entry.iterator = T2.begin();

        return;
    }

    Node node = std::move(*entry.iterator);

    T1.erase(entry.iterator);

    node.value = value;

    T2.push_front(std::move(node));

    entry.iterator = T2.begin();
    entry.inT2 = true;
}

bool ARCCache::get(
    const std::string& key,
    std::string& value
) {
    auto it = cache.find(key);

    if (it == cache.end())
        return false;

    value = it->second.iterator->value;

    moveToT2(key, value);

    return true;
}

void ARCCache::put(
    const std::string& key,
    const std::string& value
) {
    if (capacity <= 0)
        return;

    auto cacheIt = cache.find(key);

    // Key already exists in resident cache.
    if (cacheIt != cache.end()) {
        moveToT2(key, value);
        return;
    }

    // Key found in B1 ghost list.
    auto b1It = ghostB1.find(key);

    if (b1It != ghostB1.end()) {

        int delta = 1;

        if (!B1.empty() && B1.size() < B2.size()) {
            delta = static_cast<int>(B2.size() / B1.size());
            delta = std::max(1, delta);
        }

        p = std::min(capacity, p + delta);

        replace(key);

        B1.erase(b1It->second);
        ghostB1.erase(b1It);

        T2.push_front({key, value});

        cache[key] = {
            T2.begin(),
            true
        };

        return;
    }

    // Key found in B2 ghost list.
    auto b2It = ghostB2.find(key);

    if (b2It != ghostB2.end()) {

        int delta = 1;

        if (!B2.empty() && B2.size() < B1.size()) {
            delta = static_cast<int>(B1.size() / B2.size());
            delta = std::max(1, delta);
        }

        p = std::max(0, p - delta);

        replace(key);

        B2.erase(b2It->second);
        ghostB2.erase(b2It);

        T2.push_front({key, value});

        cache[key] = {
            T2.begin(),
            true
        };

        return;
    }

   // Completely new key.
int residentSize =
    static_cast<int>(T1.size() + T2.size());

if (residentSize >= capacity) {
    replace(key);
}
else if (residentSize + static_cast<int>(B1.size() + B2.size()) >= capacity) {
    if (residentSize + static_cast<int>(B1.size() + B2.size()) >= 2 * capacity &&
        !B2.empty()) {

        std::string oldKey = B2.back();
        ghostB2.erase(oldKey);
        B2.pop_back();
    }
}

T1.push_front({key, value});

cache[key] = {
    T1.begin(),
    false
};

// Keep ghost lists bounded.
while (static_cast<int>(B1.size()) > capacity) {
    std::string oldKey = B1.back();

    ghostB1.erase(oldKey);
    B1.pop_back();
}

while (static_cast<int>(B2.size()) > capacity) {
    std::string oldKey = B2.back();

    ghostB2.erase(oldKey);
    B2.pop_back();
}

    // Keep ghost lists bounded.
    while (static_cast<int>(B1.size()) > capacity) {
        std::string oldKey = B1.back();

        ghostB1.erase(oldKey);
        B1.pop_back();
    }

}

bool ARCCache::remove(const std::string& key) {
    auto it = cache.find(key);

    if (it == cache.end())
        return false;

    if (it->second.inT2)
        T2.erase(it->second.iterator);
    else
        T1.erase(it->second.iterator);

    cache.erase(it);

    return true;
}

int ARCCache::size() const {
    return static_cast<int>(cache.size());
}

void ARCCache::clear() {
    T1.clear();
    T2.clear();

    B1.clear();
    B2.clear();

    cache.clear();

    ghostB1.clear();
    ghostB2.clear();

    p = 0;

    resetDiagnostics();
}

std::size_t ARCCache::evictionCount() const {
    return evictionCountValue;
}

std::string ARCCache::debugState() const {
    std::ostringstream state;

    state << "ARC State:\n";

    state << "p = " << p << "\n";

    // T1
    state << "T1 (recent): [";

    bool first = true;

    for (const Node& node : T1) {
        if (!first)
            state << ", ";

        state << node.key;

        first = false;
    }

    state << "]\n";

    // T2
    state << "T2 (frequent): [";

    first = true;

    for (const Node& node : T2) {
        if (!first)
            state << ", ";

        state << node.key;

        first = false;
    }

    state << "]\n";

    // B1
    state << "B1 (T1 ghost): [";

    first = true;

    for (const std::string& key : B1) {
        if (!first)
            state << ", ";

        state << key;

        first = false;
    }

    state << "]\n";

    // B2
    state << "B2 (T2 ghost): [";

    first = true;

    for (const std::string& key : B2) {
        if (!first)
            state << ", ";

        state << key;

        first = false;
    }

    state << "]";

    return state.str();
}

void ARCCache::resetDiagnostics() {
    evictionCountValue = 0;
}