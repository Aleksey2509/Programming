#ifndef CONROLLER_HPP
#define CONROLLER_HPP

#include <functional>

#include "model.hpp"

// class Controller
// {
// public:

//     Controller() = 0;
//     ~Controller() = 0;

// protected:
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