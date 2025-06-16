#include <iostream>

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

#include "network.hpp"

namespace
{



struct PosColorVertex
{
	float m_x;
	float m_y;
	float m_z;
	uint32_t m_abgr;

	static void init()
	{
		ms_layout
			.begin()
			.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
			.add(bgfx::Attrib::Color0,   4, bgfx::AttribType::Uint8, true)
			.end();
	};

	static bgfx::VertexLayout ms_layout;
};

bgfx::VertexLayout PosColorVertex::ms_layout;

// cube

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

static const uint16_t s_cubeIndices[36] =
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

// sphere

void generateSphereMesh(
    std::vector<PosColorVertex>& vertices,
    std::vector<uint16_t>& indices,
    int stacks = 2, // vertical segments (horizontal lines make stack of pancakes)
    int slices = 4, // horizontal segments (vertical lines make slices like with apples or oranges)
    float radius = 1.0f)
{
    vertices.clear();
    vertices.reserve((stacks + 1) * (slices + 1));
    indices.clear();

    for (int i = 0; i <= stacks; ++i)
    {
		// going from top to bottom

        float v = float(i) / stacks;
        float phi = v * bx::kPi; // 0 to π

        for (int j = 0; j <= slices; ++j)
        {
            float u = float(j) / slices;
            float theta = u * bx::kPi * 2.0f; // 0 to 2π

            float x = sinf(phi) * cosf(theta);
            float y = cosf(phi);
            float z = sinf(phi) * sinf(theta);

            PosColorVertex vtx;
            vtx.m_x = x * radius;
            vtx.m_y = y * radius;
            vtx.m_z = z * radius;

            // Color based on position for debugging
            uint8_t r = uint8_t((x * 0.5f + 0.5f) * 255);
            uint8_t g = uint8_t((y * 0.5f + 0.5f) * 255);
            uint8_t b = uint8_t((z * 0.5f + 0.5f) * 255);
            vtx.m_abgr = (r) | (g << 8) | (b << 16) | 0xff000000;

            vertices.push_back(vtx);
        }
    }

    // Generate indices

    //   V03----------V02----------V01---------V00 
    //    |            |        __/ |        __/|
    //    |            |     __/    |  c  __/   |
    //    |            |  __/       |  __/      |
    //    |            | /        f | /     d   |
    //   V13----------V12----------V11----------V10 
    //    |            |        __/ | e      __/|
    //    |            |  a  __/    |     __/   |
    //    |            |  __/       |  __/      |
    //    |            | /      b   | /         |
    //   V23----------V22----------V21----------V20 
    //    |            |            |           |
    //    |            |            |           |
    //    |            |            |           |
    //    |            |            |           |
    //   V33----------V32----------V31---------V30 

    for (int i = 0; i < stacks; ++i)
    {
        // going from top to bottom

        for (int j = 0; j < slices; ++j)
        {
            int row1 = i * (slices + 1);
            int row2 = (i + 1) * (slices + 1);

            // triangle 1
            indices.push_back(uint16_t(row1 + j));
            indices.push_back(uint16_t(row2 + j));
            indices.push_back(uint16_t(row2 + j + 1));
            // triangle 2
            indices.push_back(uint16_t(row1 + j));
            indices.push_back(uint16_t(row2 + j + 1));
            indices.push_back(uint16_t(row1 + j + 1));
            // = one quad
        }
    }
}

class ExampleHelloWorld : public entry::AppI
{
public:
	ExampleHelloWorld(int n_neurons)
		: entry::AppI("NEUF", "spiking network simulator", "")
	{
		net.setSize(n_neurons);
	}

	// void setup_visuals(Network& net) {
  //   auto ctx = std::make_shared<VisualContext>();
  //   ctx->net = net;

  //   for (size_t i = 0; i < net.neurons.size(); ++i) {
  //       float angle = i * 2 * M_PI / net.neurons.size();
  //       ctx->positions.emplace_back(cos(angle)*5.0f, sin(angle)*5.0f, 0.0f);
  //   }

  //   auto render_strategy = std::make_shared<RenderNeuronStrategy>(ctx);

  //   for (auto& n : net.neurons)
  //       n.strategy = render_strategy;
	// }

	void init(int32_t _argc, const char* const* _argv, uint32_t _width, uint32_t _height) override
	{
		Args args(_argc, _argv);

		m_width  = _width;
		m_height = _height;
		m_debug  = BGFX_DEBUG_TEXT;
		m_reset  = BGFX_RESET_VSYNC;
		m_useInstancing    = true;
		m_lastFrameMissing = 0;
		m_sideSize         = 1;

		bgfx::Init init;
		init.type     = args.m_type;
		init.vendorId = args.m_pciId;
		init.platformData.nwh  = entry::getNativeWindowHandle(entry::kDefaultWindowHandle);
		init.platformData.ndt  = entry::getNativeDisplayHandle();
		init.platformData.type = entry::getNativeWindowHandleType();
		init.resolution.width  = m_width;
		init.resolution.height = m_height;
		init.resolution.reset  = m_reset;
		bgfx::init(init);

		// Enable debug text.
		bgfx::setDebug(m_debug);

		// Set view 0 clear state.
		bgfx::setViewClear(0
			, BGFX_CLEAR_COLOR|BGFX_CLEAR_DEPTH
			, 0x303030ff
			, 1.0f
			, 0
			);


		// Create vertex stream declaration.
		PosColorVertex::init();

		std::vector<PosColorVertex> vertices;
		std::vector<uint16_t> indices;
		generateSphereMesh(vertices, indices);

		std::cout << "vertices" << std::endl;
		for (const auto& v : vertices) {
			std::cout << "(" << v.m_x << ", "
				<< v.m_y << ", "
				<< v.m_z << ") ";
		}
		std::cout << std::endl;

		std::cout << "indices" << std::endl;
		for (const auto& i : indices) {
			std::cout << i << " ";
		}
		std::cout << std::endl;

		// Create static vertex buffer.
		m_vbh = bgfx::createVertexBuffer(
					  bgfx::makeRef(vertices.data(), uint32_t(vertices.size() * sizeof(PosColorVertex)))
					, PosColorVertex::ms_layout
					);

		// Create static index buffer.
		m_ibh = bgfx::createIndexBuffer(
					bgfx::makeRef(indices.data(), uint32_t(indices.size() * sizeof(uint16_t)))
					);

		// Create program from shaders.
		m_program = loadProgram("vs_instancing", "fs_instancing");
		m_program_non_instanced = loadProgram("vs_cubes", "fs_cubes");

		m_timeOffset = bx::getHPCounter();

		imguiCreate();
	}

	int shutdown() override
	{
		imguiDestroy();

		// Cleanup.
		bgfx::destroy(m_ibh);
		bgfx::destroy(m_vbh);
		bgfx::destroy(m_program);
		bgfx::destroy(m_program_non_instanced);

		// Shutdown bgfx.
		bgfx::shutdown();

		return 0;
	}

	bool update() override
	{
		if (!entry::processEvents(m_width, m_height, m_debug, m_reset, &m_mouseState) )
		{
			imguiBeginFrame(m_mouseState.m_mx
				,  m_mouseState.m_my
				, (m_mouseState.m_buttons[entry::MouseButton::Left  ] ? IMGUI_MBUT_LEFT   : 0)
				| (m_mouseState.m_buttons[entry::MouseButton::Right ] ? IMGUI_MBUT_RIGHT  : 0)
				| (m_mouseState.m_buttons[entry::MouseButton::Middle] ? IMGUI_MBUT_MIDDLE : 0)
				,  m_mouseState.m_mz
				, uint16_t(m_width)
				, uint16_t(m_height)
				);

			showExampleDialog(this);

			ImGui::SetNextWindowPos(
				ImVec2(m_width - m_width / 5.0f - 10.0f, 10.0f)
				, ImGuiCond_FirstUseEver
			);
			ImGui::SetNextWindowSize(
				ImVec2(m_width / 5.0f, m_height / 2.0f)
				, ImGuiCond_FirstUseEver
			);
			ImGui::Begin("Settings"
				, NULL
				, 0
			);

			// Get renderer capabilities info.
			const bgfx::Caps* caps = bgfx::getCaps();

			// Check if instancing is supported.
			const bool instancingSupported = 0 != (BGFX_CAPS_INSTANCING & caps->supported);
			m_useInstancing &= instancingSupported;

			ImGui::Text("%d draw calls", bgfx::getStats()->numDraw);

			ImGui::PushEnabled(instancingSupported);
			ImGui::Checkbox("Use Instancing", &m_useInstancing);
			ImGui::PopEnabled();

			ImGui::Text("Grid Side Size:");
			ImGui::SliderInt("##size", (int*)&m_sideSize, 1, 512);

			if (m_lastFrameMissing > 0)
			{
				ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Couldn't draw %d cubes last frame", m_lastFrameMissing);
			}

			if (bgfx::getStats()->numDraw >= bgfx::getCaps()->limits.maxDrawCalls)
			{
				ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Draw call limit reached!");
			}

			ImGui::End();

			imguiEndFrame();

			// Set view 0 default viewport.
			bgfx::setViewRect(0, 0, 0, uint16_t(m_width), uint16_t(m_height) );

			// This dummy draw call is here to make sure that view 0 is cleared
			// if no other draw calls are submitted to view 0.
			bgfx::touch(0);

			float time = (float)( (bx::getHPCounter() - m_timeOffset)/double(bx::getHPFrequency() ) );

			if (!instancingSupported)
			{
				// When instancing is not supported by GPU, implement alternative
				// code path that doesn't use instancing.
				bool blink = uint32_t(time*3.0f)&1;
				bgfx::dbgTextPrintf(0, 0, blink ? 0x4f : 0x04, " Instancing is not supported by GPU. ");

				m_useInstancing = false;
			}

			const bx::Vec3 at  = { 0.0f, 0.0f,   0.0f };
			const bx::Vec3 eye = { 0.0f, 0.0f, -35.0f };

			// Set view and projection matrix for view 0.
			{
				float view[16];
				bx::mtxLookAt(view, eye, at);

				float proj[16];
				bx::mtxProj(proj, 60.0f, float(m_width)/float(m_height), 0.1f, 100.0f, bgfx::getCaps()->homogeneousDepth);
				bgfx::setViewTransform(0, view, proj);

				// Set view 0 default viewport.
				bgfx::setViewRect(0, 0, 0, uint16_t(m_width), uint16_t(m_height) );
			}

			m_lastFrameMissing = 0;

			if (m_useInstancing)
			{
				// 80 bytes stride = 64 bytes for 4x4 matrix + 16 bytes for RGBA color.
				const uint16_t instanceStride = 80;
				// to total number of instances to draw
				uint32_t totalCubes = m_sideSize * m_sideSize;

				// figure out how big of a buffer is available
				uint32_t drawnCubes = bgfx::getAvailInstanceDataBuffer(totalCubes, instanceStride);

				// save how many we couldn't draw due to buffer room so we can display it
				m_lastFrameMissing = totalCubes - drawnCubes;

				bgfx::InstanceDataBuffer idb;
				bgfx::allocInstanceDataBuffer(&idb, drawnCubes, instanceStride);

				// uint8_t* data = idb.data;

				// for (uint32_t ii = 0; ii < drawnCubes; ++ii)
				// {
				// 	uint32_t yy = ii / m_sideSize;
				// 	uint32_t xx = ii % m_sideSize;

				// 	float* mtx = (float*)data;
				// 	// bx::mtxRotateXY(mtx, time + xx * 0.21f, time + yy * 0.37f);
				// 	// mtx[12] = -15.0f + float(xx) * 3.0f;
				// 	// mtx[13] = -15.0f + float(yy) * 3.0f;
				// 	// mtx[14] = 0.0f;

				// 	float* color = (float*)&data[64];
				// 	color[0] = bx::sin(time + float(xx) / 11.0f) * 0.5f + 0.5f;
				// 	color[1] = bx::cos(time + float(yy) / 11.0f) * 0.5f + 0.5f;
				// 	color[2] = bx::sin(time * 3.0f) * 0.5f + 0.5f;
				// 	color[3] = 1.0f;

				// 	data += instanceStride;
				// }

				// Set vertex and index buffer.
				bgfx::setVertexBuffer(0, m_vbh);
				bgfx::setIndexBuffer(m_ibh);

				// Set instance data buffer.
				bgfx::setInstanceDataBuffer(&idb);

				// Set render states.
				bgfx::setState(BGFX_STATE_DEFAULT);

				// Submit primitive for rendering to view 0.
				bgfx::submit(0, m_program);
			}
			else
			{
				// non-instanced path
				for (uint32_t yy = 0; yy < m_sideSize; ++yy)
				{
					for (uint32_t xx = 0; xx < m_sideSize; ++xx)
					{
						float mtx[16];
						bx::mtxRotateXY(mtx, time + xx * 0.21f, time + yy * 0.37f);
						mtx[12] = -15.0f + float(xx) * 3.0f;
						mtx[13] = -15.0f + float(yy) * 3.0f;
						mtx[14] = 0.0f;

						// Set model matrix for rendering.
						bgfx::setTransform(mtx);

						// Set vertex and index buffer.
						bgfx::setVertexBuffer(0, m_vbh);
						bgfx::setIndexBuffer(m_ibh);

						// Set render states.
						bgfx::setState(BGFX_STATE_DEFAULT);

						// Submit primitive for rendering to view 0.
						bgfx::submit(0, m_program_non_instanced);
					}
				}
			}

			// Advance to next frame. Rendering thread will be kicked to
			// process submitted rendering primitives.
			bgfx::frame();

			return true;
		}

		return false;
	}

	Network net;
	entry::MouseState m_mouseState;

	uint32_t m_width;
	uint32_t m_height;
	uint32_t m_debug;
	uint32_t m_reset;
	bool     m_useInstancing;
	uint32_t m_lastFrameMissing;
	uint32_t m_sideSize;

	bgfx::VertexBufferHandle m_vbh;
	bgfx::IndexBufferHandle  m_ibh;
	bgfx::ProgramHandle m_program;
	bgfx::ProgramHandle m_program_non_instanced;

	int64_t m_timeOffset;
};

} // namespace

int32_t _main_(int32_t argc, char** argv)
{
    ExampleHelloWorld app(10);
    return entry::runApp(&app, argc, argv);
}
