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

    void drawAll();

    Snake snake;
private:
    std::list<Rabbit> rabbits;

    const Point getPoint();

    AbstractView* view = AbstractView::getView();
    std::random_device gen;
    const int rabbitSpawnTime = 5;
};

#endif