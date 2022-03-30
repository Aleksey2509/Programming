#ifndef TEXTVIEW_HPP
#define TEXTVIEW_HPP

#include "view.hpp"

class TextView : public AbstractView
{
public:
    TextView();
    ~TextView();

    virtual void drawGameBoard();
    virtual void run();
    virtual void draw();
    virtual void draw(Rabbit rabbit);
    virtual void draw(Snake& snake);

    void placeCorners();

    virtual void setDrawer(Drawer drawer);
    virtual void setKeyHandler(kHandler keyHandler);

    virtual const int getMaxX();
    virtual const int getMaxY();



private:
    int windowCol;
    int windowRow;

    void gotoxy(int x, int y);
    void gotoxy(Point point);
    void putchar(char c);       //putc
    void putstr(const char* str);     //puts

    void setColor(int color);

    void hline(int x, int y, int len); // box
    void vline(int x, int y, int len); //
    //void sigHandler(int sig);

    void clearScreeen();

    kHandler keyHandler;
    Drawer drawAll;
};

#endif