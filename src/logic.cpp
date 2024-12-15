/*
 * Copyright 2024 Pavle Mihajlovic. All rights reserved.
 */

#include "render.h"

#include "input.h"
#include "game_state.h"
#include "GLFW/glfw3.h"
#include "render_state.h"
#include "logo.h"
#include "bx/os.h"
#include "bx/timer.h"

extern bx::SpScUnboundedQueue s_keyEvents;

i32 runLogicThread(bx::Thread* self, void* userData)
{
	auto args = (RenderThreadArgs*)userData;
	// Initialize bgfx using the native window handle and window resolution.

	i64 lastFrameCounter = bx::getHPCounter();
	bool exit = false;
	while (!exit) {
		// Handle events from the main thread.

		i32 renderStateIndex = 0;

		i64 frameCounter = bx::getHPCounter();
		f64 delta = (f64)(frameCounter - lastFrameCounter)/(f64)bx::getHPFrequency();
		lastFrameCounter = frameCounter;

		RenderState& renderState = g_renderState[renderStateIndex];

		while (auto ev = (EventType*)s_keyEvents.pop()) {
			if (*ev == EventType::Key) {
				auto keyEvent = (KeyEvent*)ev;
				if (keyEvent->key == GLFW_KEY_F1 && keyEvent->action == GLFW_RELEASE)
					g_gameState.showStats= !g_gameState.showStats;

			}
		}


		if (s_keyMap[GLFW_KEY_RIGHT].load())
			g_gameState.dummy+= delta;

		//g_gameState.dummy = frameTime;

		bool found = false;
		while (!found) {
			i64 oldestTime = INT64_MAX;

			for (i32 i = 0; i < NUM_RENDER_STATES; i++) {
				if (!g_renderState[i].isBusy.load()) {
					if (g_renderState[i].timeGenerated < oldestTime) {
						renderStateIndex = i;
						oldestTime = g_renderState[i].timeGenerated;
					}
				}
			}

			bool expected = false;
			if (g_renderState[renderStateIndex].isBusy.compare_exchange_weak(expected, true)) {
				found = true;
			}
		}


		renderState.dummy = g_gameState.dummy;
		renderState.showStats = g_gameState.showStats;

		renderState.timeGenerated = frameCounter;
		g_renderState[renderStateIndex].isBusy.store(false);
	}
	return 0;
}