/*
 * Copyright 2024 Pavle Mihajlovic. All rights reserved.
 */

#pragma once

#include "types.h"
#include "monk_math.h"

struct Transform
{
	vec3 m_pos = { 0.0f, 0.0f, 0.0f };
	f32  m_pitch = 0.0f;
	f32  m_yaw = 0.0f;
	f32  m_roll = 0.0f;
};
