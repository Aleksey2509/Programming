#include "view.hpp"
#include "textView.hpp"
#include "view.hpp"
#include "graphView.hpp"
#include "textView.hpp"

AbstractView* AbstractView::viewObj = nullptr;

// AbstractView::AbstractView()
// {

// }
// AbstractView::~AbstractView()
// {
//     if (viewObj != nullptr)
//         delete viewObj;
// }

AbstractView* AbstractView::getView(const char* viewType)
{
    if (viewObj)
        return viewObj;
    
    if (viewType == nullptr)
        return nullptr;

    if (!strcmp(viewType,"graph"))
    {
        viewObj = new GraphView;
        return viewObj;
    }
    else
    {
        viewObj = new TextView;
        return viewObj;
    }
}