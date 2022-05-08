#ifndef MODEL_HPP
#define MODEL_HPP

#include <list>
#include <utility>
#include <random>

#include "view.hpp"

using botController = std::function<void ()>;

constexpr int StartingRabbits = 20;

class Model
{
public:
    Model();
    ~Model();

    bool drawAll();

    Snake& createStartSnake();
    Snake& createRandomSnake();
    void setBotController(botController update);
    std::list<Rabbit>& getRabbits() { return rabbits; } // for AI


private:
    std::list<Rabbit> rabbits;
    std::list<Snake> snakes;
    std::vector<botController> botUpdaterVec;

    const Point getPoint();
    const Point makePointValid(Point& point);

    AbstractView* view = AbstractView::getView();
    std::random_device gen;
    const int rabbitSpawnTime = 5;
    void updateSnake(Snake& snake);
};

#endif