/*
 * Copyright 2024 Pavle Mihajlovic. All rights reserved.
 */

#include "types.h"
#include <atomic>
#include <bx/thread.h>
#include <bgfx/bgfx.h>

struct GameState
{
	f64				  dummy;
	bool			  showStats;
};

extern GameState g_gameState;
