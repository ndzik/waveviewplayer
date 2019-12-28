#ifndef CUSTOMBUTTON_H
#define CUSTOMBUTTON_H
#include <wx/wx.h>
#include <wx/wxprec.h>
#include <wx/dcbuffer.h>
#include "constants.h"

/*
 * JRListItem is responsible to hold all necessary information
 * to be displayed as a "soundwave" and adjust the display
 * and Resolution
 */
class CustomButton : public wxWindow
{
  public:
    /* Constructor */
    CustomButton(wxWindow* parent, const int &id, const wxString &label);
    /* Destructor */
    ~CustomButton();

  private:
    /* attr */
    std::string label;
    wxWindow *p_parent;
    /* funcs */
    void OnPaint(wxPaintEvent &evt);
    void OnClick(wxMouseEvent &evt);
    void OnRelease(wxMouseEvent &evt);
    void render(wxDC& dc);
};
#endif
