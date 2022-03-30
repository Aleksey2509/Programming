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
    FILE* log = fopen ("log", "a");
    auto it = snake_.body.begin();
    fprintf(log, "head was at %D %D with dir %D, key - %c\n", it->first, it->second, snake_.dir, key);
    fflush(log);

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
    updateSnakePosition();
    fclose(log);

    return;
}

void Human::updateSnakePosition()
{
    FILE* log = fopen ("log", "a");
    auto it = snake_.body.begin();
    Point newHead = *it;

    bool ifLost = false;
    switch(snake_.dir)
    {
        case Snake::UP:  (newHead.first)--;
                                if (newHead.first == 1)
                                    ifLost = true;
                                break;

        case Snake::DOWN:    (newHead.first)++;
                                    if (newHead.second == AbstractView::getView()->getMaxX())
                                     ifLost = true;
                                 break;

        case Snake::LEFT:  (newHead.second)--;
                                  if (newHead.second == 1)
                                     ifLost = true;
                                  break;

        case Snake::RIGHT: (newHead.second)++;
                                   if (newHead.second == AbstractView::getView()->getMaxY())
                                     ifLost = true;
                                  break;
    }

    fprintf(log, "now head at %D %D with dir %D\n", newHead.first, newHead.second, snake_.dir);
    fclose(log);
    //if (ifLost)
    snake_.body.push_front(newHead);
    
}

Human::~Human()
{

}