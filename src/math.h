/*
 * Copyright 2024 Pavle Mihajlovic
 */
#pragma once

#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/constants.hpp>             // For glm::radians and glm::pi
#include <glm/gtx/euler_angles.hpp>          // Creating matrices from Euler angles

typedef glm::mat4 mat;
typedef glm::vec3 vec3;
typedef glm::vec2 vec2;

vec3 rotateVector(const vec3& vec, float pitchDegrees, float yawDegrees) {
    // Convert degrees to radians
    float pitch = glm::radians(pitchDegrees);
    float yaw = glm::radians(yawDegrees);

    // Create a rotation matrix using yaw, pitch, and zero roll
    glm::mat4 rotationMatrix = glm::yawPitchRoll(yaw, pitch, 0.0f);

    // Convert the original vector to a 4D homogeneous coordinate
    glm::vec4 vec4(vec, 1.0f);

    // Apply the rotation matrix to the vector
    glm::vec4 rotatedVec4 = rotationMatrix * vec4;

    // Convert back to 3D vector
    return glm::vec3(rotatedVec4);
}