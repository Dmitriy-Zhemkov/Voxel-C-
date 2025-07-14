#pragma once
#include <vector>
#include <array>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "Chunk.h"
namespace cube {
	struct Vertex { glm::vec3 pos, normal; };
class ChunkMesh { public: ~ChunkMesh(); void build(const Chunk&); void draw()const; bool hasMesh()const { return m_idx > 0 && m_vao; } private: GLuint m_vao{ 0 }, m_vbo{ 0 }, m_ebo{ 0 }; GLsizei m_idx{ 0 }; };
}