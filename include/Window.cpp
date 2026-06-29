#include "Window.h"

#include <iostream>

Window::~Window() {
    Destroy();
}

bool Window::Create(const int width, const int height, const char* title) {
    if (!glfwInit()) {
        std::cerr << "Unable to init GLFW" << '\n';
        return false;
    }

    // glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    window = glfwCreateWindow(width, height, title, nullptr, nullptr);

    glfwSetWindowSizeCallback(window, [](GLFWwindow* window, const int width, const int height){
        std::cout << "Window resized: [width new: " << width << ", height new: " << height << "]\n";
        glViewport(0, 0, width, height);
    });

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwMakeContextCurrent(window);

    glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
    // glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    // glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
    // glClearColor(0.56f, 0.8f, 0.95f, 1.0f);
    // glClearColor(0.25f, 0.40f, 0.48f, 1.0f);

    glewExperimental = GL_TRUE;
    // const GLenum state = glewInit();
    const GLenum state = glewContextInit();
    if (state != GLEW_OK) {
        std::cerr << "Unable to init GLEW: " << glewGetErrorString(state) << '\n';
        return false;
    }

    glfwSwapInterval(IsVsync());
    glfwShowWindow(window);

    return true;
}

void Window::KeyPressed(const int key) {
    if (key == GLFW_KEY_ESCAPE) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    if (key == GLFW_KEY_F9) {
        ToggleFpsState();
    }
}
void Window::KeyReleased(const int key) {}

void Window::ToggleFpsState() {
	if (m_FpsState == WindowFpsState::UNLOCKED) {
		m_FpsState = WindowFpsState::LOCKED;
	} else if (m_FpsState == WindowFpsState::LOCKED) {
		m_FpsState = WindowFpsState::VSYNC;
	} else {
		m_FpsState = WindowFpsState::UNLOCKED;
	}
	glfwSwapInterval(IsVsync());
}

void Window::Destroy() {
    if (window) {
        glfwDestroyWindow(window);
        window = nullptr;
        glfwTerminate();
    }
}

bool Window::IsVsync() const {
	return m_FpsState == WindowFpsState::VSYNC;
}

GLFWwindow* Window::GetGlfw() const {
    return window;
}
WindowFpsState Window::GetFpsState() const {
	return m_FpsState;
}
