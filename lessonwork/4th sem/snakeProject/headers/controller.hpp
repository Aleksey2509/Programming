#ifndef CONROLLER_HPP
#define CONROLLER_HPP

#include <functional>
#include <cmath>
#include <algorithm>

#include "model.hpp"

class Human
{

public:
    Human(Snake& snake);
    ~Human();

private:

    Snake& snake_;

    void handleKey(char key) ;
};

class Robot
{
public:
    Robot(Model& model);
    virtual ~Robot();

protected:

    Snake& snake_;
    std::list<Rabbit>& rabbits_;
    std::list<Snake>& snakes_;
    virtual void control() {return;};
};


class DumbRobot : public Robot
{
public:
    DumbRobot(Model& model);
    ~DumbRobot();

private:
    virtual void control() override;
};

class LessDumbRobot : public Robot
{
public:
    LessDumbRobot(Model& model);
    ~LessDumbRobot();

private:
    bool ifViableDirection(Direction direction);
    virtual void control() override;
};

#endif