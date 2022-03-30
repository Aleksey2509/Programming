#ifndef VIEW_HPP
#define VIEW_HPP

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "unistd.h"
#include <list>
#include <utility>
#include <functional>

using Point = std::pair<int, int>;
using Rabbit = Point;
using kHandler = std::function<void (int )>;
using Drawer = std::function<void ()>;

class Snake
{
public:
    std::list<Point> body;
    enum Direction
    {
        UP = 0,
        DOWN,
        LEFT,
        RIGHT
    };

    int dir;
};

struct AbstractView
{
    static AbstractView* getView(const char* viewType = nullptr);
    virtual void drawGameBoard() = 0; // abstract method
    virtual void run() = 0;
    virtual void draw() = 0;
    virtual void draw(Rabbit rabbit) = 0;
    virtual void draw(Snake& snake) = 0;

    virtual void setDrawer(Drawer) = 0;
    virtual void setKeyHandler(kHandler) = 0;

    virtual const int getMaxX() = 0;
    virtual const int getMaxY() = 0;


    virtual ~AbstractView(){}

private:
    static AbstractView* viewObj;

    Drawer drawAll;
    kHandler keyHandler;
};

#endif