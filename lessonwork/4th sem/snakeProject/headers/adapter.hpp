#ifndef ADAPTER_HPP
#define ADAPTER_HPP

#include <SFML/Graphics.hpp>
#include "view.hpp"



namespace Adapter
{
    inline char adaptKey(sf::Keyboard::Key pressedKey)
    {
        switch(pressedKey)
        {
            case sf::Keyboard::W:
            case sf::Keyboard::Up: return 'w';
                                    break;

            case sf::Keyboard::S:
            case sf::Keyboard::Down: return 's';
                                        break;

            case sf::Keyboard::D:
            case sf::Keyboard::Right: return 'd';
                                        break;

            case sf::Keyboard::A:
            case sf::Keyboard::Left: return 'a';
                                        break;

            default: return 0;
        }
    }

    inline int adaptColTView (Color col)
    {
        return static_cast<int>(col) + 30;
    }

    inline sf::Color adaptColGView (Color col)
    {
        switch(col)
        {

            case Color::BLACK:   return sf::Color::Black;
            case Color::RED:     return sf::Color::Red;
            case Color::GREEN:   return sf::Color::Green;
            case Color::YELLOW:  return sf::Color::Yellow;
            case Color::BLUE:    return sf::Color::Blue;
            case Color::MAGENTA: return sf::Color::Magenta;
            case Color::CYAN:    return sf::Color::Cyan;
            case Color::WHITE:   return sf::Color::White;
            default:      return sf::Color::Black;
        }
    }
};

#endif // ifndef ADAPTER_HPP