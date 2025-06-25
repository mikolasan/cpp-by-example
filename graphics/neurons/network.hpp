#pragma once

#include <memory>
#include <vector>

#include "neuron.hpp"
#include "synapse.hpp"
#include "render_strategy.h"


struct Network {
    std::vector<Neuron> neurons;
    std::vector<std::vector<Synapse>> synapses;
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
        synapses.resize(N, std::vector<Synapse>(N));
    }

    void step() {
        std::vector<float> inputs(neurons.size(), 0.0f);

        // Synaptic input
        for (size_t i = 0; i < neurons.size(); ++i) {
            for (size_t j = 0; j < neurons.size(); ++j) {
                auto& syn = synapses[i][j];
                syn.update_pre(dt);
                syn.update_post(dt);

                if (neurons[j].spiked) {
                    syn.on_pre_spike();
                    inputs[i] += syn.weight;
                }
            }
        }

        // Neuron updates
        for (size_t i = 0; i < neurons.size(); ++i)
            neurons[i].update(inputs[i], dt, time);

        // STDP
        for (size_t i = 0; i < neurons.size(); ++i) {
            for (size_t j = 0; j < neurons.size(); ++j) {
                synapses[i][j].apply_stdp(neurons[j].spiked, neurons[i].spiked);
                if (neurons[i].spiked) synapses[i][j].on_post_spike();
            }
        }

        time += dt;
    }

    void init() {
        if (render) render->init();
    }

    void draw() const {
        if (render) render->draw();
    }

    // void draw() const {
    //     for (auto& n : neurons) {
    //         n.draw();
    //     }
    // }

    void update(float input, float dt, float t) {
        if (render) render->update(dt);
    }
};
