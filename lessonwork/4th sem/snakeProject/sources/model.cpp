#include "model.hpp"

Model::Model()
{
    view->setDrawer(std::bind(&Model::drawAll, this));
    // printf("Got max x and max y as %d and %d", view->getMaxX(), view->getMaxY());

    for (int i = 0; i < MaxRabbits; ++i)
        rabbits_.push_front(getPoint());
}

// Snake& Model::createRandomSnake()
// {
//     Snake snake;
//     snake.body.push_back(getPoint());
//     snake.col = Color::YELLOW;

//     snakes_.push_back(snake);

//     return snakes_.back();
// }

Snake& Model::createStartSnake()
{

    Snake snake;
    auto Xstart = view->getMaxX() / 2;
    auto Ystart = view->getMaxY() / 2;
    snake.body.push_back({Xstart, Ystart});
    snake.body.push_back({Xstart, Ystart - 1});
    snake.body.push_back({Xstart, Ystart - 2});
    snake.body.push_back({Xstart, Ystart - 3});
    snake.col = Color::GREEN;
    snake.player = true;

    snakes_.push_back(snake);

    return snakes_.back();

}

Snake& Model::createBotSnake()
{
    Snake snake;
    // printf("int create bot snake ctor\n");
    // fflush(stdout);
    snake.body.push_back(getPoint());
    snake.col = Color::YELLOW;
    snake.player = false;

    snakes_.push_back(snake);

    return snakes_.back();
}

void Model::setBotController(botController updater)
{
    botUpdaterVec_.push_back(updater);
}

const Point Model::getPoint()
{
    std::uniform_int_distribution<int> distributionX(2, view->getMaxX() - 3);
    std::uniform_int_distribution<int> distributionY(2, view->getMaxY() - 3);
    
    int x = distributionX(gen_);
    int y = distributionY(gen_);

    Point newPoint (x, y);


    return makePointValid(newPoint);
}

const Point Model::makePointValid(Point& point)
{
    auto oldPoint = point;
    bool needForReplacement = false;

    // printf("in make point valid\n");
    // fflush(stdout);

    while(true)
    {
        for (auto snakeIt : snakes_)
            for (auto it : snakeIt.body)
            {
                if (point == it)
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

    for (auto it : rabbits_)
    {
        view->draw(it);
        //rabbits.pop_front();
    }

    for (auto controlFunc : botUpdaterVec_)
        (controlFunc)();

    for (auto& snakeIt : snakes_)
        updateSnake(snakeIt);

    checkForDeaths();

    auto controlFuncIt = botUpdaterVec_.begin();
    for (auto snakeIt = snakes_.begin(); snakeIt != snakes_.end(); ++snakeIt)
    {
        if (snakeIt->status == Status::LOST)
        {
            if (snakeIt->player == true)
                return true;
            else
            {
                if (snakeIt->player == false)
                    {
                        auto funcToErase = controlFuncIt;
                        controlFuncIt--;
                        botUpdaterVec_.erase(funcToErase);
                        controlFuncIt++;
                    }
    
                auto snakeToErase = snakeIt;
                snakeIt--;
                view->clearSnake(*snakeToErase);
                snakes_.erase(snakeToErase);

                continue;
            }
        }
        
        view->draw(*snakeIt);

        if (snakeIt->status != Status::ATE_A_RABBIT)
        {
            if (snakeIt->body.front() != snakeIt->body.back())
                view->drawSpace(snakeIt->body.back());
            snakeIt->body.pop_back();
        }
        else
            snakeIt->status = Status::ALIVE;

        if (snakeIt->player == false)
            controlFuncIt++;

    }

    ticks = (ticks + 1) % rabbitSpawnTime_;
    if (!ticks && (rabbits_.size() < MaxRabbits))
        rabbits_.push_back(getPoint());
    // auto it = rabbits.begin();
    // view->drawRabbit(*it);

    return false;
}

void Model::checkForDeaths()
{
    for (auto& currentsnakeIt : snakes_)
    {
        if (currentsnakeIt.status == Status::LOST)
            continue;

        auto head = currentsnakeIt.body.begin();
        for (auto& snakeIt : snakes_)
            if (!(currentsnakeIt == snakeIt))
                for (auto bodyIt : snakeIt.body)
                    if (*head == bodyIt)
                    {
                        currentsnakeIt.status = snakeIt.status = Status::LOST;
                    }
    }
}

void Model::updateSnake(Snake& snake)
{
    // FILE* log = fopen ("log", "a");
    auto head = snake.body.front();

    switch(snake.direction)
    {
        case Direction::UP:  (head.first)--;
                                if (head.first == AbstractView::getView()->getMinX())
                                    snake.status = Status::LOST;
                                break;

        case Direction::DOWN:    (head.first)++;
                                    if (head.first == AbstractView::getView()->getMaxX())
                                     snake.status = Status::LOST;
                                 break;

        case Direction::LEFT:  (head.second)--;
                                  if (head.second == AbstractView::getView()->getMinY())
                                     snake.status = Status::LOST;
                                  break;

        case Direction::RIGHT: (head.second)++;
                                   if (head.second == AbstractView::getView()->getMaxY())
                                     snake.status = Status::LOST;
                                  break;
        default : break;
    }

    // fprintf(log, "now head at %D %D with dir %D\n", head.first, head.second, snake_.dir);
    // fclose(log);
    if (snake.status == Status::LOST)
        return;

    for (auto it : snake.body)
    {
        if (head == it)
        {
            snake.status = Status::LOST;
            
            return;
        }
    }

    snake.body.push_front(head);

    for (auto it = rabbits_.begin(); it != rabbits_.end(); it++)
    {
        if (*it == head)
        {
            snake.status = Status::ATE_A_RABBIT;
            auto toErase = it;
            it--;
            rabbits_.erase(toErase);
        }
    }

    return;
}


Model::~Model()
{
    return;
}

