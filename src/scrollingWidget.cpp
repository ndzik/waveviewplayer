#include "scrollingWidget.h"


ScrolledWidgetsPane::ScrolledWidgetsPane(wxWindow* parent, wxWindowID id) : wxScrolledWindow(parent, id)
{
    // this part makes the scrollbars show up
    this->SetScrollRate(0, 25);
}
