/*
 * Copyright 2024 Pavle Mihajlovic. All rights reserved.
 */

#include "camera.h"
#include "monk_math.h"

vec3 Camera::getForwardDirection() const {
	const vec3 direction =
	{
		bx::cos(pitch) * bx::sin(yaw),
		bx::sin(pitch),
		bx::cos(pitch) * bx::cos(yaw),
	};
	return direction;
}

vec3 Camera::getRightDirection() const {
	const vec3 right =
	{
		glm::sin(yaw - bx::kPiHalf),
		0.0f,
		glm::cos(yaw - bx::kPiHalf),
	};
	return right;
}

vec3 Camera::getUpDirection() const {
	return glm::cross(getRightDirection(), getForwardDirection());
}

void Camera::getViewMat(mat4* out) const {
	const vec3 forward = getForwardDirection();

	const vec3 right = getRightDirection();

	const vec3 up = getUpDirection();

	vec3 at = pos + forward;

	//change this for Left hand
	*out = (glm::lookAtLH(pos, at, up));
	//bx::mtxLookAt(out->data, m_pos, at, up);
}

void Camera::getProjMat(mat4* out) const {
	
	//change this for Left hand
	if (homogenous_depth)
		*out = (glm::perspectiveLH_NO(glm::radians(vertical_FOV), aspect_ratio, near_plane, far_plane));//-1 to 1
	else
		*out = (glm::perspectiveLH_ZO(glm::radians(vertical_FOV), aspect_ratio, near_plane, far_plane));//0 to 1

	//bx::mtxProj(out->data, m_horizontalFOV, m_aspectRatio, m_near, m_far, m_homogenousDepth);
}
