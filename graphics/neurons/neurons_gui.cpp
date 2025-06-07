// #include "snn/network.hpp"
// #include "bgfx_renderer.hpp"


std::vector<RenderNeuron> renderNeurons;
std::vector<RenderSynapse> renderSynapses;

SNN net(10);
Renderer3D renderer;

void updateRenderDataFromSNN() {

    for (size_t i = 0; i < snn.neurons.size(); ++i) {
        Neuron& n = snn.neurons[i];
        float intensity = n.v / n.threshold;
        if (n.spiked) intensity = 1.0f;

        uint32_t color = packRgba(intensity, 0.0f, 1.0f - intensity, 1.0f);

        renderNeurons.push_back({
            position[i].x, position[i].y, position[i].z,
            0.2f,
            color
        });
    }

    for (size_t i = 0; i < snn.neurons.size(); ++i) {
        for (size_t j = 0; j < snn.neurons.size(); ++j) {
            float w = snn.synapses[i][j].weight;
            if (w > 0.01f) {
                renderSynapses.push_back({
                    position[j].x, position[j].y, position[j].z,
                    position[i].x, position[i].y, position[i].z,
                    packRgba(w, w, w, 1.0f)
                });
            }
        }
    }
}

void render_frame() {
    snn.step();

    renderNeurons.clear();
    renderSynapses.clear();
    updateRenderDataFromSNN(snn);

    for (auto& n : renderNeurons)
        drawNeuron(n, solidShader);

    for (auto& s : renderSynapses)
        drawSynapse(s, lineShader);

    ImGui::Text("Time: %.1f ms", snn.time);
}

void setup_scene(SNN& net) {
    int N = net.neurons.size();
    for (int i = 0; i < N; ++i) {
        float angle = i * 2.0f * M_PI / N;
        float x = cos(angle) * 5.0f;
        float y = sin(angle) * 5.0f;
        net.neurons[i].position = {x, y, 0.0f};
    }
}

int main() {
    init_bgfx();  // setup window, bgfx, camera
    setup_scene(net);  // generate neuron positions

    while (!exit_requested()) {
        net.step();
        renderer.update(net);
        render_frame();
    }

    shutdown_bgfx();
    return 0;
}