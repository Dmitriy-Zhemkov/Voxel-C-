//  src/Engine/World.h
#pragma once
#include <unordered_map>
#include <memory>
#include <glm/gtc/noise.hpp>
#include "Chunk.h"
#include "ChunkMesh.h"

namespace cube {

    struct ChunkKey {
        int x, z;
        bool operator==(const ChunkKey& o) const { return x == o.x && z == o.z; }
    };
    struct ChunkKeyHash {
        size_t operator()(const ChunkKey& k) const {
            return std::hash<int>()((k.x << 16) ^ k.z);
        }
    };

    struct ChunkEntry {
        ChunkEntry(int cx, int cz) : chunk(cx, 0, cz) {}
        Chunk      chunk;
        ChunkMesh  mesh;
    };

    class World {
    public:
        explicit World(int seed = 0) : m_seed(seed) {}

        void ensureRadius(int cx, int cz, int R);

        /** рисуем все чанки в квадрате (2R+1)×(2R+1) вокруг (cx,cz) */
        template<class Fn>
        void drawVisible(int cx, int cz, int R, Fn fn);

    private:
        void generate(Chunk&);

        std::unordered_map<ChunkKey,
            std::unique_ptr<ChunkEntry>,
            ChunkKeyHash> m_map;
        int m_seed;
    };

    /* ---------- inline-реализация шаблона ---------- */
    template<class Fn>
    void World::drawVisible(int cx, int cz, int R, Fn fn)
    {
        for (int dz = -R; dz <= R; ++dz)
            for (int dx = -R; dx <= R; ++dx)
            {
                ChunkKey k{ cx + dx, cz + dz };
                auto it = m_map.find(k);
                if (it != m_map.end() && it->second->mesh.hasMesh())
                    fn(k, it->second->mesh);
            }
    }

} // namespace cube
