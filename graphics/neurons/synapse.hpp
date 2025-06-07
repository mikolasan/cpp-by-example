#pragma once
#include <algorithm>
#include <cmath>

struct Synapse {
    float weight = 0.5f;
    float tau_pre = 20.0f;
    float tau_post = 20.0f;
    float A_plus = 0.01f;
    float A_minus = -0.012f;
    float trace_pre = 0.0f;
    float trace_post = 0.0f;

    void update_pre(float dt) { trace_pre *= std::exp(-dt / tau_pre); }
    void update_post(float dt) { trace_post *= std::exp(-dt / tau_post); }

    void on_pre_spike() { trace_pre += 1.0f; }
    void on_post_spike() { trace_post += 1.0f; }

    void apply_stdp(bool pre_spike, bool post_spike) {
        if (pre_spike) weight += A_plus * trace_post;
        if (post_spike) weight += A_minus * trace_pre;
        weight = std::clamp(weight, 0.0f, 1.0f);
    }
};