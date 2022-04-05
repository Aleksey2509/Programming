#ifndef GRAPHVIEW_HPP
#define GRAPHVIEW_HPP

#include "view.hpp"
#include <SFML/Graphics.hpp>

class GraphView : public AbstractView
{
public:
    GraphView();
    ~GraphView();

    virtual void drawGameBoard();
    virtual void run();
    virtual void draw();
    virtual void draw(const Rabbit& rabbit);
    virtual void draw(const Snake& snake);
    virtual void drawSpace(const Point& point);
    virtual void drawLostMsg();

    virtual const int getMaxX();
    virtual const int getMaxY();

    virtual void setDrawer(drawer drawerFunc);
    virtual void setKeyHandler(keyHandler keyHandlerFunc);
};

#endif