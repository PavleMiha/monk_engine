/*
 * Copyright 2024 Pavle Mihajlovic. All rights reserved.
 */

#pragma once


#include "types.h"
#include <atomic>
#include <bx/math.h>
#include <bx/thread.h>
#include <bgfx/bgfx.h>

#define NUM_RENDER_STATES 3
struct RenderState
{
	std::atomic<bool> isBusy;
	i64				  timeGenerated;

	f32				  dummy;
	bool			  showStats;

	glm::vec3		  cameraPos   = { 0.0f, 0.0f, -30.0f };
	f32				  cameraPitch = 0.0f;
	f32				  cameraYaw   = 0.0f;

};

extern RenderState g_renderState[NUM_RENDER_STATES];
