#pragma once
#include <vector>
#include "neuron.hpp"
#include "synapse.hpp"

struct SNN {
    std::vector<Neuron> neurons;
    std::vector<std::vector<Synapse>> synapses;
    float dt = 1.0f;
    float time = 0.0f;

    SNN(int N) {
        neurons.resize(N);
        synapses.resize(N, std::vector<Synapse>(N));
        // Init weights, if desired
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
};