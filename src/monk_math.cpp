/*
 * Copyright 2024 Pavle Mihajlovic
 */
#include "monk_math.h"

vec3 rotateVector(const vec3& vec, float pitch, float yaw) {
	// Convert degrees to radians
	//float pitch = bx::toRad(pitchDegrees);// ::radians(pitchDegrees);
	//float yaw = bx::toRad(yawDegrees);// ::radians(pitchDegrees);

	// Create a rotation matrix using yaw, pitch, and zero roll
	mat4 rotationMatrix;
	return glm::eulerAngleYX(yaw, pitch) * glm::vec4(vec, 1.0f);
	/*bx::mtxRotateY(rotationMatrix.data, yaw);

	vec4 yawedVec;
	bx::vec4MulMtx(yawedVec.data, extended.data, rotationMatrix.data);

	bx::mtxRotateX(rotationMatrix.data, pitch);

	vec4 result;
	bx::vec4MulMtx(result.data, yawedVec.data, rotationMatrix.data);

	// Convert back to 3D vector
	return { result.x, result.y, result.z };*/
}

/*
mat4 createViewMatrix(float cameraYaw, float cameraPitch, const vec3& cameraPos) {
	
	// Calculate the forward direction
	float yaw= glm::radians(cameraYaw);
	float pitch= glm::radians(cameraPitch);

	mat4 rotateZ;
	bx::mtxRotateZ(rotateZ.data, yaw);
	mat4 rotateX;
	bx::mtxRotateX(rotateX.data, pitch);
	mat4 translate;
	bx::mtxTranslate(translate.data, cameraPos.x, cameraPos.y, cameraPos.z);
	
	mat4 rotation;
	bx::mtxMul(rotation.data, rotateZ.data, rotateX.data);
	mat4 translatedRotation;
	bx::mtxMul(translatedRotation.data, rotation.data, translate.data);

	return translatedRotation;
}

mat4 createProjectionMatrix(float fovHorizontal, float aspectRatio, float nearPlane, float farPlane, bool homogeneousDepth) {

	mat4 proj;
	bx::mtxProj(proj.data, 60.0f, aspectRatio, 0.1f, 100.0f, homogeneousDepth);
	return proj;
}*/

