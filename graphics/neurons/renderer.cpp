#include "renderer.hpp"

// Color color_from_voltage(float v, bool spiked) {
//     if (spiked) return {1, 1, 0}; // yellow
//     float r = v;  // map [0,1] to red
//     return {r, 0, 1.0f - r};
// }


// void Renderer3D::update(const SNN& net) {
//     // Update instance buffer for all neurons
//     for (size_t i = 0; i < net.neurons.size(); ++i) {
//         auto& n = net.neurons[i];
//         Color c = color_from_voltage(n.v, n.spiked);
//         set_instance_transform(neuronInstances[i], n.position, c);
//     }
// }


// void Renderer3D::draw_synapses(const SNN& net) {
//     for (int i = 0; i < net.neurons.size(); ++i) {
//         for (int j = 0; j < net.neurons.size(); ++j) {
//             float w = net.synapses[i][j].weight;
//             if (w < 0.01f) continue;
//             draw_line(net.neurons[i].position, net.neurons[j].position, color_for_weight(w));
//         }
//     }
// }

// void drawNeuron(const RenderNeuron& n, const bgfx::ProgramHandle& shader) {
//     bx::mtxSRT(mtx, n.radius, n.radius, n.radius, 0, 0, 0, n.x, n.y, n.z);
//     bgfx::setTransform(mtx);
//     bgfx::setUniform(u_color, &n.color);
//     bgfx::setVertexBuffer(0, sphereVb);
//     bgfx::setIndexBuffer(sphereIb);
//     bgfx::setState(BGFX_STATE_DEFAULT);
//     bgfx::submit(0, shader);
// }

// void drawSynapse(const RenderSynapse& s, const bgfx::ProgramHandle& shader) {
//     float verts[] = {
//         s.x1, s.y1, s.z1,
//         s.x2, s.y2, s.z2
//     };
//     bgfx::TransientVertexBuffer tvb;
//     bgfx::allocTransientVertexBuffer(&tvb, 2, PosVertex::ms_layout);
//     memcpy(tvb.data, verts, sizeof(verts));
//     bgfx::setVertexBuffer(0, &tvb);
//     bgfx::setUniform(u_color, &s.color);
//     bgfx::setState(BGFX_STATE_PT_LINES);
//     bgfx::submit(0, shader);
// }
