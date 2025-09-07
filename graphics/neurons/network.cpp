#include <fcntl.h>
#include <fstream>
#include <io.h>
#include <iostream>

#include <capnp/message.h>
#include <capnp/serialize.h>

#include "neuron.capnp.h"
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

namespace neuf {

std::shared_ptr<Neuron> makeNeuron(const capgen::Neuron::Reader& reader) {
    auto n = std::make_shared<Neuron>();
    n->idx        = reader.getIdx();
    n->v          = reader.getV();
    n->threshold  = reader.getThreshold();
    n->spiked     = reader.getSpiked();

    auto ctx = std::make_shared<NeuronVisualContext>(n);
    ctx->position = {
      float(reader.getPosX()),
      float(reader.getPosY()),
      float(reader.getPosZ()),
    };
    n->render = std::make_shared<NeuronRenderStrategy>(ctx);

    return n;
}

std::shared_ptr<Synapse> makeSynapse(const capgen::Synapse::Reader& reader) {
    auto s = std::make_shared<Synapse>();
    s->weight = reader.getWeight();
    s->trace_pre  = reader.getTracePre();
    s->trace_post  = reader.getTracePost();
    return s;
}

std::unique_ptr<Network> hydrateNetwork(const capgen::Network::Reader& reader) {
    std::unique_ptr<Network> net = std::make_unique<Network>();

    // --- Neurons ---
    auto capnpNeurons = reader.getNeurons();
    net->neurons.reserve(capnpNeurons.size());
    for (auto nReader : capnpNeurons) {
        auto n = makeNeuron(nReader);
        net->neurons.push_back(std::move(n));
    }

    // --- Layers ---
    auto capnpLayers = reader.getLayers();
    net->layers.reserve(capnpLayers.size());
    for (auto lReader : capnpLayers) {
        NeuronLayer layer;
        auto list = lReader.getNeuronIndices();
        layer.reserve(list.size());
        for (auto idx : list) {
            auto n = net->neurons[idx];
            layer.push_back(n);
        }
        net->layers.push_back(layer);
    }

    // --- Synapses ---
    auto capnpSynapses = reader.getSynapses();
    for (auto sReader : capnpSynapses) {
        auto key   = sReader.getKey();
        auto value = makeSynapse(sReader.getValue());
        net->synapses.emplace(key, std::move(value));
    }

    return net;
}

void dehydrateNetwork(Network* net, capgen::Network::Builder builder) {
    // --- Neurons ---
    auto neuronsBuilder = builder.initNeurons(net->neurons.size());
    for (size_t i = 0; i < net->neurons.size(); i++) {
        auto& n = *net->neurons[i];
        auto nBuilder = neuronsBuilder[i];
        nBuilder.setIdx(n.idx);
        nBuilder.setV(n.v);
        nBuilder.setThreshold(n.threshold);
        nBuilder.setSpiked(n.spiked);
        auto r = std::dynamic_pointer_cast<NeuronRenderStrategy>(n.render);
        nBuilder.setPosX(r->ctx->position.x);
        nBuilder.setPosY(r->ctx->position.y);
        nBuilder.setPosZ(r->ctx->position.z);
    }

    // --- Layers ---
    auto layersBuilder = builder.initLayers(net->layers.size());
    for (size_t i = 0; i < net->layers.size(); i++) {
        auto& layer = net->layers[i];
        auto lBuilder = layersBuilder[i];
        auto indicesBuilder = lBuilder.initNeuronIndices(layer.size());
        for (size_t j = 0; j < layer.size(); j++) {
            auto n = layer[j];
            indicesBuilder.set(j, n->idx);
        }
    }

    // --- Synapses ---
    auto synapsesBuilder = builder.initSynapses(net->synapses.size());
    size_t idx = 0;
    for (auto& [key, synPtr] : net->synapses) {
        auto sBuilder = synapsesBuilder[idx++];
        sBuilder.setKey(key);
        auto val = sBuilder.initValue();
        val.setWeight(synPtr->weight);
        val.setTracePre(synPtr->trace_pre);
        val.setTracePost(synPtr->trace_post);
    }
}

void serialize(Network* net, const std::string& filename) {
    ::capnp::MallocMessageBuilder message;
    auto netBuilder = message.initRoot<capgen::Network>();
    dehydrateNetwork(net, netBuilder);

    int fd = _open(filename.c_str(), _O_WRONLY | _O_CREAT | _O_TRUNC | _O_BINARY, _S_IWRITE);
    if (fd == -1) {
        std::cerr << "file open failed: " << strerror(errno) << "\n";
        return;
    }

    ::capnp::writeMessageToFd(fd, message);
    _close(fd);
}

std::unique_ptr<Network> deserialize(const std::string& filename) {
    int fd = _open(filename.c_str(), _O_RDONLY | _O_BINARY);
    struct _stat buf;
    int result = _stat(filename.c_str(), &buf);
    if (result != 0) return nullptr;
    if (buf.st_size == 0) return nullptr;

    capnp::StreamFdMessageReader reader(fd);

    auto netReader = reader.getRoot<capgen::Network>();
    auto net = hydrateNetwork(netReader);

    _close(fd);
    return net;
}

} // namespace neuf