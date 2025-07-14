#include "Engine/Renderer.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

void cube::Renderer::clear()
{
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

