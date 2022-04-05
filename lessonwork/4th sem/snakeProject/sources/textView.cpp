#include <sys/ioctl.h>
#include <termios.h>
#include <signal.h>
#include <functional>
#include <poll.h>

#include "textView.hpp"
#include "model.hpp"


struct termios oldSettings;
bool final = false;

void sigHandler(int sig)
{
    final = true;

    return;
}

TextView::TextView()
{
    //printf("I am text view\n");
    setvbuf (stdout, NULL, _IONBF, 0);

    winsize currentTerminalSize;

    ioctl(1, TIOCGWINSZ, &currentTerminalSize);
    ioctl(1, TIOCSWINSZ, &currentTerminalSize);

    windowRow = currentTerminalSize.ws_row;
    windowCol = currentTerminalSize.ws_col;

    // std::function<void(int)> sigHand;
    // sigHand = sigHandler;
    struct termios terminalSettings;

    tcgetattr(0, &terminalSettings);
    oldSettings = terminalSettings;

    cfmakeraw(&terminalSettings);
    terminalSettings.c_lflag |= ISIG;
    tcsetattr(0, TCSANOW, &terminalSettings);

    signal(SIGINT, sigHandler);
}

void TextView::setDrawer(drawer drawFunc)
{
    drawAll = drawFunc;
}

void TextView::setKeyHandler(keyHandler keyHandlerFunc)
{
    keyHandle = keyHandlerFunc;
}

const int TextView::getMaxX()
{
    return windowRow;
}

const int TextView::getMaxY()
{
    return windowCol;
}

void TextView::draw()
{

}

void TextView::run()
{
    struct pollfd fds = {0, POLLIN};

    drawGameBoard();
    bool ifLost = false;
    drawAll();
    //FILE* log = fopen("log", "a");
    while (final == false)
    {
        //printf("proccessing\n");

        int pollRes = poll (&fds, 1, 500);
        if (pollRes == 1)
        {
            // printf("shit, a key comming");
            

            char keyBuf[BUFSIZ];
            int size = read(0, keyBuf, BUFSIZ);

            // fprintf(log, "caught %c\n", keyBuf[0]);
            // fflush(log);

            for (int i = 0; i < size; i++)
                if (keyBuf[i] == 'q')
                {
                    final = true;
                    break;
                }
                else
                {
                    keyHandle(keyBuf[i]);
                    ifLost = drawAll();
                    if (ifLost)
                    {
                        drawLostMsg();
                        final = true;
                        break;
                    }
                }
        }

        // if (n == 0)
        // {

        // }

        usleep(500000);

        //game->draw();
    }
    //fclose(log);
    //printf("\nend proccessing\n");

    return;
}

void TextView::draw(const Rabbit& rabbit)
{   
    gotoxy(rabbit.first, rabbit.second);
    setColor(5);
    putchar('.');
    printf("\e[m");
}

void TextView::draw(const Snake& snake)
{
    //FILE* log = fopen ("log", "a");
    auto it = snake.body.begin();
    // fprintf(log, "head at %D %D with dir %D\n", it->first, it->second, snake.dir);
    // fflush(log);

    gotoxy(*it);

    switch (snake.dir)
    {
        case Snake::UP : putchar('^');
                break;
        case Snake::DOWN: putchar('v');
                break;
        case Snake::LEFT: putchar('<');
                break;
        case Snake::RIGHT: putchar('>');
                break;
    }

    auto lastToDraw = snake.body.end();
    if (snake.body.front() == snake.body.back())
        lastToDraw--;

    for (it++; it != lastToDraw; it++)
    {
        gotoxy(*it);
        putchar('#');
    }

    //fclose(log);

    return;
}

void TextView::drawSpace(const Point& point)
{
    gotoxy(point.first, point.second);
    putchar(' ');

    return;
}

void TextView::drawLostMsg()
{
    gotoxy(getMaxX()/2, (getMaxY() - strlen("YOU LOST!")) / 2);
    puts("YOU LOST!");
    sleep(2);

    return;
}

void TextView::drawGameBoard()
{
    // printf("Drawing game state\n");
    //clearScreeen();

    placeCorners();
    

    hline(1, 2, windowCol - 2);
    vline(2, 1, windowRow - 2);
    hline(windowCol - 1, 2, windowCol - 2);
    vline(2, windowCol, windowRow - 2);

    printf("\e[m");

    gotoxy(1, (windowCol- strlen("SNAKE")) / 2);
    puts("SNAKE");

    // gotoxy(windowRow, windowCol - strlen("v0.1") - 2);
    // puts("v0.1");

    //drawRabbits();

    return;
}

//======================================================================================================================

void TextView::placeCorners()
{
    gotoxy(1, 1);
    putchar('+');
    usleep(200000);

    gotoxy(windowRow, 1);
    putchar('+');
    usleep(200000);

    gotoxy(1, windowCol);
    putchar('+');
    usleep(200000);

    gotoxy(windowRow, windowCol);
    putchar('+');
    usleep(200000);
}

void TextView::gotoxy(int x, int y)
{
    printf("\e[%d;%dH", x, y);
    return;
}

void TextView::gotoxy(const Point& point)
{
    printf("\e[%d;%dH", point.first, point.second);
    return;
}

void TextView::putchar(char c)      //putc
{
    printf("%c", c);
    return;
}

void TextView::putstr(const char* str)     //puts
{
    printf("%s", str);
    return;
}

void TextView::setColor(int color)
{
    //printf ("\e[m");
    printf ("\e[%dm", 30 + color);
    return;
}

void TextView::hline(int x, int y, int len) // box
{
    for (int i = 0; i < len; i++)
    {
        gotoxy(x, y + i);
        usleep(10000);
        setColor(3);
        putchar('-');
    }

    return;
}

void TextView::vline(int x, int y, int len)
{

    for (int i = 0; i < len; i++)
    {
        gotoxy(x + i, y);
        usleep(10000);
        setColor(3);
        putchar('|');
    }

    return;
}

void TextView::clearScreeen()
{
    printf("\eH\e[J");
    return;
}

//======================================================================================================================

TextView::~TextView()
{
    sleep(3);
    gotoxy(1, 1);
    clearScreeen();
    // printf("destroying\n");
    tcsetattr(0, TCSANOW, &oldSettings);
    setvbuf (stdout, NULL, _IOFBF, 0);
    return;
}