/*
 * Copyright 2024 Pavle Mihajlovic. All rights reserved.
 */

#include "render.h"
#include "game_state.h"

#include "input.h"
#include "GLFW/glfw3.h"
#include "render_state.h"
#include "logo.h"
#include "bx/os.h"
#include "bx/math.h"
#include "bx/timer.h"
#include "resources.h"
#include "imgui/imgui.h"
#include <bgfx/platform.h>


#define RENDER_TIMES_BUFFER_SIZE 30
extern bx::SpScUnboundedQueue s_systemEventsRender;
extern std::atomic<f64> g_averageMainFrameTime;
f32 frame_times[RENDER_TIMES_BUFFER_SIZE];
i32 frameTimesIndex = 0;
i64 lastFrameCounter = 0;

void drawCube(float x, float y, float z) {

	const i32 fieldSize = 3;
	float mtx[16];
	bx::mtxTranslate(mtx, x, y, z);

	bgfx::setTransform(mtx);
	// Set vertex and index buffer.
	bgfx::setVertexBuffer(0, g_resources.cube_vbh);
	bgfx::setIndexBuffer(g_resources.cube_ibh);

	u64 state = BGFX_STATE_WRITE_R
		| BGFX_STATE_WRITE_G
		| BGFX_STATE_WRITE_B
		| BGFX_STATE_WRITE_A
		| BGFX_STATE_WRITE_Z
		| BGFX_STATE_DEPTH_TEST_LESS
		| BGFX_STATE_CULL_CCW
		| BGFX_STATE_MSAA
		| 0;// BGFX_STATE_PT_TRISTRIP;

	// Set render states.
	bgfx::setState(state);

	bgfx::submit(0, g_resources.vertexColorProgram);

}

void showStatWindow() {
	ImGui::Begin("Stats");
	ImGui::Text("Test");
	ImGui::End();
}

i32 runRenderThread(bx::Thread *self, void *userData)
{
	bgfx::renderFrame();

	auto args = (RenderThreadArgs *)userData;
	// Initialize bgfx using the native window handle and window resolution.
	bgfx::Init init;
	init.platformData = args->platformData;
	init.resolution.width = args->width;
	init.resolution.height = args->height;
	init.resolution.reset = BGFX_RESET_VSYNC;
	if (!bgfx::init(init))
		return 1;
	bgfx::setDebug(BGFX_DEBUG_TEXT);

	loadResources();

	imguiCreate();

	// Set view 0 to the same dimensions as the window and to clear the color buffer.
	const bgfx::ViewId kClearView = 0;
	bgfx::setViewClear(kClearView, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x302030ff);
	bgfx::setViewRect(kClearView, 0, 0, bgfx::BackbufferRatio::Equal);
	uint32_t width = args->width;
	uint32_t height = args->height;
	bool exit = false;
	while (!exit) {
		// Handle events from the main thread.
		while (auto ev = (EventType *)s_systemEventsRender.pop()) {
			
			if (*ev == EventType::Resize) {
				auto resizeEvent = (ResizeEvent *)ev;
				bgfx::reset(resizeEvent->width, resizeEvent->height, BGFX_RESET_VSYNC);
				bgfx::setViewRect(kClearView, 0, 0, bgfx::BackbufferRatio::Equal);
				width = resizeEvent->width;
				height = resizeEvent->height;
			}
			if (*ev == EventType::Exit) {
				exit = true;
			}
			delete ev;
		}

		bool found			 = false;
		i32 renderStateIndex = 0;

		while (!found) {
			i64 newestTime = -1;

			for (i32 i = 0; i < NUM_GAME_STATES; i++) {
				if (g_beingUpdated.load() != i) {
					if (g_gameStates[i].timeGenerated > newestTime) {
						renderStateIndex = i;
						newestTime = g_gameStates[i].timeGenerated;
					}
				}
			}

			found = true;
			g_beingRendered.store(renderStateIndex);
		}

		const GameState& render_state = g_gameStates[renderStateIndex];

		f64 cursorPos[2] = {};
		glfwGetCursorPos(args->window, &cursorPos[0], &cursorPos[1]);
		i32 leftMouseButton = glfwGetMouseButton(args->window, GLFW_MOUSE_BUTTON_LEFT);
		i32 middleMouseButton = glfwGetMouseButton(args->window, GLFW_MOUSE_BUTTON_MIDDLE);
		i32 rightMouseButton = glfwGetMouseButton(args->window, GLFW_MOUSE_BUTTON_RIGHT);

		imguiBeginFrame(cursorPos[0]//m_mouseState.m_mx
			, cursorPos[1]//m_mouseState.m_my
			, (leftMouseButton ? IMGUI_MBUT_LEFT : 0)
			| (rightMouseButton ? IMGUI_MBUT_RIGHT : 0)
			| (middleMouseButton ? IMGUI_MBUT_MIDDLE : 0)
			, 0//m_mouseState.m_mz
			, uint16_t(width)
			, uint16_t(height)
		);

		ImGui::Begin("Stats");
		ImGui::Text("Logic thread: %.1lf, %.1lf", render_state.logicThreadDeltaAverage*1000.0, 1.0 / render_state.logicThreadDeltaAverage);

		f64 averageTime = 0.0f;
		for (int i = 0; i < RENDER_TIMES_BUFFER_SIZE; i++) {
			averageTime += frame_times[i];
		}
		averageTime /= (f64)RENDER_TIMES_BUFFER_SIZE;

		ImGui::Text("Render thread: %.1lf, %.1lf", averageTime*1000.0, 1.0 / averageTime);

		f64 mainThreadTime = g_averageMainFrameTime.load();
		ImGui::Text("Main thread: %.1lf, %.1lf", mainThreadTime*1000.0, 1.0 / mainThreadTime);

		ImGui::End();
		imguiEndFrame();

		Camera camera = render_state.camera;
		camera.vertical_FOV = 60.f;
		camera.aspect_ratio = (f32)width / (f32)height;
		camera.homogenous_depth = bgfx::getCaps()->homogeneousDepth;

		mat4 view;
		camera.getViewMat(&view);

		mat4 proj;
		camera.getProjMat(&proj);

		bgfx::setViewTransform(0, &view[0], &proj[0]);
		bgfx::setViewRect(0, 0, 0, uint16_t(width), uint16_t(height));

		drawCube(0.0f, 0.0f, 0.0f);
		drawCube(0.0f, 5.0f, 0.0f);
		drawCube(5.0f, 0.0f, 0.0f);
		drawCube(0.0f, 0.0f, 5.0f);

		float mtx[16];
		bx::mtxTranslate(mtx, 5.0f, 5.0f, 5.0f);

		g_resources.teapot.submit(0, g_resources.meshProgram, mtx);

		bx::mtxTranslate(mtx, 0.0f, 0.0f, 0.0f);
		//g_resources.sponza.submit(0, g_resources.meshProgram, mtx);

		// Create view matrix
		/*mat4 glmview = createViewMatrix(renderState.cameraYaw, renderState.cameraPitch, renderState.cameraPos);

		// Create projection matrix
		mat4 glmproj = createProjectionMatrix(60.0f, aspectRatio, 0.1f, 100.0f, bgfx::getCaps()->homogeneousDepth);
		
		bgfx::setViewTransform(0, glm::value_ptr(glmview), glm::value_ptr(glmproj));
		*/
		

		// Use debug font to print information about this example.
		/*bgfx::dbgTextClear();
		//bgfx::dbgTextImage(bx::max<uint16_t>(uint16_t(width / 2 / 8), 20) - 20, bx::max<uint16_t>(uint16_t(height / 2 / 16), 6) - 6, 40, 12, s_logo, 160);
		bgfx::dbgTextPrintf(0, 0, 0x0f, "Press F1 to toggle stats.");
		bgfx::dbgTextPrintf(0, 1, 0x0f, "Color can be changed with ANSI \x1b[9;me\x1b[10;ms\x1b[11;mc\x1b[12;ma\x1b[13;mp\x1b[14;me\x1b[0m code too.");
		bgfx::dbgTextPrintf(80, 1, 0x0f, "\x1b[;0m    \x1b[;1m    \x1b[; 2m    \x1b[; 3m    \x1b[; 4m    \x1b[; 5m    \x1b[; 6m    \x1b[; 7m    \x1b[0m");
		bgfx::dbgTextPrintf(80, 2, 0x0f, "\x1b[;8m    \x1b[;9m    \x1b[;10m    \x1b[;11m    \x1b[;12m    \x1b[;13m    \x1b[;14m    \x1b[;15m    \x1b[0m");
		const bgfx::Stats* stats = bgfx::getStats();
		bgfx::dbgTextPrintf(0, 2, 0x0f, "Backbuffer %dW x %dH in pixels, debug text %dW x %dH in characters.", stats->width, stats->height, stats->textWidth, stats->textHeight);
		bgfx::dbgTextPrintf(0, 3, 0x0f, "Dummy value is %f.", renderState.dummy);

		bgfx::setDebug(renderState.showStats ? BGFX_DEBUG_STATS : BGFX_DEBUG_TEXT);
		*/
		// Enable stats or debug text.

		// Advance to next frame. Main thread will be kicked to process submitted rendering primitives.
		//bx::sleep(50);

		i64 currentCounter = bx::getHPCounter();
		frame_times[frameTimesIndex] = (f64)(currentCounter - lastFrameCounter)/(f64)bx::getHPFrequency();
		frameTimesIndex = (frameTimesIndex + 1) % RENDER_TIMES_BUFFER_SIZE;
		lastFrameCounter = currentCounter;

		g_beingRendered.store(-1);
		bgfx::frame();

		// Wait for the API thread to call bgfx::frame, then process submitted rendering primitives.
		//bgfx::renderFrame();

	}
	bgfx::shutdown();
	return 0;
}