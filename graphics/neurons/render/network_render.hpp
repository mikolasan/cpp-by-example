#pragma once

#include <memory>

#include <bx/math.h>
#include <bgfx/bgfx.h>

#include "render_strategy.h"


struct Network;

struct NetworkVisualContext : VisualContext {
    // std::vector<bx::Vec3> positions;
    Network& net;

    explicit NetworkVisualContext(Network& net) : net(net) {}
};

struct NetworkRenderStrategy : RenderStrategy {
    std::shared_ptr<NetworkVisualContext> ctx;

    NetworkRenderStrategy(std::shared_ptr<NetworkVisualContext> ctx) : ctx(ctx) {}

    void init() override {
        m_lastFrameMissing = 0;
        // 80 bytes stride = 64 bytes for 4x4 matrix + 16 bytes for RGBA color.
        instanceStride = 80;
        m_sideSize = 10;

        m_program = loadProgram("vs_instancing", "fs_instancing");

        // for (size_t i = 0; i < ctx->net.neurons.size(); i++)
        // {
        //     /* code */
        // }

        NeuronRenderStrategy::init_once();

    }

    void update(float dt) override {
        // to total number of instances to draw
        uint32_t totalCubes = m_sideSize * m_sideSize;

        // figure out how big of a buffer is available
        drawnCubes = bgfx::getAvailInstanceDataBuffer(totalCubes, instanceStride);

        // save how many we couldn't draw due to buffer room so we can display it
        m_lastFrameMissing = totalCubes - drawnCubes;
    }

    void draw(float time) const override {

        bgfx::InstanceDataBuffer idb;
        bgfx::allocInstanceDataBuffer(&idb, drawnCubes, instanceStride);

        uint8_t* data = idb.data;

        for (uint32_t ii = 0; ii < drawnCubes; ++ii)
        {
            uint32_t yy = ii / m_sideSize;
            uint32_t xx = ii % m_sideSize;

            float* mtx = (float*)data;
            bx::mtxRotateXY(mtx, time + xx * 0.21f, time + yy * 0.37f);

            // in column-major 4×4 transformation matrix the translation vector
            // | ... ... ... tx |
            // | ... ... ... ty |
            // | ... ... ... tz |
            // | ... ... ...  1 |
            // indices
            // |  0   4   8  12 |
            // |  1   5   9  13 |
            // |  2   6  10  14 |
            // |  3   7  11  15 |

            mtx[12] = -15.0f + float(xx) * 3.0f;
            mtx[13] = -15.0f + float(yy) * 3.0f;
            mtx[14] = 0.0f;

            float* color = (float*)&data[64];
            color[0] = bx::sin(time + float(xx) / 11.0f) * 0.5f + 0.5f;
            color[1] = bx::cos(time + float(yy) / 11.0f) * 0.5f + 0.5f;
            color[2] = bx::sin(time * 3.0f) * 0.5f + 0.5f;
            color[3] = 1.0f;

            data += instanceStride;
        }


        // Set vertex and index buffer.
        bgfx::setVertexBuffer(0, NeuronRenderStrategy::m_vbh);
        bgfx::setIndexBuffer(NeuronRenderStrategy::m_ibh);

        // Set instance data buffer.
        bgfx::setInstanceDataBuffer(&idb);

        // Set render states.
        bgfx::setState(BGFX_STATE_DEFAULT);

        // Submit primitive for rendering to view 0.
        bgfx::submit(0, m_program);

    }

    void destroy() override {
        bgfx::destroy(m_program);
    }

    uint16_t instanceStride;
    uint32_t drawnCubes;
    uint32_t m_lastFrameMissing;
    uint32_t m_sideSize;

    bgfx::ProgramHandle m_program;
};