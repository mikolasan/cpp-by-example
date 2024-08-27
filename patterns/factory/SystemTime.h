#pragma once

#include <cstring> // strerror
#include <iostream>
#ifdef __linux__ 
#include <sys/time.h> // settimeofday
#elif _WIN32
    // windows code goes here
#endif


#include "AbstractTime.h"
#include "TimeFactory.h"

class SystemTime : public AbstractTime {
public:
    SystemTime() {
        TimeFactory::register_builder(std::bind(&SystemTime::build, this));
    }
    AbstractTime* build() override {
        return this;
    }
    std::time_t get() const override {
        std::time_t result = time(nullptr);
        return result;
    }
    void set(std::time_t time) override {
#ifdef __linux__ 
        struct timeval now;
        now.tv_sec = time;
        now.tv_usec = 0;
        if(settimeofday(&now, NULL) == -1) {
            std::cerr << strerror(errno) <<std::endl;
        }
#endif
    }
    std::string name() const override { return "system"; };
};