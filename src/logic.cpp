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
#include "monk_math.h"
#include "tinystl/vector.h"
#include "imgui/imgui.h"
#include <cstdio>

extern bx::SpScUnboundedQueue s_keyEvents;
extern bx::SpScUnboundedQueue s_systemEventsLogic;

struct UpdateData {
	f32 delta;
	vec2 mousePos;
	i32 width;
	i32 height;
	b8	mouseButtons		  [GLFW_MOUSE_BUTTON_LAST];
	b8  keysPressedThisFrame  [GLFW_KEY_LAST];
	b8  keysReleasedThisFrame [GLFW_KEY_LAST];
	b8  keysDown			  [GLFW_KEY_LAST];
};

void logicUpdate(GameState& nextGameState, const GameState& prevGameState, const UpdateData& updateData) {

	f32 forwardAcceleration = 0.0f;
	f32 rightAcceleration	= 0.0f;
	f32 upAcceleration		= 0.0f;

	if (updateData.keysDown[GLFW_KEY_W]) {
		forwardAcceleration = 1.0f;
	}
	if (updateData.keysDown[GLFW_KEY_S]) {
		forwardAcceleration = -1.0f;
	}
	if (updateData.keysDown[GLFW_KEY_D]) {
		rightAcceleration	= -1.0f;
	}
	if (updateData.keysDown[GLFW_KEY_A]) {
		rightAcceleration	= 1.0f;
	}
	if (updateData.keysDown[GLFW_KEY_E]) {
		upAcceleration		= 1.0f;
	}
	if (updateData.keysDown[GLFW_KEY_Q]) {
		upAcceleration		= -1.0f;
	}

	vec3 accelDirection = nextGameState.camera.getForwardDirection() * forwardAcceleration +
			nextGameState.camera.getRightDirection() * rightAcceleration +
			nextGameState.camera.getUpDirection() * upAcceleration;

	if (glm::length(accelDirection) > 0.0001) {
		printf("Higher\n");
		nextGameState.player_velocity += glm::normalize(accelDirection) * updateData.delta * prevGameState.player_acceleration;
	}

	nextGameState.player_velocity = nextGameState.player_velocity * glm::pow(0.002f, updateData.delta);

	nextGameState.camera.pos = prevGameState.player_velocity * updateData.delta + prevGameState.camera.pos;

	if (updateData.keysDown[GLFW_KEY_RIGHT]) {
		nextGameState.camera.yaw = prevGameState.camera.yaw + updateData.delta * prevGameState.angular_speed;
	}

	if (updateData.keysDown[GLFW_KEY_LEFT]) {
		nextGameState.camera.yaw = prevGameState.camera.yaw - updateData.delta * prevGameState.angular_speed;
	}

	if (updateData.keysDown[GLFW_KEY_UP]) {
		nextGameState.camera.pitch = prevGameState.camera.pitch + updateData.delta * prevGameState.angular_speed;
	}

	if (updateData.keysDown[GLFW_KEY_DOWN]) {
		nextGameState.camera.pitch = prevGameState.camera.pitch - updateData.delta * prevGameState.angular_speed;
	}
}

/*void generateRenderState(RenderState& render_state, const GameState& gameState) {
	f64 logicStateAverage = 0.0;
	for (int i = 0; i < FRAME_TIMES_BUFFER_SIZE; i++) {
		logicStateAverage += gameState.frame_times[i];
	}
	logicStateAverage /= (f64)FRAME_TIMES_BUFFER_SIZE;
	render_state.logicThreadDeltaAverage = logicStateAverage;
	memcpy(&render_state.camera, &gameState.camera, sizeof(Camera));
}*/

i32 runLogicThread(bx::Thread* self, void* userData) {
	auto args = (RenderThreadArgs*)userData;
	// Initialize bgfx using the native window handle and window resolution.

	i64 lastFrameCounter = bx::getHPCounter();
	bool exit = false;
	UpdateData updateData = {};

	u32		  currentGameStateIndex = 0;

	g_gameStates[currentGameStateIndex].window_size.x = args->width;
	g_gameStates[currentGameStateIndex].window_size.y = args->height;

	i32 previousGameStateIndex = currentGameStateIndex;

	while (!exit) {
		while (auto ev = (EventType*)s_systemEventsLogic.pop()) {
			if (*ev == EventType::Exit) {
				exit = true;
			}
		}

		//find oldest game not being written to
		bool found = false;

		while (!found) {
			i64 oldestTime = -1;

			for (i32 i = 0; i < NUM_GAME_STATES; i++) {
				if (i != previousGameStateIndex && i != g_beingRendered.load()) {
					if (g_gameStates[i].timeGenerated > oldestTime) {
						currentGameStateIndex = i;
						oldestTime = g_gameStates[i].timeGenerated;
					}
				}
			}

			bool expected = false;

			g_beingUpdated.store(currentGameStateIndex);

			found = true;
		}

		const GameState& prevGameState = g_gameStates[previousGameStateIndex];
		GameState& currentGameState = g_gameStates[currentGameStateIndex];

		memcpy(&currentGameState, &prevGameState, sizeof(GameState));

		i64 frameCounter = bx::getHPCounter();
		updateData.delta = (f32)(frameCounter - lastFrameCounter)/(f64)bx::getHPFrequency();
		currentGameState.frame_times[currentGameState.frame_time_index] = updateData.delta;
		currentGameState.frame_time_index = (currentGameState.frame_time_index + 1) % FRAME_TIMES_BUFFER_SIZE;
		lastFrameCounter = frameCounter;

		f64 logicStateAverage = 0.0;
		for (int i = 0; i < FRAME_TIMES_BUFFER_SIZE; i++) {
			logicStateAverage += currentGameState.frame_times[i];
		}
		logicStateAverage /= (f64)FRAME_TIMES_BUFFER_SIZE;
		currentGameState.logicThreadDeltaAverage = logicStateAverage;

		memset((void*)updateData.keysPressedThisFrame, 0, sizeof(updateData.keysPressedThisFrame));
		memset((void*)updateData.keysReleasedThisFrame, 0, sizeof(updateData.keysReleasedThisFrame));


		while (auto ev = (EventType*)s_keyEvents.pop()) {
			if (*ev == EventType::Resize) {
				auto resizeEvent = (ResizeEvent*)ev;
				currentGameState.window_size.x = resizeEvent->width;
				currentGameState.window_size.y = resizeEvent->height;
			}
			if (*ev == EventType::Key) {
				auto keyEvent = (KeyEvent*)ev;
				if (keyEvent->action == GLFW_PRESS) {
					updateData.keysPressedThisFrame[keyEvent->key] = true;
					updateData.keysDown[keyEvent->key] = true;
				}

				if (keyEvent->action == GLFW_RELEASE) {
					updateData.keysReleasedThisFrame[keyEvent->key] = true;
					updateData.keysDown[keyEvent->key] = false;
				}
			}
		}
			
		logicUpdate(currentGameState, prevGameState, updateData);
		g_beingUpdated.store(-1);

		i64 ticksPerUpdate =
			bx::getHPFrequency() * ((f64)1.0 / (f64)240.0);
		
		i64 ticksLeft = ticksPerUpdate - (bx::getHPCounter() - lastFrameCounter);
		while (ticksLeft > 1) {
			bx::yield();
			ticksLeft = ticksPerUpdate - (bx::getHPCounter() - lastFrameCounter);
		}

		previousGameStateIndex = currentGameStateIndex;
	}
	return 0;
}