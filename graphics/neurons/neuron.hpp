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

    std::shared_ptr<RenderStrategy> render;

    void init() {
        if (render) render->init();
    }

    void simulate(float input, float dt) {
        v += dt * (-v + input) / 20.0f;
        spiked = (v > 1.0f);
        if (render) render->update(dt);
    }

    void draw() const {
        if (render) render->draw();
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
