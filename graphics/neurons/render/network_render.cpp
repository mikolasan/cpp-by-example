#include <imgui.h>
#include "bgfx_utils.h"

#include "network_render.h"
#include "neuron_render.hpp"


void NetworkRenderStrategy::init(bgfx::ViewId id) {
    view_id = id;
    m_lastFrameMissing = 0;
    // 80 bytes stride = 64 bytes for 4x4 matrix + 16 bytes for RGBA color.
    instanceStride = 80;

    ground = std::make_unique<Ground>(view_id);

    NeuronRenderStrategy::init_once();

    // std::vector<PosColorVertex> vertices;
    // std::vector<uint16_t> indices;
    // NeuronRenderStrategy::generateSphereMesh(vertices, indices);

    // std::cout << "vertices" << std::endl;
    // for (const auto& v : vertices) {
    // std::cout << "(" << v.m_x << ", "
    //     << v.m_y << ", "
    //     << v.m_z << ") ";
    // }
    // std::cout << std::endl;

    // std::cout << "indices" << std::endl;
    // for (const auto& i : indices) {
    // std::cout << i << " ";
    // }
    // std::cout << std::endl;

    // // cube

    static PosColorVertex s_cubeVertices[8] =
    {
        {-1.0f,  1.0f,  1.0f, 0xff000000 },
        { 1.0f,  1.0f,  1.0f, 0xff0000ff },
        {-1.0f, -1.0f,  1.0f, 0xff00ff00 },
        { 1.0f, -1.0f,  1.0f, 0xff00ffff },
        {-1.0f,  1.0f, -1.0f, 0xffff0000 },
        { 1.0f,  1.0f, -1.0f, 0xffff00ff },
        {-1.0f, -1.0f, -1.0f, 0xffffff00 },
        { 1.0f, -1.0f, -1.0f, 0xffffffff },
    };

    static const uint16_t s_cubeTriList[] =
    {
        0, 1, 2, // 0
        1, 3, 2,
        4, 6, 5, // 2
        5, 6, 7,
        0, 2, 4, // 4
        4, 2, 6,
        1, 5, 3, // 6
        5, 7, 3,
        0, 4, 1, // 8
        4, 5, 1,
        2, 3, 6, // 10
        6, 3, 7,
    };

    // // Create static vertex buffer.
    m_selection_vbh = bgfx::createVertexBuffer(
    bgfx::makeRef(s_cubeVertices, sizeof(s_cubeVertices) )
    , PosColorVertex::ms_layout
    );

    // Create static index buffer.
    m_selection_ibh = bgfx::createIndexBuffer(
    bgfx::makeRef(s_cubeTriList, sizeof(s_cubeTriList) )
    );

    m_color[0] = 0.62f;
    m_color[1] = 0.083f;
    m_color[2] = 0.653f;
    m_color[3] = 1.0f;

    m_selection_program = loadProgram("vs_selection", "fs_selection");
    u_color = bgfx::createUniform("u_color", bgfx::UniformType::Vec4);

    m_program = loadProgram("vs_instancing", "fs_instancing");
}

void NetworkRenderStrategy::addLayer(const NeuronLayer& layer, const std::vector<size_t>& area_size) {
    area_size_x = area_size[0];
    area_size_y = area_size[1];
    area_size_z = area_size[2];

}

void NetworkRenderStrategy::update(float dt) {
    if (ImGui::IsKeyPressed(ImGuiKey_W))
        selected_y = std::max(0, selected_y - 1);
    if (ImGui::IsKeyPressed(ImGuiKey_S))
        selected_y = std::min(max_area_size - 1, selected_y + 1);
    
    if (ImGui::IsKeyPressed(ImGuiKey_A))
        selected_x = std::max(0, selected_x - 1);
    if (ImGui::IsKeyPressed(ImGuiKey_D))
        selected_x = std::min(max_area_size - 1, selected_x + 1);
    
    if (ImGui::IsKeyPressed(ImGuiKey_R))
        selected_z = std::max(0, selected_z - 1);
    if (ImGui::IsKeyPressed(ImGuiKey_F))
        selected_z = std::min(max_area_size - 1, selected_z + 1);
    
    if (ImGui::IsKeyPressed(ImGuiKey_Space)) {
        std::vector<int32_t> pos = { selected_x, selected_y, selected_z };
        if (selected_neuron == nullptr) {
            // create neuron
            ctx->net.addNeuron(pos, {});
        }

    }
        // to total number of instances to draw
    uint32_t totalCubes = ctx->net.neurons.size();

    if (totalCubes > 0) {
        // figure out how big of a buffer is available
        drawnCubes = bgfx::getAvailInstanceDataBuffer(totalCubes, instanceStride);

        // save how many we couldn't draw due to buffer room so we can display it
        m_lastFrameMissing = totalCubes - drawnCubes;
    } else {
        drawnCubes = 0;
        m_lastFrameMissing = 0;
    }
}

void NetworkRenderStrategy::drawNeurons(float time) {

    bgfx::InstanceDataBuffer idb;
    bgfx::allocInstanceDataBuffer(&idb, drawnCubes, instanceStride);

    uint8_t* data = idb.data;

    const float offset = 3.0f;
    const float start_x = - area_size_x * offset / 2.0f;
    const float start_y = - area_size_y * offset / 2.0f;
    const float start_z = - area_size_z * offset / 2.0f;

    // reset the selection
    selected_neuron = nullptr;

    for (uint32_t ii = 0; ii < drawnCubes; ++ii)
    {
        const auto& neuron = ctx->net.neurons[ii];

        std::shared_ptr<NeuronRenderStrategy>& s = std::dynamic_pointer_cast<NeuronRenderStrategy>(neuron->render);
        bx::Vec3 pos = s->get_position();
        uint32_t xx = pos.x;
        uint32_t yy = pos.y;
        uint32_t zz = pos.z;

        if (xx == selected_x && yy == selected_y && zz == selected_z) {
            selected_neuron = neuron;
        }

        float* mtx = (float*)data;
        time = 0.0f;
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

        mtx[12] = start_x + float(xx) * offset;
        mtx[13] = start_y + float(yy) * offset;
        mtx[14] = start_z + float(zz) * offset;

        float* color = (float*)&data[64];

        float r = neuron->v;
        color[0] = r;
        color[1] = 0.0f;
        color[2] = 1.0f - r;
        // color[0] = bx::sin(time + float(xx) / 11.0f) * 0.5f + 0.5f;
        // color[1] = bx::cos(time + float(yy) / 11.0f) * 0.5f + 0.5f;
        // color[2] = bx::sin(time * 3.0f) * 0.5f + 0.5f;
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
    bgfx::submit(view_id, m_program);
}

void NetworkRenderStrategy::drawSelection(float time) {

    const float offset = 3.0f;
    const float start_x = - area_size_x * offset / 2.0f;
    const float start_y = - area_size_y * offset / 2.0f;
    const float start_z = - area_size_z * offset / 2.0f;

    // Compute selection position
    float sel_x = start_x + selected_x * offset;
    float sel_y = start_y + selected_y * offset;
    float sel_z = start_z + selected_z * offset;

    // Create transform matrix
    float selMtx[16];
    bx::mtxIdentity(selMtx);
    bx::mtxTranslate(selMtx, sel_x, sel_y, sel_z);

    // Set transform, color, and render state
    bgfx::setTransform(selMtx);
    bgfx::setUniform(u_color, m_color);
    bgfx::setVertexBuffer(0, m_selection_vbh);
    bgfx::setIndexBuffer(m_selection_ibh);
    bgfx::setState(BGFX_STATE_DEFAULT | BGFX_STATE_PT_LINESTRIP);  // Use wireframe cube
    bgfx::submit(view_id, m_selection_program);
}

void NetworkRenderStrategy::draw(float time) {
    ground->draw();

    if (drawnCubes > 0) {
        drawNeurons(time);
    }

    drawSelection(time);
}

void NetworkRenderStrategy::destroy() {
    bgfx::destroy(u_color);
    bgfx::destroy(m_selection_program);
    bgfx::destroy(m_program);
}
