#include "Common/Assert.hpp"
#include "Common/Entrypoint.hpp"
#include "SFML/Window.hpp"

MCE_ENTRYPOINT
{
    sf::Window wnd(sf::VideoMode({1280, 720}), "Minecraft: Community Edition");

    while (wnd.isOpen())
    {
        while (const auto event = wnd.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                wnd.close();
        }
    }

    return 0;
}