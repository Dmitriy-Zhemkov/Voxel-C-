#include "Engine/ChunkMesh.h"
using namespace cube;

static const std::array<glm::vec3, 6> N = {
    glm::vec3(0,0,1), glm::vec3(0,0,-1),
    glm::vec3(1,0,0), glm::vec3(-1,0,0),
    glm::vec3(0,1,0), glm::vec3(0,-1,0) };

static const std::array<std::array<glm::vec3, 4>, 6> V = { {
  {{ {0,0,1},{1,0,1},{1,1,1},{0,1,1} }},   // +Z
  {{ {0,0,0},{1,0,0},{1,1,0},{0,1,0} }},   // -Z
  {{ {1,0,1},{1,0,0},{1,1,0},{1,1,1} }},   // +X
  {{ {0,0,0},{0,0,1},{0,1,1},{0,1,0} }},   // -X
  {{ {0,1,1},{1,1,1},{1,1,0},{0,1,0} }},   // +Y
  {{ {0,0,1},{1,0,1},{1,0,0},{0,0,0} }}    // -Y
} };

ChunkMesh::~ChunkMesh() {
    if (m_ebo) glDeleteBuffers(1, &m_ebo);
    if (m_vbo) glDeleteBuffers(1, &m_vbo);
    if (m_vao) glDeleteVertexArrays(1, &m_vao);
}

void ChunkMesh::build(const Chunk& ch) {
    if (m_vao) {
        glDeleteBuffers(1, &m_ebo); glDeleteBuffers(1, &m_vbo);
        glDeleteVertexArrays(1, &m_vao); m_vao = m_vbo = m_ebo = 0;
    }

    std::vector<Vertex> vs; std::vector<unsigned> is;

    auto idAt = [&](int x, int y, int z)->uint8_t {
        if (x < 0 || x >= CHUNK_SIZE || y < 0 || y >= CHUNK_SIZE || z < 0 || z >= CHUNK_SIZE) return 0;
        return ch.at(x, y, z).id;
        };

    for (int z = 0; z < CHUNK_SIZE; ++z)
        for (int y = 0; y < CHUNK_SIZE; ++y)
            for (int x = 0; x < CHUNK_SIZE; ++x) {
                if (ch.at(x, y, z).id == 0) continue;
                for (int f = 0; f < 6; ++f) {
                    int nx = x + int(N[f].x), ny = y + int(N[f].y), nz = z + int(N[f].z);
                    if (idAt(nx, ny, nz) != 0) continue;
                    unsigned base = vs.size();
                    for (int v = 0; v < 4; ++v) vs.push_back({ glm::vec3(x,y,z) + V[f][v],N[f] });
                    is.insert(is.end(), { base,base + 1,base + 2, base,base + 2,base + 3 });

                    bool flip = (f == 1) || (f == 3) || (f == 5); // -Z, -X, -Y
                    if (!flip)
                        is.insert(is.end(), { base, base + 1, base + 2,
                                              base, base + 2, base + 3 });
                    else
                        is.insert(is.end(), { base, base + 2, base + 1,
                                              base, base + 3, base + 2 });
                }
            }

    m_idx = is.size(); if (!m_idx) return;

    /* ---------- загрузка в GPU ---------- */
    glGenVertexArrays(1, &m_vao); glBindVertexArray(m_vao);
    glGenBuffers(1, &m_vbo); glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, vs.size() * sizeof(Vertex), vs.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &m_ebo); glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, is.size() * sizeof(unsigned), is.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)sizeof(glm::vec3));
    glBindVertexArray(0);
}

void ChunkMesh::draw()const {
    if (!m_vao || !m_idx) return;
    glBindVertexArray(m_vao);
    glDrawElements(GL_TRIANGLES, m_idx, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}