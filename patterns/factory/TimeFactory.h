#pragma once

#include <functional>
#include <iostream>
#include <vector>

#include "AbstractTime.h"

using BuildFunction = std::function<AbstractTime*()>;

class TimeFactory {
public:
    static TimeFactory* instance() {
        if (!factory) {
            factory = new TimeFactory();
        }
        return factory;
    }
    static void register_builder(const BuildFunction& builder) {
        builders.push_back(builder);
    }
    static std::vector<BuildFunction>::iterator iterator_begin() {
        return builders.begin();
    }
    static std::vector<BuildFunction>::iterator iterator_end() {
        return builders.end();
    }
private:
    static TimeFactory* factory;
    static std::vector<BuildFunction> builders;
};