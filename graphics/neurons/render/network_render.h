#ifndef NETWORK_RENDER_H
#define NETWORK_RENDER_H

#include <memory>

#include <bx/math.h>
#include <bgfx/bgfx.h>

#include "network.h"
#include "render_strategy.h"

const int32_t max_area_size = 20;

struct NetworkVisualContext : VisualContext {
    // std::vector<bx::Vec3> positions;
    Network& net;

    explicit NetworkVisualContext(Network& net) : net(net) {}
};

struct NetworkRenderStrategy : RenderStrategy {

    std::shared_ptr<NetworkVisualContext> ctx;

    NetworkRenderStrategy(std::shared_ptr<NetworkVisualContext> ctx) : ctx(ctx) {}

    // void define_min_area_size() {
    //     const size_t n_neurons = ctx->net.neurons.size();
    //     area_size_x = bx::ceil(bx::sqrt(float(n_neurons / width)));
    //     area_size_y = area_size_x;
    //     area_size_z = 1;
    // }

    void init() override;
    void addLayer(const NeuronLayer& layer, const std::vector<size_t>& area_size);
    void update(float dt) override;
    void draw(float time) override;
    void destroy() override;

    void drawNeurons(float time);
    void drawSelection(float time);

    bgfx::VertexBufferHandle m_vbh;
    bgfx::IndexBufferHandle  m_ibh;

    bgfx::VertexBufferHandle m_selection_vbh;
    bgfx::IndexBufferHandle  m_selection_ibh;

    uint16_t instanceStride;
    uint32_t drawnCubes;
    uint32_t m_lastFrameMissing;

    int32_t area_size_x = 0;
    int32_t area_size_y = 0;
    int32_t area_size_z = 0;
    
    int32_t selected_x = 0;
    int32_t selected_y = 0;
    int32_t selected_z = 0;
    std::shared_ptr<Neuron> selected_neuron;

    float m_color[4];
    bgfx::UniformHandle u_color;
    bgfx::ProgramHandle m_selection_program;
    bgfx::ProgramHandle m_program;
};

#endif