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

    void step(uint8_t input[784]) {

        // for (size_t i = 0; i < 784; i++)
        // {
        //     if (input[i] > 0) {
        //         neurons[i].v += 1.5f * (input[i] / 255.0f) * 0.2f;  // inject current to neuron 0
        //     }
        // }
        
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
