#pragma once
#include <string>
#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>
namespace cube {
    class Shader {
    public:
        bool load(const std::string&, const std::string&);
        void use()const { if (m_id) glUseProgram(m_id); }
        GLuint program()const { return m_id; }
        void setFloat(const std::string&, float)const;
        void setVec3(const std::string&, const glm::vec3&)const;
        void setMat4(const std::string&, const glm::mat4&)const;
    private: GLuint compile(GLenum, const std::string&); GLuint m_id{ 0 };
    };
}