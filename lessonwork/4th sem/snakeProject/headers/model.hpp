#ifndef MODEL_HPP
#define MODEL_HPP

#include <list>
#include <utility>
#include <random>

#include "view.hpp"

class Model
{
public:
    Model();
    ~Model();

    bool drawAll();

    Snake snake;
private:
    std::list<Rabbit> rabbits;

    const Point getPoint();
    const Point makePointValid(Point& point);

    AbstractView* view = AbstractView::getView();
    std::random_device gen;
    const int rabbitSpawnTime = 5;
    int updateSnake();
};

#endif