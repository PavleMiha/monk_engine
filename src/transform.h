/*
 * Copyright 2024 Pavle Mihajlovic. All rights reserved.
 */

#pragma once

#include "types.h"
#include "monk_math.h"

#define NO_PARENT UINT32_MAX

struct Transform
{
	u32  parent;
	
	vec3 local_pos;
	quat local_orientation;
	vec3 local_scale;

	mat4 world_matrix;
	b8	 dirty;
};
