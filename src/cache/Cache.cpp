#pragma once

#include <string>

class Cache {
public:
    virtual ~Cache() = default;

    virtual bool get(const std::string& key, std::string& value) = 0;

    virtual void put(const std::string& key, const std::string& value) = 0;

    virtual bool remove(const std::string& key) = 0;

    virtual int size() const = 0;

    virtual void clear() = 0;
};