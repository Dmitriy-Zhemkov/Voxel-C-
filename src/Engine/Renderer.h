#pragma once
namespace cube {
class Renderer {
public:
    void clear();
    void present(); // пока просто свап буферов
};
} // namespace cube