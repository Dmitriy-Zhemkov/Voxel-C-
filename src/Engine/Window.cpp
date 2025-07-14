#include "Engine/Window.h"
#include <iostream>
using namespace cube;
bool Window::init() {
    if (!glfwInit()) return false;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    m_win = glfwCreateWindow(m_w, m_h, m_title.c_str(), nullptr, nullptr);
    if (!m_win) { glfwTerminate(); return false; }
    glfwMakeContextCurrent(m_win);
    glewExperimental = GL_TRUE;
    glewInit();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glfwSetWindowUserPointer(m_win, this);
    glfwSetCursorPosCallback(m_win, cursorCB);
    return true;
}
void Window::shutdown() { if (m_win) { glfwDestroyWindow(m_win); glfwTerminate(); m_win = nullptr; } }
void Window::captureCursor(bool c) {
    glfwSetInputMode(m_win, GLFW_CURSOR, c ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
    firstMouse = true;
}
float Window::deltaTime() { double t = glfwGetTime(); float dt = float(t - lastFrame); lastFrame = t; return dt; }
void Window::cursorCB(GLFWwindow* w, double x, double y) {
    auto* self = static_cast<Window*>(glfwGetWindowUserPointer(w));
    if (!self || !self->m_cam) return;
    if (self->firstMouse) { self->lastX = x; self->lastY = y; self->firstMouse = false; }
    float xoff = float(x - self->lastX), yoff = float(self->lastY - y);
    self->lastX = x; self->lastY = y;
    self->m_cam->addYawPitch(xoff * 0.1f, yoff * 0.1f);
}