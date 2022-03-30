#ifndef GRAPHVIEW_HPP
#define GRAPHVIEW_HPP

#include "view.hpp"

class GraphView : public AbstractView
{
public:
    GraphView();
    ~GraphView();

    virtual void drawGameBoard();
    virtual void run();
    virtual void draw();
    virtual void draw(Rabbit rabbit);
    virtual void draw(Snake& snake);

    virtual const int getMaxX();
    virtual const int getMaxY();

    virtual void setDrawer(Drawer );
    virtual void setKeyHandler(kHandler );
};

#endif