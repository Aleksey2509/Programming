#include "model.hpp"

Model::Model()
{
    view->setDrawer(std::bind(&Model::drawAll, this));
    // printf("Got max x and max y as %d and %d", view->getMaxX(), view->getMaxY());

    for (int i = 0; i < StartingRabbits; ++i)
        rabbits.push_front(getPoint());
}

Snake& Model::createRandomSnake()
{
    Snake snake;
    snake.body.push_back(getPoint());
    snake.col = Color::YELLOW;

    snakes.push_back(snake);

    return snakes.back();
}

void Model::setBotController(botController update)
{
    botUpdaterVec.push_back(update);
}

Snake& Model::createStartSnake()
{

    Snake snake;
    snake.body.push_back({view->getMaxX() / 2, view->getMaxY() / 2});
    snake.body.push_back({view->getMaxX() / 2, view->getMaxY() / 2 - 1});
    snake.body.push_back({view->getMaxX() / 2, view->getMaxY() / 2 - 2});
    snake.body.push_back({view->getMaxX() / 2, view->getMaxY() / 2 - 3});
    snake.col = Color::GREEN;

    snakes.push_back(snake);

    return snakes.back();

}

const Point Model::getPoint()
{
    std::uniform_int_distribution<int> distributionX(2, view->getMaxX() - 3);
    std::uniform_int_distribution<int> distributionY(2, view->getMaxY() - 3);
    
    int x = distributionX(gen);
    int y = distributionY(gen);

    Point newPoint (x, y);


    return makePointValid(newPoint);
}

const Point Model::makePointValid(Point& point)
{
    Point oldPoint = point;
    bool needForReplacement = false;

    while(true)
    {
        for (auto snakeIt = snakes.begin(); snakeIt != snakes.end(); ++snakeIt)
            for (auto it = snakeIt->body.begin(); it != snakeIt->body.end(); ++it)
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

    for (auto controlFunc = botUpdaterVec.begin(); controlFunc != botUpdaterVec.end(); ++controlFunc)
        (*controlFunc)();

    for (auto snakeIt = snakes.begin(); snakeIt != snakes.end(); ++snakeIt)
    {
        updateSnake(*snakeIt);

        if (snakeIt->status == Snake::Status::LOST)
        {
            if (snakeIt == snakes.begin())
                return true;
            else
                continue;
        }
        
        view->draw(*snakeIt);

        if (snakeIt->status != Snake::Status::ATE_A_RABBIT)
        {
            if (snakeIt->body.front() != snakeIt->body.back())
                view->drawSpace(snakeIt->body.back());
            snakeIt->body.pop_back();
        }
        else
            snakeIt->status = Snake::Status::ALIVE;
    }

    

    ticks = (ticks + 1) % rabbitSpawnTime;
    if (!ticks && (rabbits.size() < StartingRabbits))
        rabbits.push_back(getPoint());
    // auto it = rabbits.begin();
    // view->drawRabbit(*it);

    return false;
}

void Model::updateSnake(Snake& snake)
{
    // FILE* log = fopen ("log", "a");
    auto head = snake.body.front();

    switch(snake.direction)
    {
        case Snake::Direction::UP:  (head.first)--;
                                if (head.first == 1)
                                    snake.status = Snake::Status::LOST;
                                break;

        case Snake::Direction::DOWN:    (head.first)++;
                                    if (head.first == AbstractView::getView()->getMaxX())
                                     snake.status = Snake::Status::LOST;
                                 break;

        case Snake::Direction::LEFT:  (head.second)--;
                                  if (head.second == 1)
                                     snake.status = Snake::Status::LOST;
                                  break;

        case Snake::Direction::RIGHT: (head.second)++;
                                   if (head.second == AbstractView::getView()->getMaxY())
                                     snake.status = Snake::Status::LOST;
                                  break;
    }

    // fprintf(log, "now head at %D %D with dir %D\n", head.first, head.second, snake_.dir);
    // fclose(log);
    if (snake.status == Snake::Status::LOST)
        return;

    for (auto it = snake.body.begin(); *it != snake.body.back(); it++)
    {
        if (head == *it)
        {
            snake.status = Snake::Status::LOST;
            return;
        }
    }

    snake.body.push_front(head);

    for (auto it = rabbits.begin(); it != rabbits.end(); it++)
    {
        if (*it == head)
        {
            snake.status = Snake::Status::ATE_A_RABBIT;
            auto toErase = it;
            rabbits.erase(toErase);
        }
    }

    return;
}


Model::~Model()
{
    return;
}

