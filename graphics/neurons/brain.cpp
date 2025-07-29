#include <map>
#include <memory>
#include <string>
#include <vector>

#include <bx/uint32_t.h>
#include <bx/timer.h>
#include <bx/math.h>
#include <bgfx/bgfx.h>
#include <bx/bx.h>
#include <bx/filepath.h>
#include <bx/string.h>

#include "entry/entry.h"
#include "bgfx_utils.h"
#include "logo.h"
#include "imgui/bgfx_imgui.h"

// Vertex structure for brain mesh
struct Vertex {
    float x, y, z;
    float nx, ny, nz;
    uint32_t color;

    static void init() {
        ms_decl
            .begin()
            .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
            .add(bgfx::Attrib::Normal, 3, bgfx::AttribType::Float)
            .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
            .end();
    }

    static bgfx::VertexLayout ms_decl;
};

bgfx::VertexLayout Vertex::ms_decl;

// Brain region structure
struct BrainRegion {
    std::string name;
    std::vector<Vertex> vertices;
    std::vector<uint16_t> indices;
    bx::Vec3 position;
    bx::Vec3 originalPosition;
    bx::Vec3 color;
    bool isVisible;
    bool isHighlighted;
    bool isInternal;
    float transparency;

    bgfx::VertexBufferHandle vbh;
    bgfx::IndexBufferHandle ibh;

    BrainRegion(const std::string& n, bool internal = false)
        :
        name(n),
        isVisible(true),
        isHighlighted(false),
        isInternal(internal),
        transparency(1.0f),
        position({ 0.0f, 0.0f, 0.0f }),
        originalPosition({ 0.0f, 0.0f, 0.0f }),
        color({ 0.8f, 0.8f, 0.8f })
    {
    }

    void createBuffers() {
        if (!vertices.empty()) {
            vbh = bgfx::createVertexBuffer(
                bgfx::makeRef(vertices.data(), vertices.size() * sizeof(Vertex)),
                Vertex::ms_decl
            );
        }

        if (!indices.empty()) {
            ibh = bgfx::createIndexBuffer(
                bgfx::makeRef(indices.data(), indices.size() * sizeof(uint16_t))
            );
        }
    }

    void destroy() {
        if (bgfx::isValid(vbh)) bgfx::destroy(vbh);
        if (bgfx::isValid(ibh)) bgfx::destroy(ibh);
    }
};

class BrainModelGenerator {
private:
    std::map<std::string, std::unique_ptr<BrainRegion>> regions;
    bool explodedView;
    float explosionFactor;

public:
    BrainModelGenerator() : 
    explodedView(false), 
    explosionFactor(2.0f) 
    {}

    // Generate a sphere with given parameters
    void generateSphere(std::vector<Vertex>& vertices, std::vector<uint16_t>& indices,
        float radius, int segments, const bx::Vec3& center, uint32_t color) {
        vertices.clear();
        indices.clear();

        // Generate vertices
        for (int i = 0; i <= segments; ++i) {
            float theta = (float)i * bx::kPi / segments;
            float sinTheta = bx::sin(theta);
            float cosTheta = bx::cos(theta);

            for (int j = 0; j <= segments; ++j) {
                float phi = (float)j * 2.0f * bx::kPi / segments;
                float sinPhi = bx::sin(phi);
                float cosPhi = bx::cos(phi);

                float x = radius * sinTheta * cosPhi;
                float y = radius * cosTheta;
                float z = radius * sinTheta * sinPhi;

                Vertex vertex;
                vertex.x = x + center.x;
                vertex.y = y + center.y;
                vertex.z = z + center.z;
                vertex.nx = sinTheta * cosPhi;
                vertex.ny = cosTheta;
                vertex.nz = sinTheta * sinPhi;
                vertex.color = color;

                vertices.push_back(vertex);
            }
        }

        // Generate indices
        for (int i = 0; i < segments; ++i) {
            for (int j = 0; j < segments; ++j) {
                int first = i * (segments + 1) + j;
                int second = first + segments + 1;

                indices.push_back(first);
                indices.push_back(second);
                indices.push_back(first + 1);

                indices.push_back(second);
                indices.push_back(second + 1);
                indices.push_back(first + 1);
            }
        }
    }

    // Generate ellipsoid for brain-like shapes
    void generateEllipsoid(std::vector<Vertex>& vertices, std::vector<uint16_t>& indices,
        float radiusX, float radiusY, float radiusZ, int segments,
        const bx::Vec3& center, uint32_t color) {
        vertices.clear();
        indices.clear();

        for (int i = 0; i <= segments; ++i) {
            float theta = (float)i * bx::kPi / segments;
            float sinTheta = bx::sin(theta);
            float cosTheta = bx::cos(theta);

            for (int j = 0; j <= segments; ++j) {
                float phi = (float)j * 2.0f * bx::kPi / segments;
                float sinPhi = bx::sin(phi);
                float cosPhi = bx::cos(phi);

                float x = radiusX * sinTheta * cosPhi;
                float y = radiusY * cosTheta;
                float z = radiusZ * sinTheta * sinPhi;

                Vertex vertex;
                vertex.x = x + center.x;
                vertex.y = y + center.y;
                vertex.z = z + center.z;
                vertex.nx = sinTheta * cosPhi;
                vertex.ny = cosTheta;
                vertex.nz = sinTheta * sinPhi;
                vertex.color = color;

                vertices.push_back(vertex);
            }
        }

        // Generate indices (same as sphere)
        for (int i = 0; i < segments; ++i) {
            for (int j = 0; j < segments; ++j) {
                int first = i * (segments + 1) + j;
                int second = first + segments + 1;

                indices.push_back(first);
                indices.push_back(second);
                indices.push_back(first + 1);

                indices.push_back(second);
                indices.push_back(second + 1);
                indices.push_back(first + 1);
            }
        }
    }

    // Generate a deformed sphere for cortical regions
    void generateCorticalRegion(std::vector<Vertex>& vertices, std::vector<uint16_t>& indices,
        float baseRadius, const bx::Vec3& center, uint32_t color,
        float deformX = 1.0f, float deformY = 1.0f, float deformZ = 1.0f) {
        vertices.clear();
        indices.clear();

        int segments = 20;

        for (int i = 0; i <= segments; ++i) {
            float theta = (float)i * bx::kPi / segments;
            float sinTheta = bx::sin(theta);
            float cosTheta = bx::cos(theta);

            for (int j = 0; j <= segments; ++j) {
                float phi = (float)j * 2.0f * bx::kPi / segments;
                float sinPhi = bx::sin(phi);
                float cosPhi = bx::cos(phi);

                // Add some noise for brain-like surface
                float noise = 0.1f * bx::sin(theta * 3.0f) * bx::cos(phi * 4.0f);
                float radius = baseRadius + noise;

                float x = radius * sinTheta * cosPhi * deformX;
                float y = radius * cosTheta * deformY;
                float z = radius * sinTheta * sinPhi * deformZ;

                Vertex vertex;
                vertex.x = x + center.x;
                vertex.y = y + center.y;
                vertex.z = z + center.z;
                vertex.nx = sinTheta * cosPhi;
                vertex.ny = cosTheta;
                vertex.nz = sinTheta * sinPhi;
                vertex.color = color;

                vertices.push_back(vertex);
            }
        }

        // Generate indices
        for (int i = 0; i < segments; ++i) {
            for (int j = 0; j < segments; ++j) {
                int first = i * (segments + 1) + j;
                int second = first + segments + 1;

                indices.push_back(first);
                indices.push_back(second);
                indices.push_back(first + 1);

                indices.push_back(second);
                indices.push_back(second + 1);
                indices.push_back(first + 1);
            }
        }
    }

    uint32_t colorToRGBA(const bx::Vec3& color, float alpha = 1.0f) {
        uint8_t r = (uint8_t)(color.x * 255.0f);
        uint8_t g = (uint8_t)(color.y * 255.0f);
        uint8_t b = (uint8_t)(color.z * 255.0f);
        uint8_t a = (uint8_t)(alpha * 255.0f);
        return (a << 24) | (b << 16) | (g << 8) | r;
    }

    void generateBrainRegions() {
        // Color scheme for different regions
        bx::Vec3 motorColor(0.8f, 0.2f, 0.2f);        // Red
        bx::Vec3 somatosensoryColor(0.2f, 0.8f, 0.2f); // Green
        bx::Vec3 frontalColor(0.2f, 0.2f, 0.8f);       // Blue
        bx::Vec3 parietalColor(0.8f, 0.8f, 0.2f);      // Yellow
        bx::Vec3 temporalColor(0.8f, 0.2f, 0.8f);      // Magenta
        bx::Vec3 limbicColor(0.2f, 0.8f, 0.8f);        // Cyan
        bx::Vec3 subcorticalColor(0.6f, 0.3f, 0.1f);   // Brown

        // Premotor Cortex
        auto premotor = std::make_unique<BrainRegion>("Premotor Cortex");
        premotor->color = motorColor;
        premotor->position = bx::Vec3(-2.0f, 1.0f, 0.5f);
        premotor->originalPosition = premotor->position;
        generateCorticalRegion(premotor->vertices, premotor->indices,
            0.8f, premotor->position,
            colorToRGBA(premotor->color), 1.2f, 0.8f, 0.9f);
        regions["premotor"] = std::move(premotor);

        // Supplementary Motor Area
        auto sma = std::make_unique<BrainRegion>("Supplementary Motor Area");
        sma->color = motorColor;
        sma->position = bx::Vec3(0.0f, 2.0f, 0.0f);
        sma->originalPosition = sma->position;
        generateCorticalRegion(sma->vertices, sma->indices,
            0.6f, sma->position,
            colorToRGBA(sma->color), 1.5f, 0.6f, 0.8f);
        regions["sma"] = std::move(sma);

        // Primary Somatosensory Cortex
        auto s1 = std::make_unique<BrainRegion>("Primary Somatosensory Cortex");
        s1->color = somatosensoryColor;
        s1->position = bx::Vec3(-1.0f, 0.5f, 0.0f);
        s1->originalPosition = s1->position;
        generateCorticalRegion(s1->vertices, s1->indices,
            0.9f, s1->position,
            colorToRGBA(s1->color), 1.3f, 0.7f, 1.0f);
        regions["s1"] = std::move(s1);

        // Inferior Parietal Cortex
        auto ipc = std::make_unique<BrainRegion>("Inferior Parietal Cortex");
        ipc->color = parietalColor;
        ipc->position = bx::Vec3(-1.5f, 0.0f, -1.0f);
        ipc->originalPosition = ipc->position;
        generateCorticalRegion(ipc->vertices, ipc->indices,
            0.7f, ipc->position,
            colorToRGBA(ipc->color), 1.1f, 0.9f, 1.2f);
        regions["ipc"] = std::move(ipc);

        // Ventromedial Prefrontal Cortex
        auto vmpfc = std::make_unique<BrainRegion>("Ventromedial Prefrontal Cortex");
        vmpfc->color = frontalColor;
        vmpfc->position = bx::Vec3(1.5f, 0.0f, -0.5f);
        vmpfc->originalPosition = vmpfc->position;
        generateCorticalRegion(vmpfc->vertices, vmpfc->indices,
            0.8f, vmpfc->position,
            colorToRGBA(vmpfc->color), 0.9f, 1.1f, 0.8f);
        regions["vmpfc"] = std::move(vmpfc);

        // Anterior Cingulate Cortex
        auto acc = std::make_unique<BrainRegion>("Anterior Cingulate Cortex");
        acc->color = limbicColor;
        acc->position = bx::Vec3(0.5f, 0.5f, -0.3f);
        acc->originalPosition = acc->position;
        generateCorticalRegion(acc->vertices, acc->indices,
            0.6f, acc->position,
            colorToRGBA(acc->color), 1.4f, 0.5f, 0.7f);
        regions["acc"] = std::move(acc);

        // Superior Temporal Sulcus
        auto sts = std::make_unique<BrainRegion>("Superior Temporal Sulcus");
        sts->color = temporalColor;
        sts->position = bx::Vec3(-2.0f, -0.5f, 0.0f);
        sts->originalPosition = sts->position;
        generateCorticalRegion(sts->vertices, sts->indices,
            0.7f, sts->position,
            colorToRGBA(sts->color), 1.3f, 0.6f, 1.0f);
        regions["sts"] = std::move(sts);

        // Internal regions

        // Amygdala
        auto amygdala = std::make_unique<BrainRegion>("Amygdala", true);
        amygdala->color = limbicColor;
        amygdala->position = bx::Vec3(0.8f, -0.3f, -0.8f);
        amygdala->originalPosition = amygdala->position;
        generateEllipsoid(amygdala->vertices, amygdala->indices,
            0.3f, 0.2f, 0.4f, 16, amygdala->position,
            colorToRGBA(amygdala->color));
        regions["amygdala"] = std::move(amygdala);

        // Hypothalamus
        auto hypothalamus = std::make_unique<BrainRegion>("Hypothalamus", true);
        hypothalamus->color = subcorticalColor;
        hypothalamus->position = bx::Vec3(0.0f, -0.8f, -0.5f);
        hypothalamus->originalPosition = hypothalamus->position;
        generateEllipsoid(hypothalamus->vertices, hypothalamus->indices,
            0.4f, 0.3f, 0.3f, 16, hypothalamus->position,
            colorToRGBA(hypothalamus->color));
        regions["hypothalamus"] = std::move(hypothalamus);

        // Basal Ganglia (simplified as single structure)
        auto basalGanglia = std::make_unique<BrainRegion>("Basal Ganglia", true);
        basalGanglia->color = subcorticalColor;
        basalGanglia->position = bx::Vec3(0.0f, -0.2f, -0.3f);
        basalGanglia->originalPosition = basalGanglia->position;
        generateEllipsoid(basalGanglia->vertices, basalGanglia->indices,
            0.6f, 0.5f, 0.7f, 16, basalGanglia->position,
            colorToRGBA(basalGanglia->color));
        regions["basalGanglia"] = std::move(basalGanglia);

        // Create GPU buffers for all regions
        for (auto& [name, region] : regions) {
            region->createBuffers();
        }
    }

    void updatePositions() {
        if (explodedView) {
            for (auto& [name, region] : regions) {
                if (region->isInternal) {
                    // Move internal regions outward
                    bx::Vec3 direction = region->originalPosition;
                    bx::normalize(direction);
                    region->position = bx::add(region->originalPosition,
                        bx::mul(direction, explosionFactor));
                }
            }
        }
        else {
            for (auto& [name, region] : regions) {
                region->position = region->originalPosition;
            }
        }
    }

    void render(bgfx::ProgramHandle program, const float* view, const float* proj) {
        for (auto& [name, region] : regions) {
            if (!region->isVisible) continue;

            // Model matrix with translation
            float model[16];
            bx::mtxTranslate(model, region->position.x, region->position.y, region->position.z);

            // Set transform
            bgfx::setTransform(model);

            // Set vertex and index buffers
            bgfx::setVertexBuffer(0, region->vbh);
            bgfx::setIndexBuffer(region->ibh);

            // Render state
            uint64_t state = BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_WRITE_Z
                | BGFX_STATE_DEPTH_TEST_LESS | BGFX_STATE_CULL_CCW;

            if (region->transparency < 1.0f) {
                state |= BGFX_STATE_BLEND_ALPHA;
            }

            bgfx::setState(state);

            // Submit draw call
            bgfx::submit(0, program);
        }
    }

    void renderGUI() {
        ImGui::Begin("Brain Regions Control");

        if (ImGui::Checkbox("Exploded View", &explodedView)) {
            updatePositions();
        }

        if (explodedView) {
            if (ImGui::SliderFloat("Explosion Factor", &explosionFactor, 1.0f, 5.0f)) {
                updatePositions();
            }
        }

        ImGui::Separator();

        for (auto& [name, region] : regions) {
            ImGui::PushID(name.c_str());

            if (ImGui::TreeNode(region->name.c_str())) {
                ImGui::Checkbox("Visible", &region->isVisible);
                ImGui::Checkbox("Highlighted", &region->isHighlighted);

                if (region->isInternal) {
                    ImGui::Text("Internal Region");
                }

                ImGui::ColorEdit3("Color", &region->color.x);
                ImGui::SliderFloat("Transparency", &region->transparency, 0.0f, 1.0f);

                ImGui::TreePop();
            }

            ImGui::PopID();
        }

        ImGui::End();
    }

    void destroy() {
        for (auto& [name, region] : regions) {
            region->destroy();
        }
    }
};

// Main application class
class BrainVisualization : public entry::AppI {
private:
    BrainModelGenerator brainModel;
    bgfx::ProgramHandle program;

    float cameraDistance;
    float cameraAngleX;
    float cameraAngleY;
    bool mouseDown;
    float lastMouseX, lastMouseY;

    entry::MouseState m_mouseState;

    uint32_t m_width;
    uint32_t m_height;
    uint32_t m_debug;
    uint32_t m_reset;
    uint32_t m_lastFrameMissing;


    bgfx::VertexBufferHandle m_vbh;
    bgfx::IndexBufferHandle m_ibh;
    bgfx::ProgramHandle m_program;

    int64_t m_timeOffset;


public:
    BrainVisualization() :
        entry::AppI("BRAIN", "brain", ""),
        cameraDistance(5.0f),
        cameraAngleX(0.0f),
        cameraAngleY(0.0f),
        mouseDown(false)
    {
    }

    void init(int32_t _argc, const char* const* _argv, uint32_t _width, uint32_t _height) override
    {
        Args args(_argc, _argv);

        m_width = _width;
        m_height = _height;
        m_debug = BGFX_DEBUG_TEXT;
        m_reset = BGFX_RESET_VSYNC;

        bgfx::Init init;
        init.type = args.m_type;
        init.vendorId = args.m_pciId;
        init.platformData.nwh = entry::getNativeWindowHandle(entry::kDefaultWindowHandle);
        init.platformData.ndt = entry::getNativeDisplayHandle();
        init.platformData.type = entry::getNativeWindowHandleType();
        init.resolution.width = m_width;
        init.resolution.height = m_height;
        init.resolution.reset = m_reset;
        bgfx::init(init);

        // Enable debug text.
        bgfx::setDebug(m_debug);

        // Set view 0 clear state.
        bgfx::setViewClear(0
            , BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH
            , 0x303030ff
            , 1.0f
            , 0
        );

        // Initialize vertex declaration
        Vertex::init();

        // Create shaders and program (simplified - you'd load actual shaders)
        // This is a placeholder - in real implementation you'd load vertex and fragment shaders
        program = BGFX_INVALID_HANDLE; // Replace with actual shader loading

        imguiCreate();

        // Generate brain model
        brainModel.generateBrainRegions();
    }

    bool update() override
    {
        if (!entry::processEvents(m_width, m_height, m_debug, m_reset, &m_mouseState))
        {

            // Calculate view matrix
            float view[16];
            const bx::Vec3 eye = {
                cameraDistance * bx::sin(cameraAngleX) * bx::cos(cameraAngleY),
                cameraDistance * bx::sin(cameraAngleY),
                cameraDistance * bx::cos(cameraAngleX) * bx::cos(cameraAngleY)
            };
            const bx::Vec3 at = { 0.0f, 0.0f, 0.0f };
            // float up[3] = {0.0f, 1.0f, 0.0f};
            bx::mtxLookAt(view, eye, at);

            // Calculate projection matrix
            float proj[16];
            bx::mtxProj(proj, 45.0f, 1.0f, 0.1f, 100.0f, bgfx::getCaps()->homogeneousDepth);

            // Set view and projection matrices
            bgfx::setViewTransform(0, view, proj);

            // Render brain model
            brainModel.render(program, view, proj);

            // Render GUI
            brainModel.renderGUI();

            return true;
        }

        return false;
    }

    int shutdown() override
    {
        imguiDestroy();

        brainModel.destroy();

        bgfx::shutdown();
        return 0;
    }
};


int32_t _main_(int32_t argc, char** argv)
{
    entry::AppI *app = new BrainVisualization();
    return entry::runApp(app, argc, argv);
}
