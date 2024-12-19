/*
 * Copyright 2024 Pavle Mihajlovic. All rights reserved.
 */

#pragma once


#include "types.h"
#include <bx/thread.h>
#include <bgfx/bgfx.h>
#include "GLFW/glfw3.h"

struct RenderThreadArgs
{
	GLFWwindow* window;
	bgfx::PlatformData platformData;
	uint32_t width;
	uint32_t height;
};

i32 runRenderThread(bx::Thread* self, void* userData);