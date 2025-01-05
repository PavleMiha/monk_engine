/*
 * Copyright 2024 Pavle Mihajlovic. All rights reserved.
 */

#pragma once

#include "types.h"
#include "monk_math.h"

struct Camera
{
	vec3 pos   = { 0.0f, 0.0f, -80.0f };
	f32  pitch = 0.0f;
	f32  yaw   = 0.0f;
	f32  roll  = 0.0f;

	f32	aspect_ratio	 = 1.0f;
	f32 vertical_FOV     = 60.f;
	f32 near_plane		 = 0.1f;
	f32 far_plane		 = 1000.0f;
	b8  homogenous_depth = true;

	vec3 getForwardDirection() const;
	vec3 getRightDirection()   const;
	vec3 getUpDirection()	   const;
	void getViewMat(mat4* out) const;
	void getProjMat(mat4* out) const;
};
