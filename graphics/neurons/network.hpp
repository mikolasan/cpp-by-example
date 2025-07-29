#pragma once

#include <memory>
#include <vector>
#include <unordered_map>

#include "neuron.hpp"
#include "synapse.hpp"
#include "render_strategy.h"


struct Network {
    std::vector<Neuron> neurons;
    using Location = uint64_t;
    std::unordered_map<Location, Synapse> synapses;
    // std::vector<std::vector<Synapse>> synapses;
    float dt = 1.0f;
    float time = 0.0f;

    std::shared_ptr<RenderStrategy> render;

    explicit Network() = default;

    Network(int N) {
        setSize(N);
        // Init weights, if desired
    }

    void setSize(int N) {
        neurons.resize(N);
        // synapses.resize(N, std::vector<Synapse>(N));
    }

    std::vector<float> get_current_voltage_state() const {
        std::vector<float> state(neurons.size(), 0.0);
        for (size_t i = 0; i < neurons.size(); ++i) {
            state[i] = neurons[i].v;
        }
        return state;
    }

    void step(std::vector<uint8_t> inputs) {
        std::cout << "step (" << time << ")\n";
        
        for (size_t i = 0; i < inputs.size(); i++)
        {
            if (inputs[i] > 0) {
                neurons[i].v += 1.5f * (inputs[i] / 255.0f) * 0.2f;  // inject current to neuron 0
            }
        }
        
        std::vector<float> dv(neurons.size(), 0.0f);

        // Synaptic input
        for (auto& [loc, syn] : synapses) {
            uint32_t pre_idx = loc & 0xffff;
            uint32_t post_idx = (loc >> 8) & 0xffff;
            
            syn.update_pre(dt);
            syn.update_post(dt);

            // STDP
            if (neurons[pre_idx].spiked) {
                syn.on_pre_spike();
                dv[post_idx] += syn.weight;
            }
        }

        // Neuron updates
        for (size_t i = 0; i < neurons.size(); ++i)
            neurons[i].update(dv[i], dt, time);

        // STDP
        for (auto& [loc, syn] : synapses) {
            uint32_t pre_idx = loc & 0xffff;
            uint32_t post_idx = (loc >> 8) & 0xffff;
            syn.apply_stdp(neurons[pre_idx].spiked, neurons[post_idx].spiked);
            if (neurons[post_idx].spiked) syn.on_post_spike();
        }

        time += dt;
    }

    void init() {
        if (render) render->init();
    }

    void draw(float time) const {
        if (render) render->draw(time);
    }

    // void draw() const {
    //     for (auto& n : neurons) {
    //         n.draw();
    //     }
    // }

    void update(float dt) {
        if (render) render->update(dt);
    }

    void destroy() const {
        if (render) render->destroy();
    }
};
