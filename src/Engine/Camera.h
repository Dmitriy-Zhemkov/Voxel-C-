#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
namespace cube {
    class Camera {
    public:
        Camera(float fov, float aspect, float n, float f);
        void setPosition(const glm::vec3& p); void move(const glm::vec3& d);
        void addYawPitch(float y, float p);
        const glm::vec3& front()const { return m_front; }
        const glm::vec3& position()const { return m_pos; }
        glm::mat4 view()const; glm::mat4 proj()const;
    private: void update(); glm::vec3 m_pos{ 0,10,30 }, m_front{ -0.5f,-0.3f,-0.8f }, m_up{ 0,1,0 }; float yaw{ -90 }, pitch{ 0 }, fov, asp, near_, far_;
    };
}