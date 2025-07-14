#include "Engine/Chunk.h"
using namespace cube;

Chunk::Chunk(int cx, int cy, int cz) :m_x(cx), m_y(cy), m_z(cz) {
    m_blocks.fill({ 0 });
}
Block& Chunk::at(int x, int y, int z) {
    return m_blocks[x + CHUNK_SIZE * (y + CHUNK_SIZE * z)];
}
const Block& Chunk::at(int x, int y, int z) const {
    return m_blocks[x + CHUNK_SIZE * (y + CHUNK_SIZE * z)];
}