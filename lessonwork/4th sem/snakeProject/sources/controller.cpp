#include "controller.hpp"
#include "view.hpp"

Human::Human(Snake& snakeToControl) : snake_(snakeToControl)
{
    //using Controller::Controller();
    AbstractView* view = AbstractView::getView();
    view->setKeyHandler(std::bind(&Human::handleKey, this, std::placeholders::_1));

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
        case 'w': snake_.dir = Snake::UP;
                  break;
        case 's': snake_.dir = Snake::DOWN;
                  break;
        case 'a': snake_.dir = Snake::LEFT;
                  break;
        case 'd': snake_.dir = Snake::RIGHT;
                  break;

        default: break;
    }
    // fclose(log);

    return;
}

Human::~Human()
{

}