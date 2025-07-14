#include "Engine/Shader.h"
#include <fstream>
#include <sstream>
#include <iostream>
using namespace cube;

static std::string readText(const std::string& f) {
    std::ifstream in(f, std::ios::in); if (!in) return {};
    std::stringstream ss; ss << in.rdbuf(); return ss.str();
}

GLuint Shader::compile(GLenum type, const std::string& src) {
    GLuint s = glCreateShader(type);
    const char* c = src.c_str(); glShaderSource(s, 1, &c, nullptr); glCompileShader(s);
    int ok; glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[512]; glGetShaderInfoLog(s, 512, nullptr, log);
        std::cerr << (type == GL_VERTEX_SHADER ? "[VS] " : "[FS] ") << log << "\n";
    }
    return s;
}

bool Shader::load(const std::string& vsf, const std::string& fsf) {
    std::string vsrc = readText(vsf), fsrc = readText(fsf);
    if (vsrc.empty() || fsrc.empty()) { std::cerr << "Shader: cannot open " << vsf << " or " << fsf << "\n"; return false; }
    GLuint vs = compile(GL_VERTEX_SHADER, vsrc), fs = compile(GL_FRAGMENT_SHADER, fsrc);
    m_id = glCreateProgram(); glAttachShader(m_id, vs); glAttachShader(m_id, fs); glLinkProgram(m_id);
    int ok; glGetProgramiv(m_id, GL_LINK_STATUS, &ok);
    if (!ok) { char log[512]; glGetProgramInfoLog(m_id, 512, nullptr, log); std::cerr << "[Link] " << log << "\n"; m_id = 0; }
    glDeleteShader(vs); glDeleteShader(fs);
    std::cout << "vertex OK / fragment OK / link OK\n";
    return m_id;
}