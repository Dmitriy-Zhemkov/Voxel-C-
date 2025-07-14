#include "Engine/Camera.h"
using namespace cube; Camera::Camera(float f, float a, float n, float fP) :fov(f), asp(a), near_(n), far_(fP) { update(); }
void Camera::setPosition(const glm::vec3& p) { m_pos = p; }
void Camera::move(const glm::vec3& d) { m_pos += d; }
void Camera::addYawPitch(float y, float p) { yaw += y; pitch += p; if (pitch > 89)pitch = 89; if (pitch < -89)pitch = -89; update(); }
void Camera::update() { glm::vec3 f; f.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch)); f.y = sin(glm::radians(pitch)); f.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch)); m_front = glm::normalize(f); }
glm::mat4 Camera::view()const { return glm::lookAt(m_pos, m_pos + m_front, m_up); }
glm::mat4 Camera::proj()const { return glm::perspective(glm::radians(fov), asp, near_, far_); }