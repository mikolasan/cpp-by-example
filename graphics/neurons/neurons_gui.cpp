#include <iostream>
#include <fstream>

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

#include "neuron.hpp"
#include "network.hpp"
#include "render/neuron_render.hpp"
#include "render/network_render.hpp"


namespace
{

	struct MnistImage {
		uint8_t label;
		uint8_t pixels[784];
	};

	std::vector<MnistImage> load_mnist_bin(const std::string& path) {
		std::ifstream in(path, std::ios::binary);
		std::vector<MnistImage> images;
		MnistImage img;

		while (in.read(reinterpret_cast<char*>(&img), sizeof(MnistImage))) {
			images.push_back(img);
		}
		return images;
	}

	bgfx::TextureHandle create_mnist_texture(const uint8_t* pixels) {
		const uint32_t width = 28, height = 28;
		std::vector<uint8_t> rgba(width * height * 4);

		for (int i = 0; i < 784; ++i) {
			uint8_t v = pixels[i];
			rgba[i * 4 + 0] = v;
			rgba[i * 4 + 1] = v;
			rgba[i * 4 + 2] = v;
			rgba[i * 4 + 3] = 255;
		}

		const bgfx::Memory* mem = bgfx::copy(rgba.data(), rgba.size());
		return bgfx::createTexture2D(width, height, false, 1, bgfx::TextureFormat::RGBA8, 0, mem);
	}

	class ExampleHelloWorld : public entry::AppI
	{
	public:
		ExampleHelloWorld(int n_neurons)
			: entry::AppI("NEUF", "spiking network simulator", "")
		{
			net.setSize(n_neurons);
			auto ctx = std::make_shared<NetworkVisualContext>(net);
			net.render = std::make_shared<NetworkRenderStrategy>(ctx);

			const float offset = 3.0f;
			for (size_t i = 0; i < net.neurons.size(); ++i) {
				float angle = i * 2 * bx::kPi / net.neurons.size();
				auto ctx2 = std::make_shared<NeuronVisualContext>(net.neurons[i]);
				ctx2->positions.emplace_back(
					cos(angle) * offset,
					sin(angle) * offset,
					0.0f);
				net.neurons[i].render = std::make_shared<NeuronRenderStrategy>(ctx2);
			}

		}

		void init(int32_t _argc, const char* const* _argv, uint32_t _width, uint32_t _height) override
		{
			Args args(_argc, _argv);

			images = load_mnist_bin("mnist.bin");

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

			net.init();


			m_timeOffset = bx::getHPCounter();

			imguiCreate();
		}

		int shutdown() override
		{
			imguiDestroy();

			net.destroy();

			// Shutdown bgfx.
			bgfx::shutdown();

			return 0;
		}

		bool update() override
		{
			if (!entry::processEvents(m_width, m_height, m_debug, m_reset, &m_mouseState))
			{
				imguiBeginFrame(m_mouseState.m_mx
					, m_mouseState.m_my
					, (m_mouseState.m_buttons[entry::MouseButton::Left] ? IMGUI_MBUT_LEFT : 0)
					| (m_mouseState.m_buttons[entry::MouseButton::Right] ? IMGUI_MBUT_RIGHT : 0)
					| (m_mouseState.m_buttons[entry::MouseButton::Middle] ? IMGUI_MBUT_MIDDLE : 0)
					, m_mouseState.m_mz
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

				static int current_image = 0;
				ImGui::SliderInt("Image Index", &current_image, 0, images.size() - 1);
				ImGui::Text("Label: %d", images[current_image].label);

				// Create/update texture
				static bgfx::TextureHandle texture = BGFX_INVALID_HANDLE;
				if (!bgfx::isValid(texture)) {
					texture = create_mnist_texture(images[current_image].pixels);
				}
				else {
					bgfx::destroy(texture);
					texture = create_mnist_texture(images[current_image].pixels);
				}

				// Show image
				ImTextureID tex_id = (ImTextureID)(uintptr_t)texture.idx;
				ImGui::Image(tex_id, ImVec2(280, 280));

				// Get renderer capabilities info.
				const bgfx::Caps* caps = bgfx::getCaps();

				// Check if instancing is supported.
				const bool instancingSupported = 0 != (BGFX_CAPS_INSTANCING & caps->supported);

				float time = (float)((bx::getHPCounter() - m_timeOffset) / double(bx::getHPFrequency()));

				ImGui::Text("Time %.2f", time);

				ImGui::PushEnabled(instancingSupported);
				// ImGui::Checkbox("Use Instancing", &m_useInstancing);
				ImGui::PopEnabled();

				// ImGui::Text("Grid Side Size:");
				// ImGui::SliderInt("##size", (int*)&m_sideSize, 1, 512);

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
				bgfx::setViewRect(0, 0, 0, uint16_t(m_width), uint16_t(m_height));

				// This dummy draw call is here to make sure that view 0 is cleared
				// if no other draw calls are submitted to view 0.
				bgfx::touch(0);



				// if (!instancingSupported)
				// {
				// 	// When instancing is not supported by GPU, implement alternative
				// 	// code path that doesn't use instancing.
				// 	bool blink = uint32_t(time * 3.0f) & 1;
				// 	bgfx::dbgTextPrintf(0, 0, blink ? 0x4f : 0x04, " Instancing is not supported by GPU. ");

				// }

				const bx::Vec3 at = { 0.0f, 0.0f,   0.0f };
				const bx::Vec3 eye = { 0.0f, 0.0f, -35.0f };

				// Set view and projection matrix for view 0.
				{
					float view[16];
					bx::mtxLookAt(view, eye, at);

					float proj[16];
					bx::mtxProj(proj, 60.0f, float(m_width) / float(m_height), 0.1f, 100.0f, bgfx::getCaps()->homogeneousDepth);
					bgfx::setViewTransform(0, view, proj);

					// Set view 0 default viewport.
					bgfx::setViewRect(0, 0, 0, uint16_t(m_width), uint16_t(m_height));
				}

				m_lastFrameMissing = 0;

				net.step();
				net.update(time);
				net.draw(time);

				// Advance to next frame. Rendering thread will be kicked to
				// process submitted rendering primitives.
				bgfx::frame();

				return true;
			}

			return false;
		}

		std::vector<MnistImage> images;

		Network net;
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
	};

} // namespace

int32_t _main_(int32_t argc, char** argv)
{
	ExampleHelloWorld app(10);
	return entry::runApp(&app, argc, argv);
}
