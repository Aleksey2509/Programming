#include "graphView.hpp"
#include "adapter.hpp"

GraphView::GraphView(): maxX_(40), maxY_(40), pixelSize_(20), window_(sf::VideoMode(maxX_ * pixelSize_, maxY_ * pixelSize_), "Snake")
{
    window_.setFramerateLimit(20);
}

void GraphView::run()
{
    while (window_.isOpen())
    {
        sf::Event event;
        bool ifEnd = false;
        while (window_.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window_.close();

            if (!ifEnd)
            {
                if (event.type == sf::Event::KeyPressed)
                    keyHandlerFunc(Adapter::adaptKey(event.key.code));

            }
        }

        window_.clear();
        ifEnd = drawAll();
        window_.display();
        if (ifEnd)
            window_.close();

        
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
    // printf("drawing a rabbit at %d %D\n", rabbit.first, rabbit.second);
    sf::RectangleShape section;
    section.setSize(sf::Vector2f(pixelSize_, pixelSize_));
    section.setFillColor(sf::Color::Red);
    section.setPosition(sf::Vector2f(rabbit.second * pixelSize_, rabbit.first * pixelSize_));
    window_.draw(section);

    return;
}

void GraphView::draw(const Snake& snake)
{
    for (auto it = snake.body.begin(); it != snake.body.end(); it++)
    {
        // printf("drawing a point at %d %D\n", it->first, it->second);
        sf::RectangleShape section;
        section.setSize(sf::Vector2f(pixelSize_, pixelSize_));
        section.setFillColor(Adapter::adaptColGView(snake.col));
        section.setPosition(sf::Vector2f(it->second * pixelSize_, it->first * pixelSize_));
        window_.draw(section);
    }

    return;
}

void GraphView::drawSpace(const Point& point)
{
    sf::RectangleShape blackPoint;
    blackPoint.setSize(sf::Vector2f(pixelSize_, pixelSize_));
    blackPoint.setFillColor(Adapter::adaptColGView(Color::BLACK));
    blackPoint.setPosition(sf::Vector2f(point.second * pixelSize_, point.first * pixelSize_));
    window_.draw(blackPoint);
    return;
}

void GraphView::clearSnake(const Snake& snake)
{
    for (auto snakeIt : snake.body)
    {
        drawSpace(snakeIt);
    }
}

void GraphView::drawLostMsg()
{
    window_.close();
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

inline const int GraphView::getMinY() { return 0; }

inline const int GraphView::getMinX() { return 0; }

inline const int GraphView::getMaxX() { return maxX_; }

inline const int GraphView::getMaxY() { return maxY_; }


GraphView::~GraphView() { }