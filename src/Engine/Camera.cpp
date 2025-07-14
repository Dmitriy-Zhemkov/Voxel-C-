#include "Engine/Camera.h"
using namespace cube;
void Camera::addYawPitch(float yOff, float pOff) {
    m_yaw += yOff; m_pitch += pOff;
    if (m_pitch > 89) m_pitch = 89; if (m_pitch < -89) m_pitch = -89;
    updateVectors();
}
void Camera::updateVectors() {
    glm::vec3 f;
    f.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    f.y = sin(glm::radians(m_pitch));
    f.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    m_front = glm::normalize(f);
}