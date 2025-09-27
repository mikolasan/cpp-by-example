
#include <algorithm>
#include <cmath>

#include "synapse.h"

void Synapse::update_pre(float dt) { trace_pre *= std::exp(-dt / tau_pre); }
void Synapse::update_post(float dt) { trace_post *= std::exp(-dt / tau_post); }

void Synapse::on_pre_spike() { trace_pre += 1.0f; }
void Synapse::on_post_spike() { trace_post += 1.0f; }

void Synapse::apply_stdp(bool pre_spike, bool post_spike) {
    if (pre_spike) weight += A_plus * trace_post;
    if (post_spike) weight += A_minus * trace_pre;
    weight = std::clamp(weight, 0.0f, 1.0f);
}
