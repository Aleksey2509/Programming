#ifndef CONROLLER_HPP
#define CONROLLER_HPP

#include <functional>
#include <cmath>

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

class IRobot
{
public:
    IRobot(Snake& snake, Model& model);
    virtual ~IRobot();

protected:

    Snake& snake_;
    std::list<Rabbit>& rabbits;
    virtual void control() {return;};
};


class DumbRobot : public IRobot
{
public:
    DumbRobot(Snake& snake, Model& model);
    ~DumbRobot();

private:
    virtual void control() override;
};

class LessDumbRobot : public IRobot
{
public:
    LessDumbRobot(Snake& snake, Model& model);
    ~LessDumbRobot();

private:
    virtual void control() override;
};

#endif