#include "controller.hpp"
#include "model.hpp"

Human::Human(Snake& snake) : snake_(snake)
{
    //using Controller::Controller();
    auto view = AbstractView::getView();
    view->setKeyHandler(std::bind(&Human::handleKey, this, std::placeholders::_1));
    snake.col = Color::GREEN;

}

void Human::handleKey(char key)
{
    // FILE* log = fopen ("log", "a");
    // auto it = snake_.body.begin();
    // fprintf(log, "head was at %D %D with dir %D, key - %c\n", it->first, it->second, snake_.dir, key);
    // fflush(log);

    key = tolower(key);
    switch(key)
    {
        case 'w': snake_.direction = Direction::UP;
                // printf("got w\n");
                  break;
        case 's': snake_.direction = Direction::DOWN;
                // printf("got s\n");
                  break;
        case 'a': snake_.direction = Direction::LEFT;
                // printf("got a\n");
                  break;
        case 'd': snake_.direction = Direction::RIGHT;
                // printf("got d\n");
                  break;

        default: break;
    }
    // fclose(log);

    return;
}

Human::~Human() { }


Robot::Robot(Model& model) : snake_(model.createBotSnake()), rabbits_(model.getRabbits()), snakes_(model.getSnakes())
{
    model.setBotController(std::bind(&Robot::control, this));
}

Robot::~Robot() { return; }

DumbRobot::DumbRobot(Model& model) : Robot::Robot(model)
{
    snake_.col = Color::CYAN;
}

void DumbRobot::control()
{
    auto rabbitToEat = rabbits_.front();
    auto snakeHead = snake_.body.begin();

    if ((rabbitToEat.first > snakeHead->first))
    {
        snake_.direction = Direction::DOWN;
        return;
    }
    if ((rabbitToEat.first < snakeHead->first))
    {
        snake_.direction = Direction::UP;
        return;
    }
    if ((rabbitToEat.second > snakeHead->second))
    {
        snake_.direction = Direction::RIGHT;
        return;
    }
    if((rabbitToEat.second < snakeHead->second))
    {
        snake_.direction = Direction::LEFT;
        return;
    }
    return;
}

DumbRobot::~DumbRobot()
{

}

LessDumbRobot::LessDumbRobot(Model& model) : Robot::Robot(model)
{
    snake_.col = Color::YELLOW;
}

bool LessDumbRobot::ifViableDirection(Direction direction)
{
    auto futureHeadPos = *(snake_.body.begin());
    switch(direction)
    {
        case Direction::UP:  (futureHeadPos.first)--;
                                if (futureHeadPos.first == 1)
                                    return false;
                                break;

        case Direction::DOWN:    (futureHeadPos.first)++;
                                    if (futureHeadPos.first == AbstractView::getView()->getMaxX())
                                     return false;
                                 break;

        case Direction::LEFT:  (futureHeadPos.second)--;
                                  if (futureHeadPos.second == 1)
                                     return false;
                                  break;

        case Direction::RIGHT: (futureHeadPos.second)++;
                                   if (futureHeadPos.second == AbstractView::getView()->getMaxY())
                                     return false;
                                  break;

        default: return false;
    }

    for (auto snakeIt : snakes_)
    {
        for (auto bodyIt : snakeIt.body)
        {
            if (bodyIt == futureHeadPos)
                return false;
        }
    }

    return true;
}

void LessDumbRobot::control()
{
    int minDist = -1;
    auto snakeHead = snake_.body.begin();
    auto rabbitToEat = rabbits_.end();
    for (auto it = rabbits_.begin(); it != rabbits_.end(); ++it)
    {
        int dist = abs(it->first - snakeHead->first) + abs(it->second - snakeHead->second);

        if ((dist < minDist) || (minDist == -1))
        {
            rabbitToEat = it;
            minDist = dist;
        }
    }

    if (rabbitToEat == rabbits_.end())
    {
        return;
    }

    if ((rabbitToEat->first > snakeHead->first) && ifViableDirection(Direction::DOWN))
    {
        snake_.direction = Direction::DOWN;
        return;
    }
    
    if ((rabbitToEat->first < snakeHead->first) && ifViableDirection(Direction::UP))
    {
        snake_.direction = Direction::UP;
        return;
    }

    if ((rabbitToEat->second > snakeHead->second) && ifViableDirection(Direction::RIGHT))
    {
        snake_.direction = Direction::RIGHT;
        return;
    }

    if((rabbitToEat->second < snakeHead->second) && ifViableDirection(Direction::LEFT))
    {
        snake_.direction = Direction::LEFT;
        return;
    }
    

    // yeah, a bit sorry for this
    for (int dir = (int)Direction::UP; dir != (int)Direction::NO_DIRECTION; dir++)
    {
        if (ifViableDirection((Direction)dir))
        {
            snake_.direction = (Direction)dir;
            return;
        }
    }

    return;
}

LessDumbRobot::~LessDumbRobot()
{
    
}