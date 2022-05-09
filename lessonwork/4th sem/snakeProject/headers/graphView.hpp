#ifndef GRAPHVIEW_HPP
#define GRAPHVIEW_HPP

#include <SFML/Graphics.hpp>

#include "view.hpp"
#include "adapter.hpp"

class GraphView : public AbstractView
{
public:
    GraphView();
    ~GraphView();

    virtual void drawGameBoard();
    virtual void run();
    virtual void draw();
    virtual void draw(const Rabbit& rabbit);
    virtual void draw(const Snake& snake);
    virtual void drawSpace(const Point& point);
    virtual void clearSnake(const Snake& snake);
    virtual void drawLostMsg();

    virtual inline const int getMaxX();
    virtual inline const int getMaxY();
    virtual inline const int getMinX();
    virtual inline const int getMinY();

    virtual void setDrawer(drawer drawerFunc);
    virtual void setKeyHandler(keyHandler keyHandlerFunc);

private:

    int maxX_;
    int maxY_;
    int pixelSize_;
    sf::RenderWindow window_;

    drawer drawAll;
    keyHandler keyHandlerFunc;

    struct SnakeSection
    {
        sf::Vector2f position;
        sf::RectangleShape section;

        explicit SnakeSection(sf::Vector2f startPosition)
        {
            section.setSize(sf::Vector2f(20, 20));
            section.setFillColor(sf::Color::Green);
            section.setPosition(startPosition);
            position = startPosition;
        }

    };
};

#endif