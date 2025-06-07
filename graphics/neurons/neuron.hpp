#pragma once

struct Neuron {
    float v = 0.0f;
    float threshold = 1.0f;
    float tau = 20.0f;
    float refractory = 0.0f;
    float spike_time = -1.0f;
    bool spiked = false;

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