#include "Engine/Window.h"
#include "Engine/Renderer.h"
#include "Engine/Shader.h"
#include "Engine/World.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>   // для glm::translate
#include <cmath>                          // cos/sin

int main() {
    cube::Window win(1280, 720, "Cube World");
    if (!win.init()) return -1;

    cube::Camera cam(70.f, 1280.f / 720.f, 0.1f, 1000.f);
    win.captureCursor(true);
    win.attachCamera(&cam);

    cube::Renderer ren;
    cube::Shader chunkShader;
    chunkShader.load(std::string(SHADER_DIR) + "/chunk.vert",
        std::string(SHADER_DIR) + "/chunk.frag");

    cube::Shader skyShader;
    skyShader.load(std::string(SHADER_DIR) + "/sky.vert",
        std::string(SHADER_DIR) + "/sky.frag");

    /* ----------  skybox VAO (один раз)  ---------- */
    GLuint skyVAO{}, skyVBO{}, skyEBO{};
    float skyVerts[] = { -1,-1,-1,  1,-1,-1,  1, 1,-1, -1, 1,-1,
                         -1,-1, 1,  1,-1, 1,  1, 1, 1, -1, 1, 1 };
    unsigned skyIdx[] = { 0,1,2,0,2,3, 1,5,6,1,6,2, 5,4,7,5,7,6,
                          4,0,3,4,3,7, 3,2,6,3,6,7, 4,5,1,4,1,0 };
    glGenVertexArrays(1, &skyVAO);
    glGenBuffers(1, &skyVBO);
    glGenBuffers(1, &skyEBO);
    glBindVertexArray(skyVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyVerts), skyVerts, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(skyIdx), skyIdx, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindVertexArray(0);

    /* ----------  мир и время  ---------- */
    cube::World world(1337);
    const int   R = 3;            // радиус чанков
    float       timeOfDay = 0.0f; // радианы

    /* --------------- цикл --------------- */
    while (!win.shouldClose()) {
        float dt = win.deltaTime();

        /* — движение солнца — */
        timeOfDay += dt * 0.1f;                        // ~1 оборот / 62 с
        glm::vec3 sunDir = glm::normalize(
            glm::vec3(cos(timeOfDay), sin(timeOfDay), 0.3f));
        if (sunDir.y < 0.05f) sunDir.y = 0.05f;

        glm::vec3 ambient = glm::mix(glm::vec3(0.05f),
            glm::vec3(0.30f),
            glm::smoothstep(0.05f, 1.0f, sunDir.y));

        /* — обработка ввода (WASD) — */
        float speed = 6.0f * dt;
        glm::vec3 right = glm::normalize(glm::cross(cam.front(), { 0,1,0 }));
        auto* w = win.native();
        if (glfwGetKey(w, GLFW_KEY_W) == GLFW_PRESS) cam.move(speed * cam.front());
        if (glfwGetKey(w, GLFW_KEY_S) == GLFW_PRESS) cam.move(-speed * cam.front());
        if (glfwGetKey(w, GLFW_KEY_D) == GLFW_PRESS) cam.move(speed * right);
        if (glfwGetKey(w, GLFW_KEY_A) == GLFW_PRESS) cam.move(-speed * right);

        /* — загрузка чанков — */
        int cx = int(std::floor(cam.position().x)) / cube::CHUNK_SIZE;
        int cz = int(std::floor(cam.position().z)) / cube::CHUNK_SIZE;
        world.ensureRadius(cx, cz, R);

        /* матрицы вида / проекции */
        glm::mat4 proj = cam.proj();
        glm::mat4 view = cam.view();
        glm::mat4 viewNoTranslate = glm::mat4(glm::mat3(view)); // только поворот

        /* — отрисовка мира — */
        ren.clear();
        chunkShader.use();
        chunkShader.setVec3("uSunDirection", sunDir);
        chunkShader.setVec3("uAmbientColor", ambient);
        chunkShader.setFloat("uTimeOfDay", timeOfDay);

        world.drawVisible(cx, cz, R,
            [&](const cube::ChunkKey& k, const cube::ChunkMesh& m) {
                glm::mat4 model = glm::translate(glm::mat4(1),
                    glm::vec3(k.x * cube::CHUNK_SIZE, 0, k.z * cube::CHUNK_SIZE));
                glm::mat4 mvp = proj * view * model;
                chunkShader.setMat4("uMVP", mvp);
                m.draw();
            });

        /* — skybox — */
        glDepthMask(GL_FALSE);
        skyShader.use();
        skyShader.setVec3("uSunDirection", sunDir);
        skyShader.setMat4("uView", viewNoTranslate);
        skyShader.setMat4("uProjection", proj);
        glBindVertexArray(skyVAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
        glDepthMask(GL_TRUE);

        win.poll();
    }
    return 0;
}
