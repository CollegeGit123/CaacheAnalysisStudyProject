#pragma once

#include "Cache.h"

#include <unordered_map>
#include <list>
#include <string>
#include <vector>
#include <cstdint>
#include <cstddef>

class TinyLFUCache : public Cache {
private:
    class CountMinSketch {
    private:
        int width;
        int depth;
        std::vector<std::vector<uint32_t>> table;

        uint64_t hash(const std::string& key, int seed) const;

    public:
        CountMinSketch(int width = 1000, int depth = 4);

        void increment(const std::string& key);
        uint32_t estimate(const std::string& key) const;
        void age();
        void reset();
    };

    struct Node {
        std::string key;
        std::string value;
    };

    using NodeList = std::list<Node>;
    using NodeIterator = NodeList::iterator;

    int capacity;
    int windowCapacity;
    int mainCapacity;

    std::size_t evictionCountValue = 0;

    NodeList windowList;
    NodeList mainList;

    std::unordered_map<std::string, NodeIterator> windowMap;
    std::unordered_map<std::string, NodeIterator> mainMap;

    CountMinSketch sketch;
    std::size_t requestCount = 0;
 static constexpr std::size_t agingInterval = 10000;

    void promoteFromWindow();
    void touchWindow(NodeIterator it);
    void touchMain(NodeIterator it);

public:
    explicit TinyLFUCache(int capacity);
    ~TinyLFUCache() override = default;

    bool get(const std::string& key, std::string& value) override;
    void put(const std::string& key, const std::string& value) override;
    bool remove(const std::string& key) override;
    int size() const override;
    void clear() override;

    std::size_t evictionCount() const override;
    std::string debugState() const override;
    void resetDiagnostics() override;
};