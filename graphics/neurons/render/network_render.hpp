#pragma once

#include <memory>

#include "render_strategy.h"

struct Network;

struct NetworkVisualContext : VisualContext {
    // std::vector<bx::Vec3> positions;
    Network& net;
};

struct NetworkRenderStrategy : RenderStrategy {
    std::shared_ptr<NetworkVisualContext> ctx;

    NetworkRenderStrategy(std::shared_ptr<NetworkVisualContext> ctx) : ctx(ctx) {}

    void init() override {
        // Optional: nothing for now
    }

    void update(float dt) override {
        // Optional: nothing for now
    }

    void draw() const override {
        for (size_t i = 0; i < ctx->net.neurons.size(); i++) {
            const auto& n = ctx->net.neurons[i];
            // const bx::Vec3& pos = ctx->positions[i];
            n.draw();
        }

    }
};