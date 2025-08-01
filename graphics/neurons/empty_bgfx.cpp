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

#include "camera.cpp"
#include "mouse.cpp"
#include "settings.cpp"

// TODO:
// ImGui different look
// https://github.com/GraphicsProgramming/dear-imgui-styles

namespace
{


	class BrainViz : public entry::AppI
	{
	public:
		BrainViz(int n_neurons)
			: entry::AppI("NEUF", "spiking network simulator", "")
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


			m_timeOffset = bx::getHPCounter();

			imguiCreate();
		}

		int shutdown() override
		{
			imguiDestroy();

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

				// Get renderer capabilities info.
				const bgfx::Caps* caps = bgfx::getCaps();

				// Check if instancing is supported.
				const bool instancingSupported = 0 != (BGFX_CAPS_INSTANCING & caps->supported);

				int64_t now = bx::getHPCounter();
				static int64_t last = now;
				const int64_t frameTime = now - last;
				last = now;
				const double freq = double(bx::getHPFrequency());
				float time = (float)((now - m_timeOffset) / freq);
				const float deltaTimeSec = float(double(frameTime)/freq);

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
							m_settings.m_envRotDest += m_mouse.m_dx*2.0f;
						}
						else if (0 != m_mouse.m_scroll)
						{
							m_camera.dolly(float(m_mouse.m_scroll)*0.05f);
						}
					}
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
					bx::mtxProj(proj, 60.0f, float(m_width)/float(m_height), 0.1f, 100.0f, caps->homogeneousDepth);
					bgfx::setViewTransform(0, view, proj);

					// View rect.
					bgfx::setViewRect(0, 0, 0, uint16_t(m_width), uint16_t(m_height) );
					// bgfx::setViewRect(1, 0, 0, uint16_t(m_width), uint16_t(m_height) );

					// Env rotation.
					const float amount = bx::min(deltaTimeSec/0.12f, 1.0f);
					m_settings.m_envRotCurr = bx::lerp(m_settings.m_envRotCurr, m_settings.m_envRotDest, amount);

					// Env mtx.
					float mtxEnvView[16];
					m_camera.envViewMtx(mtxEnvView);
					float mtxEnvRot[16];
					bx::mtxRotateY(mtxEnvRot, m_settings.m_envRotCurr);

				}

				m_lastFrameMissing = 0;

				// Advance to next frame. Rendering thread will be kicked to
				// process submitted rendering primitives.
				bgfx::frame();

				return true;
			}

			return false;
		}

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

		Camera m_camera;
		Mouse m_mouse;
		Settings m_settings;

	};

} // namespace

int32_t _main_(int32_t argc, char** argv)
{
	BrainViz app(10);
	return entry::runApp(&app, argc, argv);
}
