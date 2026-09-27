#pragma once

#include "Cache.h"

#include <unordered_map>
#include <list>
#include <string>
#include <cstddef>

class LFUCache : public Cache {
private:
    struct Node {
        std::string key;
        std::string value;
        int frequency;
    };

    using NodeList = std::list<Node>;
    using NodeIterator = NodeList::iterator;

    int capacity;
    int minFrequency;
    std::size_t evictionCountValue = 0;

    std::unordered_map<std::string, NodeIterator> keyMap;
    std::unordered_map<int, NodeList> frequencyMap;

    void increaseFrequency(NodeIterator it);

public:
    explicit LFUCache(int capacity);
    ~LFUCache() override = default;

    bool get(const std::string& key, std::string& value) override;
    void put(const std::string& key, const std::string& value) override;
    bool remove(const std::string& key) override;
    int size() const override;
    void clear() override;

    std::size_t evictionCount() const override;
    std::string debugState() const override;
    void resetDiagnostics() override;
};