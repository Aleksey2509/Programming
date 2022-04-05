#include "graphView.hpp"

GraphView::GraphView()
{
    printf("I starting to being implemented\n");
}

void GraphView::run()
{
    sf::RenderWindow window(sf::VideoMode(200, 200), "SFML works!");
    sf::CircleShape shape(100.f);
    shape.setFillColor(sf::Color::Green);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(shape);
        window.display();
    }

    return;
}

void GraphView::drawGameBoard()
{
    return;
}

void GraphView::draw()
{
    return;
}

void GraphView::draw(const Rabbit& rabbit)
{

}

void GraphView::draw(const Snake& snake)
{
    
}

void GraphView::drawSpace(const Point& point)
{
    
}

void GraphView::drawLostMsg()
{
    
}

void GraphView::setDrawer(drawer drawFunc)
{
    
}

const int GraphView::getMaxX()
{
    return 0;
}

const int GraphView::getMaxY()
{
    return 0;
}

void GraphView::setKeyHandler(keyHandler keyHandlerFunc)
{
    return;
}

GraphView::~GraphView()
{
    return;
}