#include "engine.h"
#include "worldmanager.h"
#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <memory>

#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <cmath>
#include <iostream>


Engine::Engine()
{
    Init();
}


void Engine::Init()
{
    settings.depthBits = 24; 
    settings.majorVersion = 4; 
    settings.minorVersion = 4;

    Window = std::make_unique<sf::RenderWindow>(sf::VideoMode(800, 600, 32), "SFML OpenGL", sf::Style::Default, settings);

    GLenum glewInitResult = glewInit();
    if (glewInitResult != GLEW_OK) {
        std::cerr << "Error initializing GLEW: " << glewGetErrorString(glewInitResult) << std::endl;
    }

    // Check OpenGL version
    if (!GLEW_VERSION_3_3) {
        std::cerr << "OpenGL 3.3 is not supported by your graphics card or driver." << std::endl;
    }

    glClearDepth(1.f);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);

    World = std::make_unique<WorldManager>();
}
void Engine::Update()
{
    World->Update(Window->getSize().x, Window->getSize().y);
}




void Engine::ProcessEvents()
{
    sf::Event Event;
    while (Window->pollEvent(Event))
    {
        if (Event.type == sf::Event::Closed)
            Window->close();

        if ((Event.type == sf::Event::KeyPressed) && (Event.key.code == sf::Keyboard::Escape))
        {
            Window->close();
        }
        if ((Event.type == sf::Event::KeyPressed) && ((Event.key.code == sf::Keyboard::A) || (Event.key.code == sf::Keyboard::Left)))
        {
            if (timer.getElapsedTime().asMilliseconds() >= 50)
            {
                World->SphereVelocityX -= World->SpheredeltaVelocityX;
                timer.restart();
            }
        }
        if ((Event.type == sf::Event::KeyPressed) && ((Event.key.code == sf::Keyboard::D) || (Event.key.code == sf::Keyboard::Right)))
        {
            if (timer.getElapsedTime().asMilliseconds() >= 50)
            {
                World->SphereVelocityX += World->SpheredeltaVelocityX;
                timer.restart();
            }
        }
        if ((Event.type == sf::Event::KeyPressed) && (Event.key.code == sf::Keyboard::Space))
        {
            if (timer.getElapsedTime().asMilliseconds() >= 50)
            {
                World->SphereJump();
                
                timer.restart();
                if (World->EndingGame)
                {
                    World->Restart();
                }
            }
        }
    }
}

void Engine::Run()
{
    Window->setActive(true);
    while (Window->isOpen())
    {
        ProcessEvents();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        if (!World->EndingGame)
        {
/*            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

            glBindVertexArray(0);
            glDisableVertexAttribArray(0);
            glUseProgram(0);
            glDisableVertexAttribArray(1);

            Window->setActive(false);
            World->DrawBackground(Window.get());
            glClearDepth(1.f);
            glEnable(GL_DEPTH_TEST);
            glDepthMask(GL_TRUE);*/

       //     Window->setActive(true);  
            World->DrawBackground(Window.get());
            Update();       
            World->Draw();
          
        }
        else
        {     
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
            glDisableVertexAttribArray(0);
            glUseProgram(0);
            glDisableVertexAttribArray(1);

            Window->setActive(false);

            World->DrawEndBanner(Window.get());
        }
        Window->display();
    }
}