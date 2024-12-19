/*
 * Copyright 2024 Pavle Mihajlovic. All rights reserved.
 */

#pragma once

#include "types.h"
#include "monk_math.h"
#include <atomic>
#include <bx/math.h>
#include <bx/thread.h>
#include <bgfx/bgfx.h>
#include "camera.h"

#define NUM_RENDER_STATES 3
struct RenderState
{
	std::atomic<bool> isBusy;
	f64				  logicThreadDeltaAverage;
	f64				  mainThreadDeltaAverage;
	f64				  renderThreadDeltaAverage;
	i64				  timeGenerated;
	Camera			  camera;
	bool			  showStats;
};

extern RenderState g_renderState[NUM_RENDER_STATES];
