#pragma once

#include "Cache.h"
#include <unordered_map>
#include <list>
#include <string>

class ARCCache : public Cache {
private:
    struct Node {
        std::string key;
        std::string value;
    };

    using NodeList = std::list<Node>;
    using NodeIterator = NodeList::iterator;

    struct Entry {
        NodeIterator iterator;
        bool inT2;
    };

    int capacity;
    int p;
    std::size_t evictionCountValue = 0;

    NodeList T1;
    NodeList T2;

    std::list<std::string> B1;
    std::list<std::string> B2;

    std::unordered_map<std::string, Entry> cache;
    std::unordered_map<std::string, std::list<std::string>::iterator> ghostB1;
    std::unordered_map<std::string, std::list<std::string>::iterator> ghostB2;

    void replace(const std::string& key);
    void moveToT2(const std::string& key, const std::string& value);

public:
    explicit ARCCache(int capacity);
    ~ARCCache() override = default;

    bool get(const std::string& key, std::string& value) override;
    void put(const std::string& key, const std::string& value) override;
    bool remove(const std::string& key) override;
    int size() const override;
    void clear() override;

    std::size_t evictionCount() const override;
    std::string debugState() const override;
    void resetDiagnostics() override;
};