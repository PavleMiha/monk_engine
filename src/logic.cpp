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
//#include "math.h"


#include <cstdio>

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
					g_gameState.renderState.showStats= !g_gameState.renderState.showStats;

			}
		}


		if (s_keyMap[GLFW_KEY_RIGHT].load())
			g_gameState.renderState.dummy+= delta;

		static const f32 speed = 5.0f;

		//glm::vec3 forward = glm::vecto

	//	glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), yaw, glm::vec3(0, 1, 0)) * glm::rotate(glm::mat4(1.0f), pitch, glm::vec3(1, 0, 0));
		if (isPressed(GLFW_KEY_W)) {
			g_gameState.renderState.cameraPos.z += delta * speed;
		}
		if (isPressed(GLFW_KEY_S)) {
			g_gameState.renderState.cameraPos.z -= delta * speed;
		}
		if (isPressed(GLFW_KEY_A)) {
			g_gameState.renderState.cameraPos.x += delta * speed;
		}
		if (isPressed(GLFW_KEY_D)) {
			g_gameState.renderState.cameraPos.x -= delta * speed;
		}

		static const f32 turnSpeed = 1.0f;

		if (isPressed(GLFW_KEY_RIGHT)) {
			g_gameState.renderState.cameraYaw += delta * turnSpeed;
		}

		if (isPressed(GLFW_KEY_LEFT)) {
			g_gameState.renderState.cameraYaw -= delta * turnSpeed;
		}
		if (isPressed(GLFW_KEY_UP)) {
			g_gameState.renderState.cameraPitch += delta * turnSpeed;
		}

		if (isPressed(GLFW_KEY_DOWN)) {
			g_gameState.renderState.cameraPitch -= delta * turnSpeed;
		}


		printf("Camera pos %f %f %f\n", g_gameState.renderState.cameraPos.x, g_gameState.renderState.cameraPos.y, g_gameState.renderState.cameraPos.z);

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


		memcpy(&renderState, &g_gameState.renderState, sizeof(RenderState));

		renderState.timeGenerated = frameCounter;
		g_renderState[renderStateIndex].isBusy.store(false);
	}
	return 0;
}