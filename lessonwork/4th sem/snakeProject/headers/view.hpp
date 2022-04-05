#ifndef VIEW_HPP
#define VIEW_HPP

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <list>
#include <utility>
#include <functional>

using Point = std::pair<int, int>;
using Rabbit = Point;
using drawer = std::function<bool ()>;
using keyHandler = std::function<void (int )>;

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

    enum Status
    {
        ALIVE,
        ATE_A_RABBIT = 0x1,
        LOST,
    };

    int dir;
};

struct AbstractView
{
    static AbstractView* getView(const char* viewType = nullptr);
    virtual void drawGameBoard() = 0; // abstract method
    virtual void run() = 0;
    virtual void draw() = 0;
    virtual void draw(const Rabbit& rabbit) = 0;
    virtual void draw(const Snake& snake) = 0;
    virtual void drawSpace(const Point& point) = 0;
    virtual void drawLostMsg() = 0;

    virtual void setDrawer(drawer ) = 0;
    virtual void setKeyHandler(keyHandler ) = 0;

    virtual const int getMaxX() = 0;
    virtual const int getMaxY() = 0;


    virtual ~AbstractView(){}

private:
    static AbstractView* viewObj;

    drawer drawAll;
    keyHandler keyHandlerFunc;
};

#endif