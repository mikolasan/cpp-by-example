#include <iostream>
#include <filesystem>
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
#include "network.h"
#include "render/neuron_render.hpp"
#include "render/network_render.h"
#include "simulation_clock.hpp"
#include "mnist_data_processor.h"

#include "render/pos_color_vertex.h"
#include "camera.cpp"
#include "mouse.cpp"
#include "settings.cpp"

// TODO:
// ImGui different look
// https://github.com/GraphicsProgramming/dear-imgui-styles

namespace
{

	enum SimViewId : uint8_t
	{
		kMainView  = 1,
	};

	class SpikingWorld : public entry::AppI
	{
	public:
		SpikingWorld(int n_neurons)
			: entry::AppI("NEUF", "spiking network simulator", "")
		{
		}

		void init(int32_t _argc, const char* const* _argv, uint32_t _width, uint32_t _height) override
		{
			Args args(_argc, _argv);

			PosColorVertex::init();
			data.init();			

			m_width = _width;
			m_height = _height;
			leftPanelWidth = m_width / 4.0;

			m_debug = BGFX_DEBUG_NONE;
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

			// bgfx::setViewClear(kClearView, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0xff0000ff);
			bgfx::setViewClear(kMainView, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0xffff00ff);

			m_timeOffset = bx::getHPCounter();

			imguiCreate();


			auto ctx = std::make_shared<NetworkVisualContext>(net);
			net.render = std::make_shared<NetworkRenderStrategy>(ctx);


			bool loadReceptors = false;
			if (loadReceptors) {
				const auto area_size = data.get_area_size();
				net.addLayer(data.prepare_neurons(), area_size);
			}

			// for (size_t i = 0; i < net.neurons.size(); ++i) {
			// 	auto ctx2 = std::make_shared<NeuronVisualContext>(net.neurons[i]);
			// 	ctx2->position = {
			// 		float(i % int32_t(area_size.x)),
			// 		i / area_size.x,
			// 		0.0f };
			// 	net.neurons[i].render = std::make_shared<NeuronRenderStrategy>(ctx2);
			// }

			sim_clock.set_dt(10);
			sim_clock.pause();
			net.init(kMainView);
		}

		void ResetSimulation() {
			sim_clock.reset();
		}

		int shutdown() override
		{
			imguiDestroy();

			net.destroy();

			// Shutdown bgfx.
			bgfx::shutdown();

			return 0;
		}

		void drawCameraStats() {
			// if (ImGui::IsKeyPressed(ImGuiKey_C)) {
				// ImGui::Begin("Camera Stats", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

				ImGui::Text("Target (curr): %.2f %.2f %.2f", m_camera.m_target.curr.x, m_camera.m_target.curr.y, m_camera.m_target.curr.z);
				ImGui::Text("Target (dest): %.2f %.2f %.2f", m_camera.m_target.dest.x, m_camera.m_target.dest.y, m_camera.m_target.dest.z);

				ImGui::Separator();

				ImGui::Text("Position (curr): %.2f %.2f %.2f", m_camera.m_pos.curr.x, m_camera.m_pos.curr.y, m_camera.m_pos.curr.z);
				ImGui::Text("Position (dest): %.2f %.2f %.2f", m_camera.m_pos.dest.x, m_camera.m_pos.dest.y, m_camera.m_pos.dest.z);

				ImGui::Separator();

				ImGui::Text("Orbit: [%.2f, %.2f]", m_camera.m_orbit[0], m_camera.m_orbit[1]);

				// ImGui::End();
			// }
		}

		bool update() override
		{
			bool exit_flag = entry::processEvents(m_width, m_height, m_debug, m_reset, &m_mouseState);
			if (exit_flag) return false;



			int64_t now = bx::getHPCounter();
			static int64_t last = now;
			const int64_t frameTime = now - last;
			last = now;
			const double freq = double(bx::getHPFrequency());
			float time = (float)((now - m_timeOffset) / freq);
			const float deltaTimeSec = float(double(frameTime) / freq);

			static int current_image = 0;
			data.set_current_id(current_image);
			sim_clock.update(deltaTimeSec);
			
			if (sim_clock.advance()) {
				net.step(data.convert_current_to_inputs());
				std::vector<float> voltage_state = net.get_current_voltage_state();
				float t = sim_clock.store(voltage_state);
			}


			// float matched_time;
			// if (sim_clock.has_seen_state(neuron_state, matched_time)) {
			// 	std::cout << "State repeated from time: " << matched_time << " ms\n";
			// }

			


			// kClearView covering the whole screen
			// bgfx::setViewRect(kClearView, 0, 0, m_width, m_height);

			imguiBeginFrame(m_mouseState.m_mx, m_mouseState.m_my,
				(m_mouseState.m_buttons[entry::MouseButton::Left] ? IMGUI_MBUT_LEFT : 0) |
				(m_mouseState.m_buttons[entry::MouseButton::Right] ? IMGUI_MBUT_RIGHT : 0) |
				(m_mouseState.m_buttons[entry::MouseButton::Middle] ? IMGUI_MBUT_MIDDLE : 0),
				m_mouseState.m_mz,
				m_width,
				m_height

			);

			
			ImGui::SetNextWindowPos(ImVec2(0, 0));
			ImGui::SetNextWindowSize(ImVec2(leftPanelWidth, 0));

			ImGui::Begin("Controls",
					nullptr,
					ImGuiWindowFlags_NoTitleBar |
					ImGuiWindowFlags_NoMove     |
					ImGuiWindowFlags_NoCollapse);
			
			// ImGui::BeginChild("LeftPanel", ImVec2(leftPanelWidth, 0), ImGuiChildFlags_None, 
					// ImGuiWindowFlags_NoCollapse);

			// ImGui::SetNextWindowPos(
			// 	ImVec2(m_width - m_width / 3.0f - 10.0f, 10.0f)
			// 	, ImGuiCond_FirstUseEver
			// );
			// ImGui::SetNextWindowSize(
			// 	ImVec2(m_width / 3.0f, m_height / 1.2f)
			// 	, ImGuiCond_FirstUseEver
			// );
			// ImGui::Begin("Settings"
			// 	, NULL
			// 	, 0
			// );

			// Simulation time display
			// ImGui::Text("Sim Time: %.2f ms", sim_time_ms);

			// Control buttons
			if (ImGui::Button("Play")) {
				sim_clock.play();
				// sim_state = SimState::Playing;
			}
			ImGui::SameLine();
			if (ImGui::Button("Pause")) {
				sim_clock.pause();
				// sim_state = SimState::Paused;
			}
			ImGui::SameLine();
			if (ImGui::Button("Step")) {
				sim_clock.step();
				// sim_state = SimState::StepOnce;
			}
			ImGui::SameLine();
			if (ImGui::Button("Reset")) { ResetSimulation(); }

			// Step duration (dt)
			static int dt_index = 1;
			const int dt_values[] = { 1, 10, 100, 1000 };
			if (ImGui::Combo("dt (ms/step)", &dt_index, "1\0 10\0 100\0 1000\0")) {
				dt = dt_values[dt_index];
			}

			// Real-time delay between steps
			static int delay_index = 1;
			const int delay_values[] = { 250, 500, 1000, 5000 };
			if (ImGui::Combo("Delay between steps", &delay_index, "250ms\0 500ms\0 1s\0 5s\0")) {
				real_time_delay_ms = delay_values[delay_index];
			}

			// Camera stats
			drawCameraStats();

			
			ImGui::SliderInt("Image Index", &current_image, 0, data.get_max_id());
			ImGui::Text("Label: %d", data.get_current_label());

			bgfx::TextureHandle texture = data.create_currnet_texture();

			// Show image
			ImTextureID tex_id = (ImTextureID)(uintptr_t)texture.idx;
			ImGui::Image(tex_id, ImVec2(280, 280));

			// Get renderer capabilities info.
			const bgfx::Caps* caps = bgfx::getCaps();

			// Check if instancing is supported.
			const bool instancingSupported = 0 != (BGFX_CAPS_INSTANCING & caps->supported);

			

			ImGui::Text("Lats sim time %d", sim_clock.last_time_ms());
			// ImGui::Text("dt %.5f", deltaTimeSec);

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



			uint32_t mainViewWidth = m_width - leftPanelWidth;

			
			// {
			// 	// Set view 0 default viewport.
			// 	bgfx::setViewRect(0, 0, 0, uint16_t(m_width), uint16_t(m_height));

			// 	// This dummy draw call is here to make sure that view 0 is cleared
			// 	// if no other draw calls are submitted to view 0.
			// 	bgfx::touch(0);

			// 	const bx::Vec3 at = { 0.0f, 0.0f,   0.0f };
			// 	const bx::Vec3 eye = { 0.0f, 0.0f, -35.0f };

			// 	// Set view and projection matrix for view 0.
			// 	{
			// 		float view[16];
			// 		bx::mtxLookAt(view, eye, at);

			// 		float proj[16];
			// 		bx::mtxProj(
			// 			proj, 
			// 			60.0f, 
			// 			float(m_width) / float(m_height), 
			// 			0.1f, 
			// 			100.0f, 
			// 			bgfx::getCaps()->homogeneousDepth);
			// 		bgfx::setViewTransform(0, view, proj);

			// 		// Set view 0 default viewport.
			// 		bgfx::setViewRect(0, 0, 0, uint16_t(m_width), uint16_t(m_height));
			// 	}
			// }

			{
				const bool mouseOverGui = ImGui::MouseOverArea();
				m_mouse.update(float(m_mouseState.m_mx), float(m_mouseState.m_my), m_mouseState.m_mz, m_width, m_height);
				if (!mouseOverGui)
				{
					if (m_mouseState.m_buttons[entry::MouseButton::Left])
					{
						m_camera.orbit(m_mouse.m_dx, m_mouse.m_dy);
					}
					else if (m_mouseState.m_buttons[entry::MouseButton::Right])
					{
						m_camera.dolly(m_mouse.m_dx + m_mouse.m_dy);
					}
					else if (m_mouseState.m_buttons[entry::MouseButton::Middle])
					{
						m_settings.m_envRotDest += m_mouse.m_dx * 2.0f;
					}
					else if (0 != m_mouse.m_scroll)
					{
						m_camera.dolly(float(m_mouse.m_scroll) * 0.05f);
					}
				}

				const float camera_step = 3.0f;
				if (ImGui::IsKeyPressed(ImGuiKey_RightArrow))
						m_camera.m_target.dest.x +=  camera_step;
				if (ImGui::IsKeyPressed(ImGuiKey_LeftArrow))
						m_camera.m_target.dest.x -=  camera_step;
				
				if (ImGui::IsKeyPressed(ImGuiKey_UpArrow))
						m_camera.m_target.dest.z +=  camera_step;
				if (ImGui::IsKeyPressed(ImGuiKey_DownArrow))
						m_camera.m_target.dest.z -=  camera_step;

				m_camera.update(deltaTimeSec);
				// bx::memCopy(m_uniforms.m_cameraPos, &m_camera.m_pos.curr.x, 3*sizeof(float) );

				// View Transform 0.
				float view[16];
				// bx::mtxIdentity(view);

				float proj[16];
				// bx::mtxOrtho(proj, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 100.0f, 0.0, caps->homogeneousDepth);
				// bgfx::setViewTransform(0, view, proj);

				// View Transform 1.
				m_camera.mtxLookAt(view);
				bx::mtxProj(proj, 60.0f, float(mainViewWidth) / float(m_height), 0.1f, 100.0f, caps->homogeneousDepth);
				bgfx::setViewTransform(kMainView, view, proj);

				// View rect.
				// bgfx::setViewRect(0, 0, 0, uint16_t(m_width), uint16_t(m_height));
				// bgfx::setViewRect(1, 0, 0, uint16_t(m_width), uint16_t(m_height) );

				// Env rotation.
				// const float amount = bx::min(deltaTimeSec / 0.12f, 1.0f);
				// m_settings.m_envRotCurr = bx::lerp(m_settings.m_envRotCurr, m_settings.m_envRotDest, amount);

				// Env mtx.
				// float mtxEnvView[16];
				// m_camera.envViewMtx(mtxEnvView);
				// float mtxEnvRot[16];
				// bx::mtxRotateY(mtxEnvRot, m_settings.m_envRotCurr);

			}


			showExampleDialog(this);

			ImGui::End();


			float minPanelWidth = 100.0f;
			float maxPanelWidth = ImGui::GetIO().DisplaySize.x - 100.0f;

			// Draw splitter handle on the right edge of the panel
			ImGui::SetCursorScreenPos(ImVec2(leftPanelWidth - 10, 0)); // small overlap
			ImGui::InvisibleButton("vsplitter", ImVec2(20, ImGui::GetIO().DisplaySize.y));
			if (ImGui::IsItemActive()) {
					leftPanelWidth += ImGui::GetIO().MouseDelta.x;
					leftPanelWidth =  std::max(minPanelWidth, std::min(leftPanelWidth, maxPanelWidth));
			}

			// Optional: draw a visible line for feedback
			ImDrawList* draw_list = ImGui::GetForegroundDrawList();
			ImVec2 splitterMin(leftPanelWidth, 0.0f);
			ImVec2 splitterMax(leftPanelWidth, ImGui::GetIO().DisplaySize.y);
			draw_list->AddLine(splitterMin, splitterMax,
												ImGui::GetColorU32(ImGuiCol_Separator), 5.0f);

			imguiEndFrame();

			
			bgfx::setViewRect(kMainView, leftPanelWidth, 0, mainViewWidth, m_height);
			net.update(time);
			net.draw(time);

			m_lastFrameMissing = 0;

			
			// Advance to next frame. Rendering thread will be kicked to
			// process submitted rendering primitives.
			bgfx::frame();

			return true;
		}

		MnistDataProcessor data;

		Network net;
		entry::MouseState m_mouseState;

		SimulationClock sim_clock;
		// SimState sim_state;
		uint64_t sim_time_ms;
		uint32_t dt;
		uint32_t real_time_delay_ms;

		uint32_t m_width;
		uint32_t m_height;
		float leftPanelWidth;

		uint32_t m_debug;
		uint32_t m_reset;
		uint32_t m_lastFrameMissing;


		bgfx::VertexBufferHandle m_vbh;
		bgfx::IndexBufferHandle m_ibh;
		bgfx::ProgramHandle m_program;

		int64_t m_timeOffset;

		Camera m_camera;
		Mouse m_mouse;
		Settings m_settings;

	};

} // namespace

int32_t _main_(int32_t argc, char** argv)
{
	std::cout << "Running from: "
		<< std::filesystem::current_path()
		<< std::endl;

	SpikingWorld app(10);
	return entry::runApp(&app, argc, argv);
}
