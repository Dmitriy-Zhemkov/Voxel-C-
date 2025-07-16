#include "Engine/Window.h"
#include "Engine/Renderer.h"
#include "Engine/Shader.h"
#include "Engine/World.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>
#include <algorithm>
#include <iostream>

struct Player {
    glm::vec3 pos;
    float yaw = -90.f;
    float pitch = 0.f;
    float velocityY = 0.f;
    bool onGround = false;
    glm::vec3 front() const {
        float yawRad = glm::radians(yaw);
        float pitchRad = glm::radians(pitch);
        return glm::normalize(glm::vec3(
            cos(pitchRad) * cos(yawRad),
            sin(pitchRad),
            cos(pitchRad) * sin(yawRad)
        ));
    }
};

bool isPointBlocked(const cube::World& world, const glm::vec3& point) {
    return world.isBlockAt(int(floor(point.x)), int(floor(point.y)), int(floor(point.z)));
}

bool isFree(const cube::World& world, const glm::vec3& pos, float radius = 0.49f) {
    if (isPointBlocked(world, pos))
        return false;
    if (isPointBlocked(world, pos + glm::vec3(0.0f, radius, 0.0f)))
        return false;
    if (isPointBlocked(world, pos + glm::vec3(0.0f, -radius, 0.0f)))
        return false;
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

void movePlayerWithCollision(Player& player, const cube::World& world, const glm::vec3& delta) {
    glm::vec3 pos = player.pos;
    float radius = 0.49f;
    glm::vec3 tryPos = pos + delta;
    if (isFree(world, tryPos, radius)) {
        player.pos = tryPos;
        return;
    }
    tryPos = pos;
    tryPos.x += delta.x;
    if (isFree(world, tryPos, radius)) {
        pos.x = tryPos.x;
    }
    tryPos = pos;
    tryPos.y += delta.y;
    if (isFree(world, tryPos, radius)) {
        pos.y = tryPos.y;
    }
    tryPos = pos;
    tryPos.z += delta.z;
    if (isFree(world, tryPos, radius)) {
        pos.z = tryPos.z;
    }
    player.pos = pos;
}

bool isOnGround(const cube::World& world,
    const glm::vec3& pos,
    float radius = 0.49f)
{
    // Чуть ниже низа сферы
    const float yProbe = -(radius + 0.05f);

    // Радиусы для выборки
    const float c = radius * 0.8f;          // осевые
    const float d = c * 0.70710678f;        // диагонали (~sqrt(2)/2)

    static const glm::vec3 probe[] = {
        { 0,  yProbe,  0 },
        {  c, yProbe,  0 }, { -c, yProbe, 0 },
        {  0, yProbe,  c }, {  0, yProbe,-c },
        {  d, yProbe,  d }, { -d, yProbe, d },
        {  d, yProbe, -d }, { -d, yProbe,-d }
    };

    for (auto o : probe)
        if (isPointBlocked(world, pos + o))
            return true;
    return false;
}

int main() {
    cube::Window win(1280, 720, "Cube World");
    if (!win.init()) return -1;

    glEnable(GL_DEPTH_TEST);

    const float cameraDistance = 5.0f;
    const float cameraHeight = 2.5f;
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
    Player player;
    player.pos = glm::vec3(0, groundY + 1.7f, 0);

    double lastX = 0, lastY = 0;
    bool firstMouse = true;

    bool ground = isOnGround(world, player.pos, 0.49f);

    while (!win.shouldClose()) {
        float dt = win.deltaTime();
        if (dt > 0.1f) dt = 0.1f; // ограничение шага физики
        float spd = 5.f * dt;
        float gravity = 18.0f;
        float jumpVel = 8.0f;

        // --- Управление мышью (yaw/pitch) ---
        double dx = 0, dy = 0;
        glfwGetCursorPos(win.native(), &dx, &dy);
        if (firstMouse) { lastX = dx; lastY = dy; firstMouse = false; }
        float sens = 0.15f;
        player.yaw += float(dx - lastX) * sens;
        player.pitch -= float(dy - lastY) * sens;
        player.pitch = std::clamp(player.pitch, -89.f, 89.f);
        lastX = dx; lastY = dy;

        // --- Движение по WASD ---
        glm::vec3 forward = player.front();
        glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0, 1, 0)));
        glm::vec3 move(0.0f);
        if (glfwGetKey(win.native(), GLFW_KEY_W) == GLFW_PRESS) move += glm::normalize(glm::vec3(forward.x, 0, forward.z)) * spd;
        if (glfwGetKey(win.native(), GLFW_KEY_S) == GLFW_PRESS) move -= glm::normalize(glm::vec3(forward.x, 0, forward.z)) * spd;
        if (glfwGetKey(win.native(), GLFW_KEY_D) == GLFW_PRESS) move += right * spd;
        if (glfwGetKey(win.native(), GLFW_KEY_A) == GLFW_PRESS) move -= right * spd;

        // --- Физика: гравитация и прыжки ---
        player.velocityY -= gravity * dt;
        if (player.velocityY < -gravity) player.velocityY = -gravity;
        glm::vec3 velMove = move;
        velMove.y = player.velocityY * dt;
        float prevY = player.pos.y;
        movePlayerWithCollision(player, world, velMove);
        // Проверка на землю (только центр)
        bool ground = isOnGround(world, player.pos);
        player.onGround = ground;
        if (ground) {
            player.velocityY = 0.f;
            player.pos.y = floor(player.pos.y) + 0.5f;
        }
        // Отладочный вывод (обновление одной строки)
        std::cout << "\rpos: " << player.pos.x << ", " << player.pos.y << ", " << player.pos.z << " onGround: " << ground << std::flush;
        // Прыжок
        if (player.onGround && glfwGetKey(win.native(), GLFW_KEY_SPACE) == GLFW_PRESS) {
            player.velocityY = jumpVel;
            player.onGround = false;
        }

        // --- Камера третьего лица ---
        glm::vec3 camTarget = player.pos + glm::vec3(0, 0.7f, 0);
        glm::vec3 camBack = -player.front();
        glm::vec3 camPos = camTarget + camBack * cameraDistance + glm::vec3(0, cameraHeight, 0);
        // Raycast: не позволять камере попадать внутрь блоков
        glm::vec3 dir = glm::normalize(camPos - camTarget);
        float maxDist = glm::length(camPos - camTarget);
        float safeDist = maxDist;
        for (float t = 0.0f; t < maxDist; t += 0.2f) {
            glm::vec3 p = camTarget + dir * t;
            if (!isFree(world, p, 0.3f)) { safeDist = t - 0.2f; break; }
        }
        camPos = camTarget + dir * std::max(safeDist, 1.0f);
        cam.setPosition(camPos);
        cam.addYawPitch(0, 0);
        cam.setFront(glm::normalize(camTarget - camPos));

        int cx = int(floor(player.pos.x)) / cube::CHUNK_SIZE;
        int cz = int(floor(player.pos.z)) / cube::CHUNK_SIZE;
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

        // Нарисовать игрока (ручной куб OpenGL, совместимость)
        glUseProgram(0);
        glMatrixMode(GL_PROJECTION);
        glLoadMatrixf(glm::value_ptr(cam.projection()));
        glMatrixMode(GL_MODELVIEW);
        glm::mat4 mv = cam.view() * glm::translate(glm::mat4(1), player.pos);
        glLoadMatrixf(glm::value_ptr(mv));
        glColor3f(1, 0, 0);
        float s = 0.5f;
        glBegin(GL_QUADS);
        // Нижняя грань
        glVertex3f(-s, -s, -s); glVertex3f(s, -s, -s); glVertex3f(s, -s, s); glVertex3f(-s, -s, s);
        // Верхняя грань
        glVertex3f(-s, s, -s); glVertex3f(s, s, -s); glVertex3f(s, s, s); glVertex3f(-s, s, s);
        // Передняя грань
        glVertex3f(-s, -s, s); glVertex3f(s, -s, s); glVertex3f(s, s, s); glVertex3f(-s, s, s);
        // Задняя грань
        glVertex3f(-s, -s, -s); glVertex3f(s, -s, -s); glVertex3f(s, s, -s); glVertex3f(-s, s, -s);
        // Левая грань
        glVertex3f(-s, -s, -s); glVertex3f(-s, -s, s); glVertex3f(-s, s, s); glVertex3f(-s, s, -s);
        // Правая грань
        glVertex3f(s, -s, -s); glVertex3f(s, -s, s); glVertex3f(s, s, s); glVertex3f(s, s, -s);
        glEnd();
        // Восстановить шейдер и матрицы для следующего кадра
        shader.use();
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        win.poll();
    }
    return 0;
}