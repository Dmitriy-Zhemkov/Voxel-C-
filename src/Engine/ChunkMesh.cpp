#include "Engine/ChunkMesh.h"
using namespace cube;

// нормали (float-литералы)
static const std::array<glm::vec3, 6> N = {
    glm::vec3(0.0f,  0.0f,  1.0f),   // +Z
    glm::vec3(0.0f,  0.0f, -1.0f),   // -Z
    glm::vec3(1.0f,  0.0f,  0.0f),   // +X
    glm::vec3(-1.0f,  0.0f,  0.0f),   // -X
    glm::vec3(0.0f,  1.0f,  0.0f),   // +Y
    glm::vec3(0.0f, -1.0f,  0.0f)    // -Y
};

// вершины граней (float-литералы)
static const std::array<std::array<glm::vec3, 4>, 6> V = { {
        // +Z
        {{ {0.0f,0.0f,1.0f}, {1.0f,0.0f,1.0f}, {1.0f,1.0f,1.0f}, {0.0f,1.0f,1.0f} }},
        // -Z
        {{ {0.0f,0.0f,0.0f}, {1.0f,0.0f,0.0f}, {1.0f,1.0f,0.0f}, {0.0f,1.0f,0.0f} }},
        // +X
        {{ {1.0f,0.0f,1.0f}, {1.0f,0.0f,0.0f}, {1.0f,1.0f,0.0f}, {1.0f,1.0f,1.0f} }},
        // -X
        {{ {0.0f,0.0f,0.0f}, {0.0f,0.0f,1.0f}, {0.0f,1.0f,1.0f}, {0.0f,1.0f,0.0f} }},
        // +Y
        {{ {0.0f,1.0f,1.0f}, {1.0f,1.0f,1.0f}, {1.0f,1.0f,0.0f}, {0.0f,1.0f,0.0f} }},
        // -Y
        {{ {0.0f,0.0f,1.0f}, {1.0f,0.0f,1.0f}, {1.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f} }}
    } };

void ChunkMesh::build(const Chunk& ch)
{
    /*--- очистка старых буферов (если меш уже был) ---*/
    if (m_vao) {
        glDeleteBuffers(1, &m_ebo);
        glDeleteBuffers(1, &m_vbo);
        glDeleteVertexArrays(1, &m_vao);
        m_vao = m_vbo = m_ebo = 0;
    }

    std::vector<Vertex>        vs;
    std::vector<unsigned int>  is;

    /* лямбда для чтения id блока, вне границ считаем воздухом */
    auto idAt = [&](int x, int y, int z)->uint8_t {
        if (x < 0 || x >= CHUNK_SIZE ||
            y < 0 || y >= CHUNK_SIZE ||
            z < 0 || z >= CHUNK_SIZE)
            return 0;
        return ch.at(x, y, z).id;
        };

    /*--- проходим все блоки чанка ---*/
    for (int z = 0; z < CHUNK_SIZE; ++z)
        for (int y = 0; y < CHUNK_SIZE; ++y)
            for (int x = 0; x < CHUNK_SIZE; ++x)
            {
                if (ch.at(x, y, z).id == 0) continue;        // воздух

                /* 6 граней */
                for (int f = 0; f < 6; ++f)
                {
                    int nx = x + int(N[f].x);
                    int ny = y + int(N[f].y);
                    int nz = z + int(N[f].z);
                    if (idAt(nx, ny, nz) != 0) continue;    // сосед заполнен ⇒ грань скрыта

                    unsigned base = static_cast<unsigned>(vs.size());
                    /* добавляем 4 вершины грани */
                    for (int v = 0; v < 4; ++v)
                        vs.push_back({ glm::vec3(x,y,z) + V[f][v], N[f] });

                    /* два треугольника — CCW, с учётом «flip» для –Z, –X, –Y */
                    bool flip = (f == 1) || (f == 3) || (f == 5);   // -Z -X -Y
                    if (!flip)
                        is.insert(is.end(), { base, base + 1, base + 2,
                                             base, base + 2, base + 3 });
                    else
                        is.insert(is.end(), { base, base + 2, base + 1,
                                             base, base + 3, base + 2 });
                }
            }

    m_idx = static_cast<GLsizei>(is.size());
    if (!m_idx) return;               // чанк пуст

    /*--- загрузка в GPU ---*/
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER,
        vs.size() * sizeof(Vertex), vs.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &m_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        is.size() * sizeof(unsigned), is.data(), GL_STATIC_DRAW);

    /* атрибуты: 0-позиция, 1-нормаль */
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
        sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
        sizeof(Vertex), (void*)sizeof(glm::vec3));

    glBindVertexArray(0);
}

ChunkMesh::~ChunkMesh()
{
    if (m_ebo) glDeleteBuffers(1, &m_ebo);
    if (m_vbo) glDeleteBuffers(1, &m_vbo);
    if (m_vao) glDeleteVertexArrays(1, &m_vao);
}

void ChunkMesh::draw() const
{
    if (!m_vao || !m_idx) return;
    glBindVertexArray(m_vao);
    glDrawElements(GL_TRIANGLES, m_idx, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}