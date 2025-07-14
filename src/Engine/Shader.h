#pragma once
#include <string>
#include <GL/glew.h>

namespace cube {
    class Shader {
    public:
        bool load(const std::string& vs, const std::string& fs);
        void use() const { if (m_id) glUseProgram(m_id); }
        GLuint program() const { return m_id; }      // ← другое имя
    private:
        GLuint compile(GLenum, const std::string&);
        GLuint m_id{ 0 };                                     // переменная
    };
}