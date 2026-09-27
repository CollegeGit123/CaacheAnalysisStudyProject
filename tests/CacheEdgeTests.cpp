#include <iostream>
#include <string>

#include "../include/cache/FIFOCache.h"
#include "../include/cache/LRUCache.h"
#include "../include/cache/LFUCache.h"
#include "../include/cache/ARCCache.h"
#include "../include/cache/TinyLFUCache.h"

void testCapacityZero() {
    std::cout << "=== Capacity 0 Test ===\n";

    FIFOCache fifo(0);
    LRUCache lru(0);
    LFUCache lfu(0);
    ARCCache arc(0);
    TinyLFUCache tinylfu(0);

    std::string value;

    fifo.put("A", "A");
    lru.put("A", "A");
    lfu.put("A", "A");
    arc.put("A", "A");
    tinylfu.put("A", "A");

    std::cout << "FIFO:    " << fifo.size() << "\n";
    std::cout << "LRU:     " << lru.size() << "\n";
    std::cout << "LFU:     " << lfu.size() << "\n";
    std::cout << "ARC:     " << arc.size() << "\n";
    std::cout << "TinyLFU: " << tinylfu.size() << "\n";
}

void testCapacityOne() {
    std::cout << "\n=== Capacity 1 Test ===\n";

    FIFOCache fifo(1);
    LRUCache lru(1);
    LFUCache lfu(1);
    ARCCache arc(1);
    TinyLFUCache tinylfu(1);

    std::string value;

    fifo.put("A", "1");
    lru.put("A", "1");
    lfu.put("A", "1");
    arc.put("A", "1");
    tinylfu.put("A", "1");

    std::cout << "After inserting A:\n";
    std::cout << "FIFO:    " << fifo.size() << "\n";
    std::cout << "LRU:     " << lru.size() << "\n";
    std::cout << "LFU:     " << lfu.size() << "\n";
    std::cout << "ARC:     " << arc.size() << "\n";
    std::cout << "TinyLFU: " << tinylfu.size() << "\n";

    fifo.put("B", "2");
    lru.put("B", "2");
    lfu.put("B", "2");
    arc.put("B", "2");
    tinylfu.put("B", "2");

    std::cout << "\nAfter inserting B:\n";
    std::cout << "FIFO:    " << fifo.size() << "\n";
    std::cout << "LRU:     " << lru.size() << "\n";
    std::cout << "LFU:     " << lfu.size() << "\n";
    std::cout << "ARC:     " << arc.size() << "\n";
    std::cout << "TinyLFU: " << tinylfu.size() << "\n";
}

int main() {
    testCapacityZero();
    testCapacityOne();

    std::cout << "\nEdge test completed.\n";

    return 0;
}