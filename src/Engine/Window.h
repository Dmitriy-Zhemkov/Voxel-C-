#pragma once
#include <string>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Camera.h"

namespace cube {
    class Window {
    public:
        Window(int w, int h, const std::string& t) :m_w(w), m_h(h), m_title(t) {}
        ~Window() { shutdown(); }

        bool init();
        void shutdown();
        bool shouldClose()const { return glfwWindowShouldClose(m_win); }
        void poll() { glfwPollEvents(); glfwSwapBuffers(m_win); }
        float deltaTime();
        GLFWwindow* native() { return m_win; }

        void captureCursor(bool c);
        void attachCamera(Camera* c) { m_cam = c; }
    private:
        static void cursorCB(GLFWwindow*, double, double);
        int m_w, m_h; std::string m_title;
        GLFWwindow* m_win{ nullptr };
        Camera* m_cam{ nullptr };
        bool firstMouse{ true }; double lastX{}, lastY{}, lastFrame{};
    };
}