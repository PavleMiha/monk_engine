/*
 * Copyright 2024 Pavle Mihajlovic. All rights reserved.
 */
#pragma once

#include "types.h"
#include <bx/thread.h>
#include <bgfx/bgfx.h>

i32 runLogicThread(bx::Thread* self, void* userData);