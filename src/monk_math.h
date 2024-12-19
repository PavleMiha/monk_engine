/*
 * Copyright 2024 Pavle Mihajlovic
 */
#pragma once

#include "bx/math.h"

#define GLM_ENABLE_EXPERIMENTAL

#include "glm/glm.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/matrix_projection.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/euler_angles.hpp"

#include "types.h"

typedef glm::mat4x4  mat4;
typedef glm::vec4	 vec4;
typedef glm::vec3	 vec3;
typedef glm::vec2	 vec2;
typedef glm::i32vec2 vec2i;

const static vec3 s_forward(0.0f, 0.0f, 1.0f);
const static vec3 s_right(1.0f, 0.0f, 0.0f);
vec3 rotateVector(const vec3& vec, float pitch, float yaw);