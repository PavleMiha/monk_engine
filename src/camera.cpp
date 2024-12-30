/*
 * Copyright 2024 Pavle Mihajlovic. All rights reserved.
 */

#include "camera.h"
#include "monk_math.h"

vec3 Camera::getForwardDirection() const {
	const vec3 direction =
	{
		bx::cos(m_pitch) * bx::sin(m_yaw),
		bx::sin(m_pitch),
		bx::cos(m_pitch) * bx::cos(m_yaw),
	};
	return direction;
}

vec3 Camera::getRightDirection() const {
	const vec3 right =
	{
		glm::sin(m_yaw - bx::kPiHalf),
		0.0f,
		glm::cos(m_yaw - bx::kPiHalf),
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

	vec3 at = m_pos + forward;

	//change this for Left hand
	*out = (glm::lookAtLH(m_pos, at, up));
	//bx::mtxLookAt(out->data, m_pos, at, up);
}

void Camera::getProjMat(mat4* out) const {
	
	//change this for Left hand
	if (m_homogenousDepth)
		*out = (glm::perspectiveLH_NO(glm::radians(m_verticalFOV), m_aspectRatio, m_near, m_far));//-1 to 1
	else
		*out = (glm::perspectiveLH_ZO(glm::radians(m_verticalFOV), m_aspectRatio, m_near, m_far));//0 to 1

	//bx::mtxProj(out->data, m_horizontalFOV, m_aspectRatio, m_near, m_far, m_homogenousDepth);
}
