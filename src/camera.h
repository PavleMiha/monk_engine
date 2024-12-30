/*
 * Copyright 2024 Pavle Mihajlovic. All rights reserved.
 */

#pragma once

#include "types.h"
#include "monk_math.h"

struct Camera
{
	vec3 m_pos   = { 0.0f, 0.0f, -80.0f };
	f32  m_pitch = 0.0f;
	f32  m_yaw   = 0.0f;
	f32  m_roll  = 0.0f;

	f32	 m_aspectRatio	   = 1.0f;
	f32  m_verticalFOV     = 60.f;
	bool m_homogenousDepth = true;
	f32  m_near			   = 0.1f;
	f32  m_far			   = 1000.0f;

	vec3 getForwardDirection() const;
	vec3 getRightDirection()   const;
	vec3 getUpDirection()	   const;
	void getViewMat(mat4* out) const;
	void getProjMat(mat4* out) const;
};
