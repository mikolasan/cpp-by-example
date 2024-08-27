#pragma once

#include <iostream>
#include <random>
#include "AbstractTime.h"
#include "TimeFactory.h"

class RandomTime : public AbstractTime {
public:
    RandomTime() {
        TimeFactory::register_builder(std::bind(&RandomTime::build, this));
        _uniform_dist = std::uniform_int_distribution<int>(-5, 5);
        _rand_delta = _uniform_dist(_rand_engine);
    }
    AbstractTime* build() override {
        return this;
    }
    std::time_t get() const override {
        std::time_t result = time(nullptr);
        return result + _rand_delta;
    }
    void set(std::time_t time) override {}
    std::string name() const override { return "random"; };

private:
    std::random_device _rand_dev;
    std::minstd_rand _rand_engine{_rand_dev()};
    std::uniform_int_distribution<int> _uniform_dist;
    time_t _rand_delta;
};