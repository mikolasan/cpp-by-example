#pragma once

#include <iostream>
#include "AbstractTime.h"
#include "TimeFactory.h"

class MotherboardRTC : public AbstractTime {
public:
    MotherboardRTC() {
        TimeFactory::register_builder(std::bind(&MotherboardRTC::build, this));
    }
    AbstractTime* build() override {
        return this;
    }
    std::time_t get() const override {
        return 0;
    }
    void set(std::time_t time) override {}
    std::string name() const override { return "hardware"; };
};