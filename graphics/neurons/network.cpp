#include <iostream>

#include "network.h"
#include "render/neuron_render.hpp"
#include "render/network_render.h"

Network::Network() {
    layers.push_back({});
}

void Network::addLayer(const NeuronLayer&& layer, const std::vector<size_t>& area_size) {
    layers.push_back(layer);

    uint32_t last_id = neurons.back()->idx;
    for (auto it = layer.begin(); it != layer.end(); ++it)
    {
        (*it)->idx = ++last_id;
        neurons.emplace_back(std::move(*it));
    }

    // neurons.assign(layer.begin(), layer.end());
    if (render && std::dynamic_pointer_cast<NetworkRenderStrategy>(render) != nullptr) {
        std::dynamic_pointer_cast<NetworkRenderStrategy>(render)->addLayer(layer, area_size);
    }
}

void Network::addNeuron(const std::vector<int32_t>& pos, const NeuronLayer& connected_to) {
    uint32_t layer = pos[1]; // y
    if (layer >= layers.size()) {
        layers.insert(layers.end(), layers.size() - layer + 1, {});
        // std::cerr << "wrong layer position\n";
        return;
    }

    std::shared_ptr<Neuron> neuron = std::make_shared<Neuron>();
    neuron->idx = create_id(nullptr);
    auto ctx = std::make_shared<NeuronVisualContext>(neuron);
    ctx->position = { float(pos[0]), float(pos[1]), float(pos[2]) };
    neuron->render = std::make_shared<NeuronRenderStrategy>(ctx);

    layers[layer].push_back(neuron);
    neurons.push_back(neuron);
}

void Network::addConnection(const std::shared_ptr<Neuron>& n1, const std::shared_ptr<Neuron>& n2) {
    synapses.emplace(idsToLocation(n1->idx, n2->idx), std::make_shared<Synapse>());
}

std::pair<uint32_t, uint32_t> Network::locationToIds(uint64_t loc) const {
    uint32_t pre_idx = loc & 0xffff;
    uint32_t post_idx = (loc >> 8) & 0xffff;
    return {pre_idx, post_idx};
}

uint64_t Network::idsToLocation(uint32_t pre, uint32_t post) const {
    uint64_t loc = 0;
    loc = pre | (post << 8);
    return loc;
}

std::vector<float> Network::get_current_voltage_state() const {
    std::vector<float> state(neurons.size(), 0.0);
    for (size_t i = 0; i < neurons.size(); ++i) {
        state[i] = neurons[i]->v;
    }
    return state;
}

void Network::step(std::vector<uint8_t> inputs) {
    std::cout << "step (" << time << ")\n";
    
    for (size_t i = 0; i < inputs.size(); i++)
    {
        if (inputs[i] > 0) {
            neurons[i]->v += 1.5f * (inputs[i] / 255.0f) * 0.2f;  // inject current to neuron 0
        }
    }
    
    std::vector<float> dv(neurons.size(), 0.0f);

    // Synaptic input
    for (auto& [loc, syn] : synapses) {
        auto [pre_idx, post_idx] = locationToIds(loc);
        
        syn->update_pre(dt);
        syn->update_post(dt);

        // STDP
        if (neurons[pre_idx]->spiked) {
            syn->on_pre_spike();
            dv[post_idx] += syn->weight;
        }
    }

    // Neuron updates
    for (size_t i = 0; i < neurons.size(); ++i)
        neurons[i]->update(dv[i], dt, time);

    // STDP
    for (auto& [loc, syn] : synapses) {
        auto [pre_idx, post_idx] = locationToIds(loc);
        syn->apply_stdp(neurons[pre_idx]->spiked, neurons[post_idx]->spiked);
        if (neurons[post_idx]->spiked) syn->on_post_spike();
    }

    time += dt;
}

void Network::init(bgfx::ViewId id) {
    if (render) render->init(id);
}

void Network::draw(float time) {
    if (render) render->draw(time);
}

// void draw() const {
//     for (auto& n : neurons) {
//         n.draw();
//     }
// }

void Network::update(float dt) {
    if (render) render->update(dt);
}

void Network::destroy() const {
    if (render) render->destroy();
}

uint32_t Network::create_id(std::shared_ptr<Neuron> neuron) const {
    return neurons.size();
}