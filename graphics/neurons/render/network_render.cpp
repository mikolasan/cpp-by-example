#include <imgui.h>
#include "bgfx_utils.h"

#include "network_render.h"
#include "neuron_render.hpp"


// 80 bytes stride = 64 bytes for 4x4 matrix + 16 bytes for RGBA color.
constexpr uint16_t neuronInstanceStride = 80;
constexpr uint16_t synapseInstanceStride = 2 * sizeof(bx::Quaternion);

void NetworkRenderStrategy::init(bgfx::ViewId id) {
    view_id = id;
    m_lastFrameMissing = 0;
    ground = std::make_unique<Ground>(view_id);

    NeuronRenderStrategy::init_once();

    NeuronRenderStrategy::buildUnitTubeGrid(6, 6);

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
        bgfx::makeRef(s_cubeVertices, sizeof(s_cubeVertices))
        , PosColorVertex::ms_layout
    );

    // Create static index buffer.
    m_selection_ibh = bgfx::createIndexBuffer(
        bgfx::makeRef(s_cubeTriList, sizeof(s_cubeTriList))
    );

    m_color[0] = 0.62f;
    m_color[1] = 0.083f;
    m_color[2] = 0.653f;
    m_color[3] = 1.0f;

    m_selection_program = loadProgram("vs_selection", "fs_selection");
    u_color = bgfx::createUniform("u_color", bgfx::UniformType::Vec4);

    m_program = loadProgram("vs_instancing", "fs_instancing");

    m_tube_program = loadProgram("vs_bezier_tube", "fs_bezier_tube");
    m_single_tube_program = loadProgram("vs_single_tube", "fs_single_tube");
    // m_tube_program = loadProgram("vs_tube_test", "fs_tube_test");
}

void NetworkRenderStrategy::addLayer(const NeuronLayer& layer, const std::vector<size_t>& area_size) {
    area_size_x = area_size[0];
    area_size_y = area_size[1];
    area_size_z = area_size[2];

}

void NetworkRenderStrategy::update(float dt) {
    if (ImGui::IsKeyPressed(ImGuiKey_W))
        selected_z = std::min(max_area_size - 1, selected_z + 1);
    if (ImGui::IsKeyPressed(ImGuiKey_S))
        selected_z = std::max(0, selected_z - 1);

    if (ImGui::IsKeyPressed(ImGuiKey_D))
        selected_x = std::min(max_area_size - 1, selected_x + 1);
    if (ImGui::IsKeyPressed(ImGuiKey_A))
        selected_x = std::max(0, selected_x - 1);

    if (ImGui::IsKeyPressed(ImGuiKey_R))
        selected_y = std::min(max_area_size - 1, selected_y + 1);
    if (ImGui::IsKeyPressed(ImGuiKey_F))
        selected_y = std::max(0, selected_y - 1);

    if (ImGui::IsKeyPressed(ImGuiKey_Space)) {
        std::vector<int32_t> pos = { selected_x, selected_y, selected_z };
        if (selected_neuron == nullptr) {
            // create neuron
            ctx->net->addNeuron(pos, {});
        }
    }

    if (ImGui::IsKeyPressed(ImGuiKey_C)) {
        std::vector<int32_t> pos = { selected_x, selected_y, selected_z };
        if (synapse_from_neuron != nullptr && selected_neuron != nullptr) {
            if (synapse_from_neuron == selected_neuron) {
                synapse_from_neuron = nullptr;
                std::cout << "*reset* FROM neuron set\n";
            }
            else {
                std::cout << "TO neuron set!\n";
                ctx->net->addConnection(synapse_from_neuron, selected_neuron);
                synapse_from_neuron = nullptr;
                std::cout << "--- reset ---\n";
            }
        }
        else if (selected_neuron != nullptr) {
            synapse_from_neuron = selected_neuron;
            std::cout << "FROM neuron set!\n";
        }
        else {
            synapse_from_neuron = nullptr;
            std::cout << "*reset* FROM neuron set\n";
        }

    }

    // to total number of instances to draw
    uint32_t totalCubes = ctx->net->neurons.size();

    if (totalCubes > 0) {
        // figure out how big of a buffer is available
        drawnCubes = bgfx::getAvailInstanceDataBuffer(totalCubes, neuronInstanceStride);

        // save how many we couldn't draw due to buffer room so we can display it
        m_lastFrameMissing = totalCubes - drawnCubes;
    }
    else {
        drawnCubes = 0;
        m_lastFrameMissing = 0;
    }
}

void NetworkRenderStrategy::drawStandardTube(float time) {
    float sel_x = 0.0;
    float sel_y = 0.0;
    float sel_z = 0.0;

    // Create transform matrix
    float selMtx[16];
    bx::mtxIdentity(selMtx);
    bx::mtxTranslate(selMtx, sel_x, sel_y, sel_z);

    // Set transform, color, and render state
    bgfx::setTransform(selMtx);

    bgfx::setVertexBuffer(0, NeuronRenderStrategy::m_tube_vbh);
    bgfx::setIndexBuffer(NeuronRenderStrategy::m_tube_ibh);
    bgfx::setState(BGFX_STATE_WRITE_RGB);
    bgfx::submit(view_id, m_single_tube_program);
}

void NetworkRenderStrategy::drawSynapses(float time) {
    auto ss = ctx->net->synapses;
    if (ss.empty()) return;

    uint32_t drawSynapses = bgfx::getAvailInstanceDataBuffer(ss.size(), synapseInstanceStride);
    if (drawSynapses < ss.size()) {
        return;
    }

    bgfx::InstanceDataBuffer idb;
    bgfx::allocInstanceDataBuffer(&idb, ss.size(), synapseInstanceStride);

    uint8_t* dst = idb.data;

    for (const auto& [loc, s] : ss)
    {
        auto [pre_idx, post_idx] = Network::locationToIds(loc);

        const auto& neuron_pre = ctx->net->neurons[pre_idx];
        const auto& neuron_post = ctx->net->neurons[post_idx];

        std::shared_ptr<NeuronRenderStrategy>& s_pre = std::dynamic_pointer_cast<NeuronRenderStrategy>(neuron_pre->render);
        bx::Vec3 pos_pre = s_pre->get_world_position();
        uint32_t x_pre = pos_pre.x;
        uint32_t y_pre = pos_pre.y;
        uint32_t z_pre = pos_pre.z;

        std::shared_ptr<NeuronRenderStrategy>& s_post = std::dynamic_pointer_cast<NeuronRenderStrategy>(neuron_post->render);
        bx::Vec3 pos_post = s_post->get_world_position();
        uint32_t x_post = pos_post.x;
        uint32_t y_post = pos_post.y;
        uint32_t z_post = pos_post.z;

        // // draw synapse as a tube going from `pos_pre` to `pos_post`
        // // pick side offsets perpendicular to the straight segment.
        // bx::Vec3 d = bx::sub(pos_post, pos_pre);
        // float L = bx::length(d);
        // bx::Vec3 dir = bx::mul(d, 1.0f / (L + 1e-6f));

        // // pick a side vector
        // bx::Vec3 up = { 0,0,1 };
        // bx::Vec3 side = bx::cross(dir, up);
        // float sideLen = bx::length(side);
        // if (sideLen < 1e-4f) {
        //     up = { 0,1,0 };
        //     side = bx::cross(dir, up);
        // }
        // side = bx::mul(side, 1.0f / (bx::length(side) + 1e-6f));

        // float k = 0.25f * L; // curvature strength
        // bx::Vec3 p0 = pos_pre;
        // bx::Vec3 p3 = pos_post;
        // bx::Vec3 p1 = bx::mad(dir, 0.25f * L, bx::mad(side, k, p0)); // p0 + 0.25L*dir + k*side
        // bx::Vec3 p2 = bx::mad(dir, -0.25f * L, bx::mad(side, -k, p3)); // p3 - 0.25L*dir - k*side

        // s->p0 = p0;
        // s->p1 = p1;
        // s->p2 = p2;
        // s->p3 = p3;

        float* f = reinterpret_cast<float*>(dst);

        f[0] = pos_pre.x; 
        f[1] = pos_pre.y; 
        f[2] = pos_pre.z; 
        f[3] = 0.0f;
        f[4] = pos_post.x; 
        f[5] = pos_post.y; 
        f[6] = pos_post.z; 
        f[7] = 0.0f;

        // f[0] = s->p0.x; f[1] = s->p0.y; f[2] = s->p0.z; f[3] = s->radius; // i_data0
        // f[4] = s->p1.x; f[5] = s->p1.y; f[6] = s->p1.z; f[7] = 0.0f; // i_data1
        // f[8] = s->p2.x; f[9] = s->p2.y; f[10] = s->p2.z; f[11] = 0.0f; // i_data2
        // f[12] = s->p3.x; f[13] = s->p3.y; f[14] = s->p3.z; // i_data3
        // f[15] = 0.0f;

        dst += idb.stride;
    }

    bgfx::setVertexBuffer(0, NeuronRenderStrategy::m_tube_vbh);
    bgfx::setIndexBuffer(NeuronRenderStrategy::m_tube_ibh);
    bgfx::setInstanceDataBuffer(&idb);
    bgfx::setState(BGFX_STATE_WRITE_RGB);
    bgfx::submit(view_id, m_tube_program);
}

void NetworkRenderStrategy::drawNeurons(float time) {

    bgfx::InstanceDataBuffer idb;
    bgfx::allocInstanceDataBuffer(&idb, drawnCubes, neuronInstanceStride);

    uint8_t* data = idb.data;

    const float offset = 3.0f;
    const float start_x = -area_size_x * offset / 2.0f;
    const float start_y = -area_size_y * offset / 2.0f;
    const float start_z = -area_size_z * offset / 2.0f;

    // reset the selection
    selected_neuron = nullptr;

    for (uint32_t ii = 0; ii < drawnCubes; ++ii)
    {
        const auto& neuron = ctx->net->neurons[ii];

        std::shared_ptr<NeuronRenderStrategy>& s = std::dynamic_pointer_cast<NeuronRenderStrategy>(neuron->render);
        bx::Vec3 pos = s->get_world_position();
        float xx = pos.x;
        float yy = pos.y;
        float zz = pos.z;

        bx::Vec3 grid_pos = s->get_grid_position();
        if (uint32_t(grid_pos.x) == selected_x 
                && uint32_t(grid_pos.y) == selected_y 
                && uint32_t(grid_pos.z) == selected_z) {
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

        bx::mtxTranslate(mtx,
            start_x + xx,
            start_y + yy,
            start_z + zz
        );

        float* color = (float*)&data[64];
        float r = neuron->v;
        color[0] = r;
        color[1] = 0.0f;
        color[2] = 1.0f - r;
        // color[0] = bx::sin(time + float(xx) / 11.0f) * 0.5f + 0.5f;
        // color[1] = bx::cos(time + float(yy) / 11.0f) * 0.5f + 0.5f;
        // color[2] = bx::sin(time * 3.0f) * 0.5f + 0.5f;
        color[3] = 1.0f;

        data += idb.stride;
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
    const float start_x = -area_size_x * offset / 2.0f;
    const float start_y = -area_size_y * offset / 2.0f;
    const float start_z = -area_size_z * offset / 2.0f;

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
    // for (uint32_t ii = 0; ii < drawnCubes; ++ii)
    // {
    //     const auto& neuron = ctx->net->neurons[ii];

    //     std::shared_ptr<NeuronRenderStrategy>& s = std::dynamic_pointer_cast<NeuronRenderStrategy>(neuron->render);
    //     bx::Vec3 pos = s->get_world_position();
    //     uint32_t xx = pos.x;
    //     uint32_t yy = pos.y;
    //     uint32_t zz = pos.z;

    //     if (xx == selected_x && yy == selected_y && zz == selected_z) {
    //         selected_neuron = neuron;
    //     }
    // }

    ground->draw();

    drawSynapses(time);
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
