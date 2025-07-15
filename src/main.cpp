#include "Engine/Window.h"
#include "Engine/Renderer.h"
#include "Engine/Shader.h"
#include "Engine/World.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>

// Проверка, что вокруг позиции нет блоков (6 направлений + центр)
bool isFree(const cube::World& world, const glm::vec3& pos, float radius = 0.3f) {
    // Центр
    if (world.isBlockAt(int(floor(pos.x)), int(floor(pos.y)), int(floor(pos.z))))
        return false;
    // По 6 направлениям
    for (int axis = 0; axis < 3; ++axis) {
        for (int sign = -1; sign <= 1; sign += 2) {
            glm::vec3 offset(0.0f);
            offset[axis] = sign * radius;
            glm::vec3 p = pos + offset;
            if (world.isBlockAt(int(floor(p.x)), int(floor(p.y)), int(floor(p.z))))
                return false;
        }
    }
    return true;
}

// Перемещение камеры с учётом коллизии (поочерёдно по осям, учёт объёма камеры)
void moveWithCollision(cube::Camera& cam, const cube::World& world, const glm::vec3& delta) {
    glm::vec3 pos = cam.position();
    float radius = 0.3f;
    glm::vec3 tryPos = pos;
    // Двигаем по X
    tryPos.x += delta.x;
    if (isFree(world, tryPos, radius)) {
        pos.x = tryPos.x;
    }
    tryPos = pos;
    // Двигаем по Y
    tryPos.y += delta.y;
    if (isFree(world, tryPos, radius)) {
        pos.y = tryPos.y;
    }
    tryPos = pos;
    // Двигаем по Z
    tryPos.z += delta.z;
    if (isFree(world, tryPos, radius)) {
        pos.z = tryPos.z;
    }
    cam.setPosition(pos);
}

int main() {
    cube::Window win(1280, 720, "Cube World");
    if (!win.init()) return -1;

    cube::Camera cam(70.f, 1280.f / 720.f, 0.1f, 1000.f);
    win.captureCursor(true); win.attachCamera(&cam);

    cube::Renderer ren;
    cube::Shader shader;
    shader.load(std::string(SHADER_DIR) + "/basic.vert",
        std::string(SHADER_DIR) + "/basic.frag");

    cube::World world(1337);
    const int R = 3;

    int groundY = 10;
    for (int y = cube::CHUNK_SIZE - 1; y >= 0; --y) {
        if (world.isBlockAt(0, y, 0)) {
            groundY = y;
            break;
        }
    }
    cam.setPosition(glm::vec3(0, groundY + 2, 0)); // +2 — чтоб

    while (!win.shouldClose()) {
        float dt = win.deltaTime();
        float spd = 5.f * dt;
        glm::vec3 right = glm::normalize(glm::cross(cam.front(), glm::vec3(0, 1, 0)));
        if (glfwGetKey(win.native(), GLFW_KEY_W) == GLFW_PRESS) moveWithCollision(cam, world, spd * cam.front());
        if (glfwGetKey(win.native(), GLFW_KEY_S) == GLFW_PRESS) moveWithCollision(cam, world, -spd * cam.front());
        if (glfwGetKey(win.native(), GLFW_KEY_D) == GLFW_PRESS) moveWithCollision(cam, world, spd * right);
        if (glfwGetKey(win.native(), GLFW_KEY_A) == GLFW_PRESS) moveWithCollision(cam, world, -spd * right);

        int cx = int(floor(cam.position().x)) / cube::CHUNK_SIZE;
        int cz = int(floor(cam.position().z)) / cube::CHUNK_SIZE;
        world.ensureRadius(cx, cz, R);

        ren.clear(); shader.use();
        world.drawVisible(cx, cz, R, [&](const cube::ChunkKey& k, const cube::ChunkMesh& m) {
            glm::mat4 model = glm::translate(glm::mat4(1),
                glm::vec3(k.x * cube::CHUNK_SIZE, 0, k.z * cube::CHUNK_SIZE));
            glm::mat4 mvp = cam.projection() * cam.view() * model;
            glUniformMatrix4fv(glGetUniformLocation(shader.program(), "uMVP"),
                1, GL_FALSE, glm::value_ptr(mvp));
            m.draw();
            });

        win.poll();
    }
    return 0;
}