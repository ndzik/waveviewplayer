#ifndef SCROLL_PANEL_H
#define SCROLL_PANEL_H
#include <wx/wx.h>
#include "constants.h"

class ScrolledWidgetsPane : public wxScrolledWindow
{
public:
    ScrolledWidgetsPane(wxWindow* parent, wxWindowID id);
};
#endif
