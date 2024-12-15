/*
 * Copyright 2011-2019 Branimir Karadzic. All rights reserved.
 * License: https://github.com/bkaradzic/bgfx#license-bsd-2-clause
 */
#include <stdio.h>
#include <atomic>
#include "types.h"
#include <bx/bx.h>
#include <bx/spscqueue.h>
#include <bx/thread.h>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <GLFW/glfw3.h>

#if BX_PLATFORM_LINUX
#define GLFW_EXPOSE_NATIVE_X11
#elif BX_PLATFORM_WINDOWS
#define GLFW_EXPOSE_NATIVE_WIN32
#elif BX_PLATFORM_OSX
#define GLFW_EXPOSE_NATIVE_COCOA
#endif
#include <GLFW/glfw3native.h>

#include "logo.h"
#include "render.h"
#include "logic.h"
#include "input.h"
#include "render_state.h"
#include "game_state.h"
#include "resources.h"

bx::AllocatorI* getDefaultAllocator()
{
	BX_PRAGMA_DIAGNOSTIC_PUSH();
	BX_PRAGMA_DIAGNOSTIC_IGNORED_MSVC(4459); // warning C4459: declaration of 's_allocator' hides global declaration
	BX_PRAGMA_DIAGNOSTIC_IGNORED_CLANG_GCC("-Wshadow");
	static bx::DefaultAllocator s_allocator;
	return &s_allocator;
	BX_PRAGMA_DIAGNOSTIC_POP();
}

std::atomic<u8> s_keyMap[GLFW_KEY_LAST];
bx::SpScUnboundedQueue s_systemEvents(getDefaultAllocator());
bx::SpScUnboundedQueue s_keyEvents(getDefaultAllocator());
RenderState g_renderState[NUM_RENDER_STATES];
GameState g_gameState;
Resources g_resources;

static void glfw_errorCallback(int error, const char *description)
{
	fprintf(stderr, "GLFW error %d: %s\n", error, description);
}

static void glfw_keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	auto keyEvent = new KeyEvent;
	keyEvent->key = key;
	keyEvent->action = action;

	if (key < 0)//some system key, volume up/down for example
		return;

	if (keyEvent->action == GLFW_PRESS)
		s_keyMap[keyEvent->key].store(true);
	else if (keyEvent->action == GLFW_RELEASE)
		s_keyMap[keyEvent->key].store(false);

	s_keyEvents.push(keyEvent);
}


int main(int argc, char **argv)
{
	// Create a GLFW window without an OpenGL context.
	glfwSetErrorCallback(glfw_errorCallback);
	if (!glfwInit())
		return 1;
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	GLFWwindow *window = glfwCreateWindow(1280, 720, "helloworld multithreaded", nullptr, nullptr);
	if (!window)
		return 1;
	glfwSetKeyCallback(window, glfw_keyCallback);
	// Call bgfx::renderFrame before bgfx::init to signal to bgfx not to create a render thread.
	// Most graphics APIs must be used on the same thread that created the window.
	bgfx::renderFrame();
	// Create a thread to call the bgfx API from (except bgfx::renderFrame).
	RenderThreadArgs apiThreadArgs;
#if BX_PLATFORM_LINUX || BX_PLATFORM_BSD
	apiThreadArgs.platformData.ndt = glfwGetX11Display();
	apiThreadArgs.platformData.nwh = (void*)(uintptr_t)glfwGetX11Window(window);
#elif BX_PLATFORM_OSX
	apiThreadArgs.platformData.nwh = glfwGetCocoaWindow(window);
#elif BX_PLATFORM_WINDOWS
	apiThreadArgs.platformData.nwh = glfwGetWin32Window(window);
#endif
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	apiThreadArgs.width = (uint32_t)width;
	apiThreadArgs.height = (uint32_t)height;
	//bx::Thread apiThread;
	//apiThread.init(runRenderThread, &apiThreadArgs);




	bgfx::Init init;
	init.platformData.nwh = glfwGetWin32Window(window);// args->platformData;
	init.resolution.width = apiThreadArgs.width;
	init.resolution.height = apiThreadArgs.height;
	init.resolution.reset = BGFX_RESET_VSYNC;
	if (!bgfx::init(init))
		return 1;

	loadResources();

	// Set view 0 to the same dimensions as the window and to clear the color buffer.
	const bgfx::ViewId kClearView = 0;
	bgfx::setViewClear(kClearView, BGFX_CLEAR_COLOR, 0xFF88FFFF);
	bgfx::setViewRect(kClearView, 0, 0, bgfx::BackbufferRatio::Equal);




	//bx::Thread logicThread;
	//logicThread.init(runLogicThread, nullptr);

	// Run GLFW message pump.
	bool exit = false;
	while (!exit) {
		glfwPollEvents();
		// Send window close event to the API thread.
		if (glfwWindowShouldClose(window)) {
			s_systemEvents.push(new ExitEvent);
			exit = true;
		}
		// Send window resize event to the API thread.
		int oldWidth = width, oldHeight = height;
		glfwGetWindowSize(window, &width, &height);
		if (width != oldWidth || height != oldHeight) {
			auto resize = new ResizeEvent;
			resize->width = (uint32_t)width;
			resize->height = (uint32_t)height;
			s_systemEvents.push(resize);
		}



		// This dummy draw call is here to make sure that view 0 is cleared if no other draw calls are submitted to view 0.
		//bgfx::touch(kClearView);

		float view[16];

		bx::Vec3 at = { 0.0f, 0.0f,  0.0f };
		bx::Vec3 eye = { 0.0f, 0.0f, -35.f };

		bx::mtxLookAt(view, eye, at);

		float proj[16];
		bx::mtxProj(proj, 60, float(width) / float(height), 0.1f, 100.0f, bgfx::getCaps()->homogeneousDepth);
		bgfx::setViewTransform(0, view, proj);

		bgfx::setViewRect(0, 0, 0, uint16_t(width), uint16_t(height));

		for (int i = -10; i < 10; i++) {
			for (int j = -10; j < 10; j++) {
				for (int k = -10; k < 10; k++) {
					float mtx[16];
					bx::mtxTranslate(mtx, i * 2, j * 2, k * 2);

					bgfx::setTransform(mtx);
					// Set vertex and index buffer.
					bgfx::setVertexBuffer(0, g_resources.m_vbh);
					bgfx::setIndexBuffer(g_resources.m_ibh);

					u64 state = BGFX_STATE_WRITE_R
						| BGFX_STATE_WRITE_G
						| BGFX_STATE_WRITE_B
						| BGFX_STATE_WRITE_A
						| BGFX_STATE_WRITE_Z
						| BGFX_STATE_DEPTH_TEST_LESS
						//| BGFX_STATE_CULL_CW
						| BGFX_STATE_MSAA;
					// BGFX_STATE_PT_TRISTRIP;

					// Set render states.
					bgfx::setState(state);

					bgfx::submit(0, g_resources.vertexColorProgram);

				}
			}
		}

		bgfx::frame();












		// Wait for the API thread to call bgfx::frame, then process submitted rendering primitives.
		//bgfx::renderFrame();
	}
	// Wait for the API thread to finish before shutting down.
	while (bgfx::RenderFrame::NoContext != bgfx::renderFrame()) {}
	//apiThread.shutdown();
	//logicThread.shutdown();
	glfwTerminate();
	return 0;// apiThread.getExitCode();
}
