#include "../../include/cache/LFUCache.h"

#include <sstream>

LFUCache::LFUCache(int capacity)
    : capacity(capacity),
      minFrequency(0) {}

void LFUCache::increaseFrequency(NodeIterator it) {
    int oldFrequency = it->frequency;

    Node node = std::move(*it);

    frequencyMap[oldFrequency].erase(it);

    if (frequencyMap[oldFrequency].empty()) {
        frequencyMap.erase(oldFrequency);

        if (minFrequency == oldFrequency)
            ++minFrequency;
    }

    ++node.frequency;

    auto& newList = frequencyMap[node.frequency];

    newList.push_front(std::move(node));

    keyMap[newList.front().key] = newList.begin();
}

bool LFUCache::get(const std::string& key, std::string& value) {
    auto it = keyMap.find(key);

    if (it == keyMap.end())
        return false;

    NodeIterator nodeIt = it->second;

    value = nodeIt->value;

    increaseFrequency(nodeIt);

    return true;
}

void LFUCache::put(const std::string& key, const std::string& value) {
    if (capacity <= 0)
        return;

    auto it = keyMap.find(key);

    if (it != keyMap.end()) {
        it->second->value = value;
        increaseFrequency(it->second);
        return;
    }

    if (static_cast<int>(keyMap.size()) >= capacity) {
        auto& list = frequencyMap[minFrequency];

        Node& victim = list.back();

        keyMap.erase(victim.key);
        list.pop_back();

        ++evictionCountValue;

        if (list.empty())
            frequencyMap.erase(minFrequency);
    }

    auto& list = frequencyMap[1];

    list.push_front({key, value, 1});

    keyMap[key] = list.begin();

    minFrequency = 1;
}

bool LFUCache::remove(const std::string& key) {
    auto it = keyMap.find(key);

    if (it == keyMap.end())
        return false;

    NodeIterator nodeIt = it->second;
    int frequency = nodeIt->frequency;

    auto& list = frequencyMap[frequency];

    list.erase(nodeIt);
    keyMap.erase(it);

    if (list.empty()) {
        frequencyMap.erase(frequency);

        if (frequencyMap.empty()) {
            minFrequency = 0;
        }
        else if (minFrequency == frequency) {
            // Find the smallest remaining frequency.
            minFrequency = frequencyMap.begin()->first;

            for (const auto& entry : frequencyMap) {
                if (entry.first < minFrequency)
                    minFrequency = entry.first;
            }
        }
    }

    return true;
}

int LFUCache::size() const {
    return static_cast<int>(keyMap.size());
}

void LFUCache::clear() {
    keyMap.clear();
    frequencyMap.clear();

    minFrequency = 0;

    resetDiagnostics();
}

std::size_t LFUCache::evictionCount() const {
    return evictionCountValue;
}

std::string LFUCache::debugState() const {
    std::ostringstream state;

    state << "LFU State:\n";

    for (const auto& entry : frequencyMap) {
        state << "Frequency " << entry.first << ": [";

        bool first = true;

        for (const Node& node : entry.second) {
            if (!first)
                state << ", ";

            state << node.key;

            first = false;
        }

        state << "]";

        if (entry.first == minFrequency)
            state << " <- MIN";

        state << "\n";
    }

    return state.str();
}

void LFUCache::resetDiagnostics() {
    evictionCountValue = 0;
}