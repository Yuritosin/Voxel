#ifndef GAME_H
#define GAME_H

#include "Window.h"

#include "world/World.h"

class Game {
public:
    ~Game() = default;
    Game(const int seed);

    void RunLocked();

    bool IsRunning() const { return running; };
private:
    // std::unique_ptr<Window> m_mainWindow;
    World m_World;
    Window m_MainWindow;
    bool running = false;
};

#endif
