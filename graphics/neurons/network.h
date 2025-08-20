#ifndef NETWORK_H
#define NETWORK_H

#include <memory>
#include <vector>
#include <unordered_map>

#include "neuron.hpp"
#include "synapse.hpp"
#include "render_strategy.h"

using NeuronLayer = std::vector<std::shared_ptr<Neuron>>;
using Neocortex = std::vector<NeuronLayer>; // lol
using FlatNeuronStorage = std::vector<std::shared_ptr<Neuron>>;
using Location = uint64_t;
using SynapsesMap = std::unordered_map<Location, std::shared_ptr<Synapse>>;

struct Network {
    FlatNeuronStorage neurons;
    std::vector<NeuronLayer> layers;
    SynapsesMap synapses;
    // std::vector<std::vector<Synapse>> synapses;

    float dt = 1.0f;
    float time = 0.0f;

    std::shared_ptr<RenderStrategy> render;

    explicit Network();

    void addLayer(const NeuronLayer&& layer, const std::vector<size_t>& area_size);
    void addNeuron(const std::vector<int32_t>& pos, const NeuronLayer& connected_to);
    void addConnection(const std::shared_ptr<Neuron>& n1,
        const std::shared_ptr<Neuron>& n2);

    std::pair<uint32_t, uint32_t> locationToIds(uint64_t loc) const;
    uint64_t idsToLocation(uint32_t pre, uint32_t post) const;
    
    std::vector<float> get_current_voltage_state() const;
    void step(std::vector<uint8_t> inputs);
    void init(bgfx::ViewId id);
    void draw(float time);
    void update(float dt);
    void destroy() const;

    uint32_t create_id(std::shared_ptr<Neuron> neuron) const;
};

#endif