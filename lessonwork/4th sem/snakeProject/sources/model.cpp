#include "model.hpp"

Model::Model()
{
    view->setDrawer(std::bind(&Model::drawAll, this));

    snake.body.push_back({view->getMaxX() / 2, view->getMaxY() / 2});
    snake.body.push_back({view->getMaxX() / 2, view->getMaxY() / 2 - 1});
    snake.body.push_back({view->getMaxX() / 2, view->getMaxY() / 2 - 2});
    snake.body.push_back({view->getMaxX() / 2, view->getMaxY() / 2 - 3});

    rabbits.push_front(getPoint());
    rabbits.push_front(getPoint());
    rabbits.push_front(getPoint());

    snake.dir = Snake::RIGHT;
}

const Point Model::getPoint()
{
    std::uniform_int_distribution<int> distributionX(2, view->getMaxX() - 3);
    std::uniform_int_distribution<int> distributionY(2, view->getMaxY() - 3);
    
    int x = distributionX(gen);
    int y = distributionY(gen);

    Rabbit newRabbit (x, y);


    return makePointValid(newRabbit);
}

const Point Model::makePointValid(Point& point)
{
    Point oldPoint = point;
    bool needForReplacement = false;

    while(true)
    {
        for (auto it = snake.body.begin(); it != snake.body.end(); it++)
        {
            if (point == *it)
            {
                needForReplacement = true;
                break;
            }
        }
        if (needForReplacement)
        {
            if (point.first != 2)
                point.first--;
            else
                point.first++;

            if (oldPoint.first == point.first)
            {
                if (point.second != 2)
                    point.second--;
                else
                    point.second++;
            }

            needForReplacement = false;
        }
        else
            break;
    }
    return point;
}

bool Model::drawAll()
{
    static int ticks = 1;

    for (auto it = rabbits.begin(); it != rabbits.end(); it++)
    {
        view->draw(*it);
        //rabbits.pop_front();
    }

    int status = updateSnake();
    if (status == snake.LOST)
        return true;

    view->draw(snake);

    if (status != snake.ATE_A_RABBIT)
    {
        if (snake.body.front() != snake.body.back())
            view->drawSpace(snake.body.back());
        snake.body.pop_back();
    }

    ticks = (ticks + 1) % rabbitSpawnTime;
    if (!ticks)
        rabbits.push_back(getPoint());
    // auto it = rabbits.begin();
    // view->drawRabbit(*it);

    return false;
}

int Model::updateSnake()
{
    //FILE* log = fopen ("log", "a");
    auto head = snake.body.front();

    int status = snake.ALIVE;
    switch(snake.dir)
    {
        case Snake::UP:  (head.first)--;
                                if (head.first == 1)
                                    status = snake.LOST;
                                break;

        case Snake::DOWN:    (head.first)++;
                                    if (head.first == AbstractView::getView()->getMaxX())
                                     status = snake.LOST;
                                 break;

        case Snake::LEFT:  (head.second)--;
                                  if (head.second == 1)
                                     status = snake.LOST;
                                  break;

        case Snake::RIGHT: (head.second)++;
                                   if (head.second == AbstractView::getView()->getMaxY())
                                     status = snake.LOST;
                                  break;
    }

    // fprintf(log, "now head at %D %D with dir %D\n", head.first, head.second, snake_.dir);
    // fclose(log);
    if (status == snake.LOST)
        return status;

    for (auto it = snake.body.begin(); *it != snake.body.back(); it++)
    {
        if (head == *it)
            return snake.LOST;
    }

    snake.body.push_front(head);

    for (auto it = rabbits.begin(); it != rabbits.end(); it++)
    {
        if (*it == head)
        {
            status = snake.ATE_A_RABBIT;
            rabbits.erase(it);
        }
    }

    return status;
}


Model::~Model()
{
    return;
}

