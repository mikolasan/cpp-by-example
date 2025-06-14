#include <vector>
#include <bgfx/bgfx.h>

#include "network.hpp"

struct RenderNeuron {
    float x, y, z;
    float radius;
    uint32_t color;  // bgfx packed RGBA
};

struct RenderSynapse {
    float x1, y1, z1;
    float x2, y2, z2;
    uint32_t color;
};

struct Renderer3D {
    bgfx::ProgramHandle sphereShader;
    bgfx::VertexBufferHandle sphereVbo;
    bgfx::IndexBufferHandle sphereIbo;
    bgfx::UniformHandle colorUniform;
    std::vector<bgfx::InstanceDataBuffer> neuronInstances;

    void init();
    void update(const SNN& net);
    void render(const SNN& net);
};

