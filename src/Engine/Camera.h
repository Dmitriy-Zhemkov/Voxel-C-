#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace cube {
    class Camera {
    public:
        Camera(float fovDeg, float aspect, float nearP, float farP)
            : m_fov(fovDeg), m_aspect(aspect), m_near(nearP), m_far(farP)
        {
            updateVectors();
        }

        void setPosition(const glm::vec3& p) { m_pos = p; }
        void addYawPitch(float yawOff, float pitchOff);
        void move(const glm::vec3& delta) { m_pos += delta; }
        void setFront(const glm::vec3& front) { m_front = front; }

        const glm::vec3& front() const { return m_front; }
        const glm::vec3& position()const { return m_pos; }

        glm::mat4 view() const { return glm::lookAt(m_pos, m_pos + m_front, m_up); }
        glm::mat4 projection() const {
            return glm::perspective(glm::radians(m_fov), m_aspect, m_near, m_far);
        }
    private:
        void updateVectors();
        glm::vec3 m_pos{ 0,10,30 };
        glm::vec3 m_front{ -0.5f,-0.3f,-0.8f };
        glm::vec3 m_up{ 0,1,0 };
        float m_yaw{ -90.f }, m_pitch{ 0.f };
        float m_fov, m_aspect, m_near, m_far;
    };
}