#include "Game.h"

#include <random>
#include <chrono>
#include <limits>

#include <iostream>

#include "Shader.h"

#include "options/Options.h"

struct GameInputData {
public:
    GameInputData(Game* game, World* world, Window* window)
        : m_Game(game), m_World(world), m_Window(window) {}

    Game* GetGame() const {
        return m_Game;
    }
    World* GetWorld() const {
        return m_World;
    }
    Window* GetWindow() const {
        return m_Window;
    }
    bool IsCorrect() const {
        return (GetGame() && GetWorld() && GetWindow());
    }
private:
    Game* m_Game = nullptr;
    World* m_World = nullptr;
    Window* m_Window = nullptr;
};

static void RegisterGlobalKeyEvent(const GameInputData& inputData);
static void RegisterGlobalMousePosEvent(const GameInputData& inputData);
static void RegisterGlobalMouseButtonClickEvent(const GameInputData& inputData);
static void RegisterGlobalMouseScrollEvent(const GameInputData& inputData);

static int generateInt() {
    std::mt19937 rnd(
        std::chrono::steady_clock::now().time_since_epoch().count()
    );
    std::uniform_int_distribution<int> dist(
        std::numeric_limits<int>::min(),
        std::numeric_limits<int>::max()
    );
    return dist(rnd);
}

Game::Game(const int seed): m_World(seed) {}

void Game::RunLocked() {
    running = true;

    const bool state = m_MainWindow.Create(
        1024, 1024 / 16 * 9, "C++ Minecraft"
        // 1440, 1440 / 16 * 9, "C++ Minecraft"
    );
    if (state) {
        GameInputData inputData = {
            this,
            &m_World,
            &m_MainWindow
        };
        glfwSetWindowUserPointer(inputData.GetWindow()->GetGlfw(), &inputData);
        RegisterGlobalKeyEvent(inputData);
        RegisterGlobalMousePosEvent(inputData);
        RegisterGlobalMouseButtonClickEvent(inputData);
        RegisterGlobalMouseScrollEvent(inputData);

        const auto glfwWindow = m_MainWindow.GetGlfw();
        if (!glfwWindow) return;

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);

        Shader::LoadCompileAll();

        m_World.Init(m_MainWindow);

        int fps = 0;
        int ups = 0;

        constexpr double UPDATE_TARGET_CUP = 1.0 / UPDATES_PER_SECOND;
        constexpr double FRAME_TARGET_CUP = 1.0 / FRAMES_PER_SECOND;

        double timer = glfwGetTime();

        double prevFrameTime = glfwGetTime();

        double elapsed = 0.0;
        double prevTime = glfwGetTime();
        double currentTime = 0.0;
        while(!glfwWindowShouldClose(glfwWindow)) {
			if (m_MainWindow.GetFpsState() == WindowFpsState::LOCKED) {
				const double nowFrameTime = glfwGetTime();
				if (nowFrameTime - prevFrameTime < FRAME_TARGET_CUP) {
					continue;
				} else {
					prevFrameTime = nowFrameTime;
				}
			}

            currentTime = glfwGetTime();
            elapsed += (currentTime - prevTime);
            prevTime = currentTime;

            while (elapsed >= UPDATE_TARGET_CUP) {
                m_World.Update();
                ups ++;
                elapsed -= UPDATE_TARGET_CUP;
            }

            m_World.Draw(elapsed / UPDATE_TARGET_CUP, m_MainWindow);
            fps ++;

            if (glfwGetTime() - timer >= 1.0) {
                timer++;
                if (PERFOMANCE_OUTPUT) {
                    std::cout << "FPS: " << fps << " | "
                        << "UPS: " << ups << " "
                        << "(VSync: " << (m_MainWindow.IsVsync() ? "On" : "Off")
						<< ", FPS-Lock: " << (m_MainWindow.GetFpsState() == WindowFpsState::LOCKED ? "On" : "Off")
                    << ")\n";
                }
                fps = 0;
                ups = 0;
            }

            glfwPollEvents();
            if (glfwWindowShouldClose(glfwWindow)) break;
            glfwSwapBuffers(glfwWindow);
        }
    }
}

static void RegisterGlobalKeyEvent(const GameInputData& inputData) {
    if (!inputData.IsCorrect()) return;
    glfwSetKeyCallback(
        inputData.GetWindow()->GetGlfw(),
        [](GLFWwindow* window, int key, int scancode, int action, int mods) {
            GameInputData* data = static_cast<GameInputData*>(glfwGetWindowUserPointer(window));
            if (data) {
                if (action == GLFW_PRESS) {
                    data->GetWindow()->KeyPressed(key);
                    data->GetWorld()->KeyPressed(*data->GetWindow(), key);
                } else if (action == GLFW_RELEASE) {
                    data->GetWindow()->KeyReleased(key);
                    data->GetWorld()->KeyReleased(*data->GetWindow(), key);
                }
            }
        }
    );
}

static void RegisterGlobalMousePosEvent(const GameInputData& inputData) {
    if (!inputData.IsCorrect()) return;
    glfwSetCursorPosCallback(
        inputData.GetWindow()->GetGlfw(),
        [](GLFWwindow* window, const double xPos, const double yPos) {
            GameInputData* data = static_cast<GameInputData*>(glfwGetWindowUserPointer(window));
            if (data) {
                data->GetWorld()->MouseMoved(*data->GetWindow(), xPos, yPos);
            }
        }
    );
}

static void RegisterGlobalMouseButtonClickEvent(const GameInputData& inputData) {
    if (!inputData.IsCorrect()) return;
    glfwSetMouseButtonCallback(
        inputData.GetWindow()->GetGlfw(),
        [](GLFWwindow* window, const int button, const int action, const int mode) {
            GameInputData* data = static_cast<GameInputData*>(glfwGetWindowUserPointer(window));
            if (data) {
                if (action == GLFW_PRESS) {
                    data->GetWorld()->MousePressed(*data->GetWindow(), button);
                }
                if (action == GLFW_RELEASE) {
                    data->GetWorld()->MouseReleased(*data->GetWindow(), button);
                }
                // data->GetWorld()->MouseMoved(*data->GetWindow(), xPos, yPos);
            }
        }
    );
}

static void RegisterGlobalMouseScrollEvent(const GameInputData& inputData) {
    if (!inputData.IsCorrect()) return;
    glfwSetScrollCallback(
        inputData.GetWindow()->GetGlfw(),
        [](GLFWwindow* window, const double xOffset, const double yOffset) {
            GameInputData* data = static_cast<GameInputData*>(glfwGetWindowUserPointer(window));
            if (data) {
                data->GetWorld()->MouseWheelScrolled(*data->GetWindow(), xOffset, yOffset);
            }
        }
    );
}
