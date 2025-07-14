#include "Engine/Window.h"
#include "Engine/Renderer.h"
#include "Engine/Shader.h"
#include "Engine/World.h"
#include <glm/gtc/type_ptr.hpp>

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

    while (!win.shouldClose()) {
        float dt = win.deltaTime();
        float spd = 5.f * dt;
        glm::vec3 right = glm::normalize(glm::cross(cam.front(), glm::vec3(0, 1, 0)));
        if (glfwGetKey(win.native(), GLFW_KEY_W) == GLFW_PRESS) cam.move(spd * cam.front());
        if (glfwGetKey(win.native(), GLFW_KEY_S) == GLFW_PRESS) cam.move(-spd * cam.front());
        if (glfwGetKey(win.native(), GLFW_KEY_D) == GLFW_PRESS) cam.move(spd * right);
        if (glfwGetKey(win.native(), GLFW_KEY_A) == GLFW_PRESS) cam.move(-spd * right);

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