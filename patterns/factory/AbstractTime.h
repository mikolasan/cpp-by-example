#pragma once

#include <ctime>
#include <string>

class AbstractTime {
public:
    virtual AbstractTime* build() = 0;
    virtual std::time_t get() const = 0;
    virtual void set(std::time_t time) = 0;
    virtual std::string name() const = 0;
};