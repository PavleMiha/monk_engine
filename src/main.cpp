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
#include <bx/timer.h>
#include <bx/os.h>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <imgui/imgui.h>
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

bx::SpScUnboundedQueue s_systemEventsRender(getDefaultAllocator());
bx::SpScUnboundedQueue s_systemEventsLogic(getDefaultAllocator());
bx::SpScUnboundedQueue s_keyEvents(getDefaultAllocator());

Resources g_resources;
std::atomic<i32> g_beingRendered(0);
std::atomic<i32> g_beingUpdated(0);
GameState g_gameStates[NUM_GAME_STATES];

#define MAIN_LOOP_TIME_BUFFER_SIZE 30
f64				 g_frameTimes[MAIN_LOOP_TIME_BUFFER_SIZE];
std::atomic<f64> g_averageMainFrameTime;

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

	s_keyEvents.push(keyEvent);
}

static void glfw_mouseCallback(GLFWwindow* window, int button, int action, int mods)
{
	auto mouseEvent = new MouseEvent;
	mouseEvent->button = button;
	mouseEvent->action = action;

	s_keyEvents.push(mouseEvent);
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
	glfwSetMouseButtonCallback(window, glfw_mouseCallback);

	// Call bgfx::renderFrame before bgfx::init to signal to bgfx not to create a render thread.
	// Most graphics APIs must be used on the same thread that created the window.
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
	apiThreadArgs.window = window;
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	apiThreadArgs.width = (uint32_t)width;
	apiThreadArgs.height = (uint32_t)height;
	bx::Thread apiThread;

	apiThread.init(runRenderThread, &apiThreadArgs);

	bx::Thread logicThread;
	logicThread.init(runLogicThread, &apiThreadArgs);

	// Run GLFW message pump.
	bool exit = false;
	i64 lastFrameCounter = bx::getHPCounter();
	i32 frame_time_index = 0;

	while (!exit) {
		glfwPollEvents();
		// Send window close event to the API thread.
		if (glfwWindowShouldClose(window)) {
			s_systemEventsRender.push(new ExitEvent);
			s_systemEventsLogic.push(new ExitEvent);
			exit = true;
		}
		// Send window resize event to the API thread.
		int oldWidth = width, oldHeight = height;
		glfwGetWindowSize(window, &width, &height);
		if (width != oldWidth || height != oldHeight) {
			auto resize = new ResizeEvent;
			resize->width = (uint32_t)width;
			resize->height = (uint32_t)height;
			s_systemEventsRender.push(resize);
		}

		// Wait for the API thread to call bgfx::frame, then process submitted rendering primitives.
		//bgfx::renderFrame();

		i64 currentCounter = bx::getHPCounter();
		f64 frameTime = (f64)(bx::getHPCounter() - lastFrameCounter) / (f64)bx::getHPFrequency();
		lastFrameCounter = currentCounter;

		g_frameTimes[frame_time_index] = frameTime;
		frame_time_index = (frame_time_index + 1) % FRAME_TIMES_BUFFER_SIZE;
		f64 averageFrameTime = 0.0;
		for (int i = 0; i < FRAME_TIMES_BUFFER_SIZE; i++) {
			averageFrameTime += frameTime;
		}
		averageFrameTime /= FRAME_TIMES_BUFFER_SIZE;
		g_averageMainFrameTime.store(averageFrameTime);

		i64 ticksPerUpdate = bx::getHPFrequency() * ((f64)1.0 / (f64)480.0);

		i64 ticksLeft = ticksPerUpdate - (bx::getHPCounter() - lastFrameCounter);
		while (ticksLeft > 0) {
			bx::yield();
			ticksLeft = ticksPerUpdate - (bx::getHPCounter() - lastFrameCounter);
		}
	}

	// Wait for the API thread to finish before shutting down.
	//while (bgfx::RenderFrame::NoContext != bgfx::renderFrame()) {}
	apiThread.shutdown();
	logicThread.shutdown();
	glfwTerminate();
	return apiThread.getExitCode();
}
