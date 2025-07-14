//  Engine/Chunk.h
#pragma once
#include <array>
#include <cstdint>

namespace cube {
    constexpr int CHUNK_SIZE = 16;
    struct Block { uint8_t id{ 0 }; };

    class Chunk {
    public:
        Chunk(int cx, int cy, int cz);                 // ← тело убрать в .cpp
        Block& at(int x, int y, int z);
        const Block& at(int x, int y, int z) const;

        int m_x, m_y, m_z;                             // доступен генератору
    private:
        std::array<Block, CHUNK_SIZE* CHUNK_SIZE* CHUNK_SIZE> m_blocks;
    };
}
