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

    maxX_ = currentTerminalSize.ws_row;
    maxY_ = currentTerminalSize.ws_col;

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

void TextView::setDrawer(drawer drawFunc) { drawAll = drawFunc; }

void TextView::setKeyHandler(keyHandler keyHandler) { keyHandlerFunc = keyHandler; }

inline const int TextView::getMinY() { return 1; }

inline const int TextView::getMinX() { return 1; }

inline const int TextView::getMaxX() { return maxX_; }

inline const int TextView::getMaxY() { return maxY_; }

void TextView::draw() { }

void TextView::run()
{
    struct pollfd fds = {0, POLLIN};

    clearScreeen();
    drawGameBoard();
    bool ifLost = false;
    drawAll();
    //FILE* log = fopen("log", "a");
    while (final == false)
    {
        //printf("proccessing\n");

        int pollRes = poll (&fds, 1, 100);
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
                    keyHandlerFunc(keyBuf[i]);
                    ifLost = drawAll();
                    if (ifLost)
                    {
                        drawLostMsg();
                        final = true;
                        usleep(500000);
                        return;
                    }
                }
        }
        else
        {
            #if 1
            ifLost = drawAll();
            if (ifLost)
            {
                drawLostMsg();
                final = true;
                usleep(500000);
                return;
            }
            #endif
        }

        
    }
    //fclose(log);
    //printf("\nend proccessing\n");

    return;
}

void TextView::draw(const Rabbit& rabbit)
{   
    gotoxy(rabbit.first, rabbit.second);
    setColor(Color::MAGENTA);
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

    setColor(snake.col);
    switch (snake.direction)
    {
        case Direction::UP : putchar('^');
                break;
        case Direction::DOWN: putchar('v');
                break;
        case Direction::LEFT: putchar('<');
                break;
        case Direction::RIGHT: putchar('>');
                break;
        default : break;
    }

    auto lastToDraw = snake.body.end();
    if (snake.body.front() == snake.body.back())
        lastToDraw--;

    for (it++; it != lastToDraw; it++)
    {
        gotoxy(*it);
        putchar('#');
    }
    setColor(Color::BLACK);
    //fclose(log);

    return;
}

void TextView::drawSpace(const Point& point)
{
    gotoxy(point);
    putchar(' ');

    return;
}

void TextView::clearSnake(const Snake& snake)
{
    for (auto snakeIt : snake.body)
    {
        drawSpace(snakeIt);
    }
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
    

    hline(1, 2, maxY_ - 2);
    vline(2, 1, maxX_ - 2);
    hline(maxY_ - 1, 2, maxY_ - 2);
    vline(2, maxY_, maxX_ - 2);

    printf("\e[m");

    gotoxy(1, (maxY_ - strlen("SNAKE")) / 2);
    puts("SNAKE");

    // gotoxy(maxX, maxY - strlen("v0.1") - 2);
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

    gotoxy(maxX_, 1);
    putchar('+');
    usleep(200000);

    gotoxy(1, maxY_);
    putchar('+');
    usleep(200000);

    gotoxy(maxX_, maxY_);
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

void TextView::setColor(Color color)
{
    //printf ("\e[m");
    printf ("\e[%dm", Adapter::adaptColTView(color));
    return;
}

void TextView::hline(int x, int y, int len) // box
{
    for (int i = 0; i < len; i++)
    {
        gotoxy(x, y + i);
        usleep(10000);
        setColor(Color::YELLOW);
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
        setColor(Color::YELLOW);
        putchar('|');
    }

    return;
}

void TextView::clearScreeen()
{
    gotoxy(0, 0);
    printf("\eH\e[J");
    return;
}

//======================================================================================================================

TextView::~TextView()
{
    sleep(3);
    printf("\e[m");
    gotoxy(1, 1);
    clearScreeen();
    tcsetattr(0, TCSANOW, &oldSettings);
    setvbuf (stdout, NULL, _IOFBF, 0);
    return;
}