/*
 * Copyright 2024 Pavle Mihajlovic. All rights reserved.
 */

#pragma once

#include "types.h"
#include "monk_math.h"

struct Transform
{
	vec3 pos = { 0.0f, 0.0f, 0.0f };
	f32  pitch = 0.0f;
	f32  yaw = 0.0f;
	f32  roll = 0.0f;
};
