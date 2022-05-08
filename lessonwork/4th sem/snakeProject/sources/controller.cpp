#include "controller.hpp"
#include "model.hpp"

Human::Human(Snake& snake) : snake_(snake)
{
    //using Controller::Controller();
    AbstractView* view = AbstractView::getView();
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
        case 'w': snake_.direction = Snake::Direction::UP;
                // printf("got w\n");
                  break;
        case 's': snake_.direction = Snake::Direction::DOWN;
                // printf("got s\n");
                  break;
        case 'a': snake_.direction = Snake::Direction::LEFT;
                // printf("got a\n");
                  break;
        case 'd': snake_.direction = Snake::Direction::RIGHT;
                // printf("got d\n");
                  break;

        default: break;
    }
    // fclose(log);

    return;
}

Human::~Human() { }


IRobot::IRobot(Snake& snake, Model& model) : snake_(snake), rabbits(model.getRabbits())
{
    model.setBotController(std::bind(&IRobot::control, this));
}

IRobot::~IRobot() { return; }

DumbRobot::DumbRobot(Snake& snake, Model& model) : IRobot::IRobot(snake, model)
{
    snake.col = Color::YELLOW;
}

void DumbRobot::control()
{
    auto rabbitToEat = rabbits.front();
    auto snakeHead = snake_.body.begin();

    if (rabbitToEat.first > snakeHead->first)
    {
        snake_.direction = Snake::Direction::LEFT;
    }
    else if (rabbitToEat.first < snakeHead->first)
    {
        snake_.direction = Snake::Direction::RIGHT;
    }
    else if (rabbitToEat.second > snakeHead->second)
    {
        snake_.direction = Snake::Direction::DOWN;
    }
    else
        snake_.direction = Snake::Direction::UP;

    return;
}

DumbRobot::~DumbRobot()
{

}

LessDumbRobot::LessDumbRobot(Snake& snake, Model& model) : IRobot::IRobot(snake, model)
{
    snake.col = Color::YELLOW;
}

void LessDumbRobot::control()
{
    int minDist = -1;
    auto snakeHead = snake_.body.begin();
    auto rabbitToEat = rabbits.end();
    for (auto it = rabbits.begin(); it != rabbits.end(); ++it)
    {
        int dist = abs(it->first - snakeHead->first) + abs(it->second - snakeHead->second);

        if ((dist < minDist) || (minDist == -1))
        {
            rabbitToEat = it;
            minDist = dist;
        }
    }

    if (rabbitToEat->first > snakeHead->first)
    {
        snake_.direction = Snake::Direction::DOWN;
    }
    else if (rabbitToEat->first < snakeHead->first)
    {
        snake_.direction = Snake::Direction::UP;
    }
    else if (rabbitToEat->second > snakeHead->second)
    {
        snake_.direction = Snake::Direction::RIGHT;
    }
    else
        snake_.direction = Snake::Direction::LEFT;

    return;
}

LessDumbRobot::~LessDumbRobot()
{
    
}