#pragma once

#include <cstddef>
#include <string>

class CacheDiagnostics {
public:
    virtual ~CacheDiagnostics() = default;

    virtual std::size_t evictionCount() const = 0;

    virtual std::string debugState() const = 0;

    virtual void resetDiagnostics() = 0;
};