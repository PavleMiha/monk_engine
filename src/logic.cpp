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

struct UpdateData {
	f64 delta;
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
		rightAcceleration	= 1.0f;
	}
	if (updateData.keysDown[GLFW_KEY_A]) {
		rightAcceleration	= -1.0f;
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
		nextGameState.playerVelocity += glm::normalize(accelDirection) * (f32)updateData.delta * prevGameState.playerAcceleration;
	}

	nextGameState.playerVelocity = nextGameState.playerVelocity * (f32)glm::pow(0.002, updateData.delta);

	nextGameState.camera.m_pos = prevGameState.playerVelocity * (f32)updateData.delta + prevGameState.camera.m_pos;

	if (updateData.keysDown[GLFW_KEY_RIGHT]) {
		nextGameState.camera.m_yaw = prevGameState.camera.m_yaw - updateData.delta * prevGameState.angularSpeed;
	}

	if (updateData.keysDown[GLFW_KEY_LEFT]) {
		nextGameState.camera.m_yaw = prevGameState.camera.m_yaw + updateData.delta * prevGameState.angularSpeed;
	}

	if (updateData.keysDown[GLFW_KEY_UP]) {
		nextGameState.camera.m_pitch = prevGameState.camera.m_pitch + updateData.delta * prevGameState.angularSpeed;
	}

	if (updateData.keysDown[GLFW_KEY_DOWN]) {
		nextGameState.camera.m_pitch = prevGameState.camera.m_pitch - updateData.delta * prevGameState.angularSpeed;
	}
}

void generateRenderState(RenderState& renderState, const GameState& gameState) {
	f64 logicStateAverage = 0.0;
	for (int i = 0; i < FRAME_TIMES_BUFFER_SIZE; i++) {
		logicStateAverage += gameState.frameTimes[i];
	}
	logicStateAverage /= (f64)FRAME_TIMES_BUFFER_SIZE;
	renderState.logicThreadDeltaAverage = logicStateAverage;
	memcpy(&renderState.camera, &gameState.camera, sizeof(Camera));
}

i32 runLogicThread(bx::Thread* self, void* userData) {
	auto args = (RenderThreadArgs*)userData;
	// Initialize bgfx using the native window handle and window resolution.

	i64 lastFrameCounter = bx::getHPCounter();
	bool exit = false;
	UpdateData updateData = {};

	GameState gameState[2];
	u32		  currentGameStateIndex = 0;

	gameState[currentGameStateIndex].windowSize.x = args->width;
	gameState[currentGameStateIndex].windowSize.y = args->height;

	while (!exit) {
		i64 frameCounter = bx::getHPCounter();
		updateData.delta = (f64)(frameCounter - lastFrameCounter)/(f64)bx::getHPFrequency();
		gameState->frameTimes[gameState->frameTimeIndex] = updateData.delta;
		gameState->frameTimeIndex = (gameState->frameTimeIndex + 1) % FRAME_TIMES_BUFFER_SIZE;
		lastFrameCounter = frameCounter;

		memset((void*)updateData.keysPressedThisFrame, 0, sizeof(updateData.keysPressedThisFrame));
		memset((void*)updateData.keysReleasedThisFrame, 0, sizeof(updateData.keysReleasedThisFrame));

		u32 prevGameStateIndex = currentGameStateIndex;
		currentGameStateIndex = (currentGameStateIndex + 1) % 2;

		const GameState& prevGameState = gameState[currentGameStateIndex];
		GameState& currentGameState = gameState[currentGameStateIndex];

		memcpy(&gameState[currentGameStateIndex], &gameState[prevGameStateIndex], sizeof(GameState));

		while (auto ev = (EventType*)s_keyEvents.pop()) {
			if (*ev == EventType::Resize) {
				auto resizeEvent = (ResizeEvent*)ev;
				gameState->windowSize.x = resizeEvent->width;
				gameState->windowSize.y = resizeEvent->height;
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


		u32 currentRenderStateIndex = 0;
		bool found = false;
		while (!found) {
			i64 oldestTime = INT64_MAX;

			for (i32 i = 0; i < NUM_RENDER_STATES; i++) {
				if (!g_renderState[i].isBusy.load()) {
					if (g_renderState[i].timeGenerated < oldestTime) {
						currentRenderStateIndex = i;
						oldestTime = g_renderState[i].timeGenerated;
					}
				}
			}

			bool expected = false;
			if (g_renderState[currentRenderStateIndex].isBusy.compare_exchange_weak(expected, true)) {
				found = true;
			}
		}

		RenderState& currentRenderState = g_renderState[currentRenderStateIndex];

		currentRenderState.timeGenerated = frameCounter;

		generateRenderState(currentRenderState, currentGameState);

		currentRenderState.isBusy.store(false);

		i64 ticksPerUpdate =
			bx::getHPFrequency() * ((f64)1.0 / (f64)240.0);
		
		i64 ticksLeft = ticksPerUpdate - (bx::getHPCounter() - lastFrameCounter);
		while (ticksLeft > 1) {
			bx::yield();
			ticksLeft = ticksPerUpdate - (bx::getHPCounter() - lastFrameCounter);
		}
	}
	return 0;
}