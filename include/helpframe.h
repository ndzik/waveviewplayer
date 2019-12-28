#ifndef HELPFRAME_H
#define HELPFRAME_H
#include <wx/wx.h>
#include <wx/wxprec.h>
#include <wx/panel.h>
#include <wx/dnd.h>
#include <wx/busyinfo.h>
#include <wx/frame.h>
#include <sstream>
#include "constants.h"

class JRWVPMainWin;

class HelpFrame : public wxFrame
{
  friend class JRWVPMainWin;

public:
  HelpFrame(wxWindow* p_parent, const wxString& title, const wxPoint& pos, const wxSize& size);

private:
  wxPanel *text_panel;
  wxSize txt_size;
  std::stringstream ss_helptxt;
  void render(wxDC& dc);
  void OnPaint(wxPaintEvent& evt);
};
#endif
