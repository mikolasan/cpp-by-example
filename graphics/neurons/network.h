#ifndef NETWORK_H
#define NETWORK_H

#include <memory>
#include <vector>
#include <unordered_map>

#include "neuron.hpp"
#include "synapse.hpp"
#include "render_strategy.h"
#include "render/network_render.h"

using NeuronLayer = std::vector<std::shared_ptr<Neuron>>;

struct Network {
    std::vector<std::shared_ptr<Neuron>> neurons;
    std::vector<NeuronLayer> layers;

    using Location = uint64_t;
    std::unordered_map<Location, Synapse> synapses;
    // std::vector<std::vector<Synapse>> synapses;
    float dt = 1.0f;
    float time = 0.0f;

    std::shared_ptr<RenderStrategy> render;

    explicit Network() = default;

    void addLayer(const NeuronLayer&& layer);
    std::vector<float> get_current_voltage_state() const;
    void step(std::vector<uint8_t> inputs);
    void init();
    void draw(float time) const;
    void update(float dt);
    void destroy() const;
};

#endif