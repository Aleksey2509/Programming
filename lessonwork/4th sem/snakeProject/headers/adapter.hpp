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

            case BLACK:   return sf::Color::Black;
            case RED:     return sf::Color::Red;
            case GREEN:   return sf::Color::Green;
            case YELLOW:  return sf::Color::Yellow;
            case BLUE:    return sf::Color::Blue;
            case MAGENTA: return sf::Color::Magenta;
            case CYAN:    return sf::Color::Cyan;
            case WHITE:   return sf::Color::White;
            default:      return sf::Color::Black;
        }
    }
};

#endif // ifndef ADAPTER_HPP