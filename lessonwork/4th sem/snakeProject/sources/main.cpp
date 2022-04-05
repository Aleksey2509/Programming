#include <cstdio>
#include "controller.hpp"
#include "view.hpp"
#include "model.hpp"

int main(int argc, char* argv[])
{
    AbstractView* v;
    if (argc == 1)
        v = AbstractView::getView("text");
    else
    {
        v = AbstractView::getView(argv[1]);
    }
    Model model;
    Human humanAI(model.snake);
    v->run();

    // sleep(1);

    delete v;

    return 0;
}