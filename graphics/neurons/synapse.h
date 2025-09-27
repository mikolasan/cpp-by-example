#ifndef SYNAPSE_H
#define SYNAPSE_H

#include <bx/math.h>
// #include <bgfx/bgfx.h>

struct Synapse {
    float weight = 0.5f;
    float tau_pre = 20.0f;
    float tau_post = 20.0f;
    float A_plus = 0.01f;
    float A_minus = -0.012f;
    float trace_pre = 0.0f;
    float trace_post = 0.0f;

    // TODO: extract into a dedicated renderer
    bx::Vec3 p0 = bx::Vec3(bx::InitZero);
    bx::Vec3 p1 = bx::Vec3(bx::InitZero);
    bx::Vec3 p2 = bx::Vec3(bx::InitZero);
    bx::Vec3 p3 = bx::Vec3(bx::InitZero);
    float radius = 0.3;

    void update_pre(float dt);
    void update_post(float dt);

    void on_pre_spike();
    void on_post_spike();

    void apply_stdp(bool pre_spike, bool post_spike);
};

#endif