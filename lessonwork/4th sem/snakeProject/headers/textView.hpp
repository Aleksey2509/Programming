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
    virtual void draw(const Rabbit& rabbit);
    virtual void draw(const Snake& snake);
    virtual void drawSpace(const Point& point);
    virtual void drawLostMsg();

    void placeCorners();

    virtual void setDrawer(drawer drawerFunc);
    virtual void setKeyHandler(keyHandler keyHandlerFunc);

    virtual const int getMaxX();
    virtual const int getMaxY();



private:
    int windowCol;
    int windowRow;

    void gotoxy(int x, int y);
    void gotoxy(const Point& point);
    void putchar(char c);       //putc
    void putstr(const char* str);     //puts

    void setColor(int color);

    void hline(int x, int y, int len); // box
    void vline(int x, int y, int len); //
    //void sigHandler(int sig);

    void clearScreeen();

    keyHandler keyHandle;
    drawer drawAll;
};

#endif