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
#include "mesh.h"
#include "transform.h"
#include "tinystl/vector.h"
//#include "glm/glm.hpp"

#define NUM_GAME_STATES 3

#define FRAME_TIMES_BUFFER_SIZE 30
#define NUM_ENTITIES			8192

struct GameState
{
	vec2i	  window_size = { };
	Camera	  camera = { };
	const f32 player_acceleration = 200.0f;
	const f32 angular_speed = 3.0f;
	vec3	  player_velocity = vec3(0.0f);
	f64		  frame_times[FRAME_TIMES_BUFFER_SIZE];
	u32		  frame_time_index = 0;
	//glm::vec3 playerSpeed;

	f64		  logicThreadDeltaAverage;
	f64		  mainThreadDeltaAverage;
	f64		  renderThreadDeltaAverage;
	i64		  timeGenerated;
	bool	  showStats;

	Transform transforms[NUM_ENTITIES];
	Mesh	  mesh		[NUM_ENTITIES];
	//RenderState render_state;
};


extern std::atomic<i32> g_beingRendered;
extern std::atomic<i32> g_beingUpdated;
extern GameState g_gameStates[NUM_GAME_STATES];
