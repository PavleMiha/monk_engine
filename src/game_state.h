/*
 * Copyright 2024 Pavle Mihajlovic. All rights reserved.
 */
#pragma once

#include "types.h"
#include <atomic>
#include <bx/thread.h>
#include <bgfx/bgfx.h>
#include "render_state.h"
#include "camera.h"
#include "tinystl/vector.h"
//#include "glm/glm.hpp"

#define FRAME_TIMES_BUFFER_SIZE 30

struct GameState
{
	vec2i		windowSize = { };
	Camera		camera = { };
	const float playerAcceleration = 200.0f;
	const float angularSpeed = 3.0f;
	vec3		playerVelocity = vec3(0.0f);
	f64			frameTimes[FRAME_TIMES_BUFFER_SIZE];
	u32			frameTimeIndex = 0;
	//glm::vec3 playerSpeed;
	RenderState renderState;
};

extern GameState g_gameState;
