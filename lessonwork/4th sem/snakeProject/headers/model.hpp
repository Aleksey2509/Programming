#ifndef MODEL_HPP
#define MODEL_HPP

#include <list>
#include <utility>
#include <random>
#include <algorithm>

#include "view.hpp"

using botController = std::function<void ()>;

constexpr int MaxRabbits = 20;

class Model
{
public:
    Model();
    ~Model();

    bool drawAll();

    Snake& createStartSnake();
    Snake& createBotSnake();
    // Snake& createRandomSnake();
    void setBotController(botController update);
    std::list<Rabbit>& getRabbits() { return rabbits_; } // for AI
    std::list<Snake>& getSnakes() { return snakes_; } // for AI


private:
    std::list<Rabbit> rabbits_;
    // std::list<BotSnakeControl> snakesCont_;
    std::list<Snake> snakes_;
    std::vector<botController> botUpdaterVec_;

    const Point getPoint();
    const Point makePointValid(Point& point);

    AbstractView* view = AbstractView::getView();
    std::random_device gen_;
    int rabbitSpawnTime_ = 5;
    void updateSnake(Snake& snake);
    void checkForDeaths();
};

#endif