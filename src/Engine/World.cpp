#include "Engine/World.h"
using namespace cube;

void World::generate(Chunk& ch) {
    float freq = 0.05f; int amp = 12;
    for (int z = 0; z < CHUNK_SIZE; ++z)
        for (int x = 0; x < CHUNK_SIZE; ++x) {
            int wx = ch.m_x * CHUNK_SIZE + x, wz = ch.m_z * CHUNK_SIZE + z;
            float h = glm::perlin(glm::vec2(wx, wz) * freq, glm::vec2(m_seed)) * 0.5f + 0.5f;
            int H = int(h * amp) + 4;
            for (int y = 0; y < CHUNK_SIZE && y < H; ++y) ch.at(x, y, z).id = 1;
        }
}

void World::ensureRadius(int cx, int cz, int R) {
    for (int dz = -R; dz <= R; ++dz)
        for (int dx = -R; dx <= R; ++dx) {
            ChunkKey k{ cx + dx,cz + dz };
            if (m_map.count(k)) continue;
            auto entry = std::make_unique<ChunkEntry>(k.x, k.z);
            generate(entry->chunk); entry->mesh.build(entry->chunk);
            m_map.emplace(k, std::move(entry));
        }
    for (auto it = m_map.begin(); it != m_map.end();) {
        int dx = it->first.x - cx, dz = it->first.z - cz;
        if (abs(dx) > R || abs(dz) > R) it = m_map.erase(it); else ++it;
    }
}

// Проверка наличия блока по мировым координатам
bool World::isBlockAt(int wx, int wy, int wz) const {
    if (wy < 0 || wy >= CHUNK_SIZE) return false; // только один слой чанков по y
    int cx = wx / CHUNK_SIZE;
    int cz = wz / CHUNK_SIZE;
    int lx = wx % CHUNK_SIZE;
    int lz = wz % CHUNK_SIZE;
    if (lx < 0) { cx -= 1; lx += CHUNK_SIZE; }
    if (lz < 0) { cz -= 1; lz += CHUNK_SIZE; }
    ChunkKey k{ cx, cz };
    auto it = m_map.find(k);
    if (it == m_map.end()) return false;
    const Chunk& chunk = it->second->chunk;
    return chunk.at(lx, wy, lz).id != 0;
}