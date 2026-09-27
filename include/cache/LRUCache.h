#pragma once

#include "Cache.h"
#include <unordered_map>
#include <list>
#include <string>

class LRUCache : public Cache {
private:
    struct Node {
        std::string key;
        std::string value;
    };

    int capacity;
    std::size_t evictionCountValue = 0;

    std::list<Node> order;
    std::unordered_map<std::string, std::list<Node>::iterator> cache;

public:
    explicit LRUCache(int capacity);

    bool get(const std::string& key, std::string& value) override;
    void put(const std::string& key, const std::string& value) override;
    bool remove(const std::string& key) override;
    int size() const override;
    void clear() override;

    std::size_t evictionCount() const override;
    std::string debugState() const override;
    void resetDiagnostics() override;
};