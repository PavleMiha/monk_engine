/*
 * Copyright 2024 Pavle Mihajlovic. All rights reserved.
 */
#pragma once

#include "types.h"
#include <atomic>
#include <bx/thread.h>
#include <bgfx/bgfx.h>
#include "render_state.h"
#include "glm/glm.hpp"

struct GameState
{
	glm::vec3 playerSpeed;

	RenderState renderState;
};

extern GameState g_gameState;
