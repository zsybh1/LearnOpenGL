#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>

class Camera {
public:
	glm::vec3 pos;
	glm::vec3 towards;
	glm::vec3 worldUp;
	float yaw;
	float pitch;
	float sensitivity;
	float fov;

	Camera(const glm::vec3 &cPos, const glm::vec3 &cWorldUp = glm::vec3(0.0f, 1.0f, 0.0f),
		float sens = 0.1f, float cYaw = -90.0f, float cPitch = 0.0f, float cFov = 45.0f)
		: pos(cPos), towards(glm::vec3(0.0, 0.0, -1.0f)), worldUp(cWorldUp) , sensitivity(sens)
		, yaw(cYaw), pitch(cPitch), fov(cFov){}
	glm::mat4 view_matrix();
	void change_direction(double xpos, double ypos);
	void change_fov(double yoffset);
};