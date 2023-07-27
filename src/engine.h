#pragma once
#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include "worldmanager.h"
class WorldManager;

class Engine
{
    void Init();

    sf::ContextSettings settings;
    sf::Clock timer;

public:
    Engine();

    void Update();
    void ProcessEvents();
    void Run();

    std::unique_ptr<sf::RenderWindow> Window;

    std::unique_ptr<WorldManager> World;
};
