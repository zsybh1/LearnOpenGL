#pragma once
#include <camera.h>
#include <cmath>
#include <algorithm>

#include <iostream>

glm::mat4 Camera::view_matrix() {
	/*自行构造的视图矩阵
	glm::vec3 right = glm::normalize(glm::cross(worldUp, -towards));
	glm::vec3 up = glm::normalize(glm::cross(-towards, right));
	glm::vec3 to = glm::normalize(-towards);
	glm::mat4 mat(
		right.x, right.y, right.z, 0.0f,
		up.x, up.y, up.z, 0.0f,
		to.x, to.y, to.z, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);
	mat = glm::translate(mat, -pos);
	return mat;
	*/
	return glm::lookAt(pos, pos + towards, worldUp);
}

void Camera::change_direction(double xpos, double ypos)
{
	static float lastX = xpos;
	static float lastY = ypos;
	float xOffset = xpos - lastX;
	float yOffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;
	yaw += xOffset * sensitivity;
	pitch += yOffset * sensitivity;

	pitch = std::min(std::max(pitch, -89.0f), 89.0f);
	
	towards.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
	towards.y = sin(glm::radians(pitch));
	towards.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
}

void Camera::change_fov(double yoffset) {
	if (fov >= 1.0f && fov <= 90.0f)
		fov -= yoffset;
	if (fov <= 1.0f)
		fov = 1.0f;
	if (fov >= 90.0f)
		fov = 90.0f;
}