#include "Engine/Window.h"
#include "Engine/Renderer.h"
#include "Engine/Shader.h"
#include "Engine/World.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>

// Проверка точки в мире на коллизию
bool isPointBlocked(const cube::World& world, const glm::vec3& point) {
    return world.isBlockAt(int(floor(point.x)), int(floor(point.y)), int(floor(point.z)));
}

// Проверка, что вокруг позиции нет блоков (включая голову и точку перед камерой)
bool isFree(const cube::World& world, const glm::vec3& pos, const glm::vec3& front, float radius = 0.49f) {
    // Проверка центра
    if (isPointBlocked(world, pos))
        return false;

    // Проверка "головы" (точка выше центра)
    if (isPointBlocked(world, pos + glm::vec3(0.0f, radius, 0.0f)))
        return false;

    // Проверка "ног" (точка ниже центра)
    if (isPointBlocked(world, pos + glm::vec3(0.0f, -radius, 0.0f)))
        return false;

    // Проверка нескольких точек перед камерой на разном расстоянии
    for (float dist = 0.1f; dist <= 0.5f; dist += 0.1f) {
        if (isPointBlocked(world, pos + front * dist))
            return false;
    }

    // Проверка по шести направлениям
    for (int axis = 0; axis < 3; ++axis) {
        for (int sign = -1; sign <= 1; sign += 2) {
            glm::vec3 offset(0.0f);
            offset[axis] = sign * radius;
            if (isPointBlocked(world, pos + offset))
                return false;
        }
    }
    return true;
}

// Перемещение камеры с учётом коллизии (сначала диагонально, потом по осям)
void moveWithCollision(cube::Camera& cam, const cube::World& world, const glm::vec3& delta) {
    glm::vec3 pos = cam.position();
    float radius = 0.49f;
    glm::vec3 tryPos = pos + delta;
    // Сначала пробуем двигаться по всем осям сразу (диагонально)
    if (isFree(world, tryPos, cam.front(), radius)) {
        cam.setPosition(tryPos);
        return;
    }
    // Если не получилось — по отдельным осям
    tryPos = pos;

    // X axis
    tryPos.x += delta.x;
    if (isFree(world, tryPos, cam.front(), radius)) {
        pos.x = tryPos.x;
    }
    tryPos = pos;

    // Y axis (обычно для прыжков/падения)
    tryPos.y += delta.y;
    if (isFree(world, tryPos, cam.front(), radius)) {
        pos.y = tryPos.y;
    }
    tryPos = pos;

    // Z axis
    tryPos.z += delta.z;
    if (isFree(world, tryPos, cam.front(), radius)) {
        pos.z = tryPos.z;
    }

    cam.setPosition(pos);
}

int main() {
    cube::Window win(1280, 720, "Cube World");
    if (!win.init()) return -1;

    // Увеличено значение near clip plane с 0.1f до 0.3f
    cube::Camera cam(70.f, 1280.f / 720.f, 0.3f, 1000.f);
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
    cam.setPosition(glm::vec3(0, groundY + 1.7f, 0)); // 1.7 - примерная высота глаз

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