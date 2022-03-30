#include "model.hpp"

Model::Model()
{
    rabbits.push_front(getPoint());
    rabbits.push_front(getPoint());
    rabbits.push_front(getPoint());

    view->setDrawer(std::bind(&Model::drawAll, this));

    snake.body.push_back({view->getMaxX() / 2, view->getMaxY() / 2});
    snake.body.push_back({view->getMaxX() / 2, view->getMaxY() / 2 - 1});
    snake.body.push_back({view->getMaxX() / 2, view->getMaxY() / 2 - 2});
    snake.body.push_back({view->getMaxX() / 2, view->getMaxY() / 2 - 3});

    snake.dir = Snake::RIGHT;
}

const Point Model::getPoint()
{
    std::uniform_int_distribution<int> distributionX(3, view->getMaxX() - 4);
    std::uniform_int_distribution<int> distributionY(3, view->getMaxY() - 4);
    
    int x = distributionX(gen);
    int y = distributionY(gen);

    Rabbit newRabbit (x, y);
    return newRabbit;
}

void Model::drawAll()
{
    static int ticks = 1;
    for (auto it = rabbits.begin(); it != rabbits.end(); it++)
    {
        view->draw(*it);
        rabbits.pop_front();
    }

    view->draw(snake);
    snake.body.pop_back();

    ticks = (ticks + 1) % rabbitSpawnTime;
    if (!ticks)
        rabbits.push_back(getPoint());
    // auto it = rabbits.begin();
    // view->drawRabbit(*it);

    return;
}


Model::~Model()
{
    return;
}

