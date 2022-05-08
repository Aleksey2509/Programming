#include "graphView.hpp"
#include "adapter.hpp"

GraphView::GraphView(): maxX(40), maxY(40), pixelSize(20), window(sf::VideoMode(maxX * pixelSize, maxY * pixelSize), "Snake")
{
    window.setFramerateLimit(20);
}

void GraphView::run()
{
    while (window.isOpen())
    {
        sf::Event event;
        bool ifEnd = false;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            if (!ifEnd)
            {
                if (event.type == sf::Event::KeyPressed)
                    keyHandlerFunc(Adapter::adaptKey(event.key.code));

            }
        }

        window.clear();
        ifEnd = drawAll();
        window.display();
        if (ifEnd)
            window.close();

        
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
    printf("drawing a rabbit at %d %D\n", rabbit.first, rabbit.second);
    sf::RectangleShape section;
    section.setSize(sf::Vector2f(pixelSize, pixelSize));
    section.setFillColor(sf::Color::Red);
    section.setPosition(sf::Vector2f(rabbit.second * pixelSize, rabbit.first * pixelSize));
    window.draw(section);

    return;
}

void GraphView::draw(const Snake& snake)
{
    for (auto it = snake.body.begin(); it != snake.body.end(); it++)
    {
        printf("drawing a point at %d %D\n", it->first, it->second);
        sf::RectangleShape section;
        section.setSize(sf::Vector2f(pixelSize, pixelSize));
        section.setFillColor(Adapter::adaptColGView(snake.col));
        section.setPosition(sf::Vector2f(it->second * pixelSize, it->first * pixelSize));
        window.draw(section);
    }

    return;
}

void GraphView::drawSpace(const Point& point)
{
    sf::RectangleShape blackPoint;
    blackPoint.setSize(sf::Vector2f(pixelSize, pixelSize));
    blackPoint.setFillColor(Adapter::adaptColGView(Color::BLACK));
    blackPoint.setPosition(sf::Vector2f(point.second * pixelSize, point.first * pixelSize));
    window.draw(blackPoint);
    return;
}

void GraphView::drawLostMsg()
{
    window.close();
    return;
}

void GraphView::setDrawer(drawer drawFunc)
{
    drawAll = drawFunc;
    return;
}

void GraphView::setKeyHandler(keyHandler keyHandler)
{
    keyHandlerFunc = keyHandler;
    return;
}

inline const int GraphView::getMaxX() { return maxX; }

inline const int GraphView::getMaxY() { return maxY; }


GraphView::~GraphView() { }