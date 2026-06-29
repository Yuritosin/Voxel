#ifndef WINDOW_H
#define WINDOW_H

#include <GL/glew.h>

#include <GLFW/glfw3.h>

enum class WindowFpsState {
	UNLOCKED, LOCKED, VSYNC
};

class Window {
public:
    ~Window();
    Window() = default;

    bool Create(const int width, const int height, const char* title);
    void Destroy();

    void KeyPressed(const int key);
    void KeyReleased(const int key);

    void ToggleFpsState();

    bool IsVsync() const;

    GLFWwindow* GetGlfw() const;
    WindowFpsState GetFpsState() const;
private:
    GLFWwindow* window = nullptr;

    WindowFpsState m_FpsState = WindowFpsState::LOCKED;
};

#endif
