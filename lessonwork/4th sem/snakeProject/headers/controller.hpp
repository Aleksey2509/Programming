#ifndef CONROLLER_HPP
#define CONROLLER_HPP

#include <functional>

#include "model.hpp"

// class Controller
// {
// public:

//     Controller() = 0;
//     ~Controller() = 0;

// private:
//     Model::Snake& snake;
// };

class Human
{
public:
    Human(Snake& snakeToControl);
    ~Human();

private:
    Snake& snake_;

    void handleKey(char key);
    void updateSnakePosition();
};

// class RobotAI
// {
//     RobotAI(Model::Snake& snakeToControl);
//     ~RobotAI();

// private:
//     void handleKey(int key);
//     //Model::Snake& snake;
// };

#endif