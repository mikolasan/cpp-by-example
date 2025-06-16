#pragma once

#include <algorithm>
#include <memory>
#include <vector>

#include <bx/math.h>


struct Network;

struct VisualContext {
    const Network& net;
    std::vector<bx::Vec3> positions;  // same index as neuron
};

struct Neuron;

struct NeuronStrategy {
    virtual void update(Neuron&, float dt) = 0;
    virtual void draw(const Neuron&) const = 0;
    virtual ~NeuronStrategy() = default;
};

