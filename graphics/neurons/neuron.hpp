#pragma once

#include <memory>

#include "render_strategy.h"

struct Neuron {
    size_t idx = 0;
    float v = 0.0f;
    float threshold = 1.0f;
    float tau = 20.0f;
    float refractory = 0.0f;
    float spike_time = -1.0f;
    bool spiked = false;

    std::shared_ptr<NeuronStrategy> strategy;

    void simulate(float input, float dt) {
        v += dt * (-v + input) / 20.0f;
        spiked = (v > 1.0f);
        if (strategy) strategy->update(*this, dt);
    }

    void draw() const {
        if (strategy) strategy->draw(*this);
    }

    void update(float input, float dt, float t) {
        if (refractory > 0) {
            refractory -= dt;
            spiked = false;
            return;
        }
        v += dt * (-v + input) / tau;
        if (v >= threshold) {
            spiked = true;
            spike_time = t;
            v = 0.0f;
            refractory = 5.0f;
        } else {
            spiked = false;
        }
    }
};

struct RenderNeuronStrategy : NeuronStrategy {
    std::shared_ptr<VisualContext> ctx;

    RenderNeuronStrategy(std::shared_ptr<VisualContext> ctx) : ctx(ctx) {}

    void update(Neuron&, float) override {
        // Optional: nothing for now
    }

    void draw(const Neuron& n) const override {
        size_t idx = n.idx;
        const bx::Vec3& pos = ctx->positions[idx];

        bx::Vec3 c = n.spiked ? bx::Vec3{1, 1, 0} : bx::Vec3{n.v, 0, 1 - n.v};
        // draw_sphere(pos, 0.2f, c);
    }
};