#include "helpframe.h"


HelpFrame::HelpFrame(wxWindow* p_parent, const wxString& title,
    const wxPoint& pos, const wxSize& size)
  : wxFrame(p_parent, wxID_ANY, title, pos, size)
{
  ss_helptxt <<
    "Welcome to JR's WaveViewPlayer\n"
    "\n"
    "This application is still in its earliest shoes and alot of fixes need to be done.\n"
    "\n"
    "USAGE:\n"
    "\n"
    "     Magnify Snippet     - (hold) <Shift>+LMB -> Drag to desired location and release\n"
    "\n"
    "     Reset Snippet       - Hover with mouse over song, then press <R>\n"
    "                         HOTKEY: <R>\n"
    "                         alternative: Press button [R]\n"
    "\n"
    "     Adjust Height       - (hold) <Shift>+Mousewheel\n"
    "                         alternative: Press buttons [+]/[-]\n"
    "\n"
    "     Reset Height        - Hover with mouse over song, then press <Ctrl+R>\n"
    "                         HOTKEY: <Ctrl+R>\n"
    "                         alternative: Press button [o]\n"
    "\n"
    "     Adjust Resolution   - Hover with mouse over song, then press <Ctrl+Shift+X>\n"
    "                         HOTKEY: <Ctrl+Shift+X>\n"
    "                         alternative: Right-Click -> 'Change Resolution'\n"
    "\n"
    "     Strip               - Tries to zoom into each audiofile and get rid of zerovalues\n"
    "                         in the beginning and the end. Decodes a file to a higher\n"
    "                         more fine grained resolution if necessary\n"
    "\n"
    "     Sessions            - Manage your sessions. Either use the button [load] to load\n"
    "                         a selected session from the list, or just Double-Click\n"
    "                         your desired session.\n"
    "\n"
    "     Play                - Double_Click on a song\n"
    "\n"
    "     Seek                - <Ctrl>+Double_Click on desired position within the song \n"
    "                                 (SONG HAS TO BE RUNNING)\n"
    "";

  this->SetBackgroundColour(wxColour(wxT("#0b0b0b")));
  Bind(wxEVT_PAINT, &HelpFrame::OnPaint, this, wxID_ANY);
}

void HelpFrame::OnPaint(wxPaintEvent& WXUNUSED(evt))
{
  wxPaintDC dc(this);
  render(dc);
}

void HelpFrame::render(wxDC& dc)
{
  wxFont font = GetFont();
  font.SetFamily(wxTELETYPE);
  font.SetPointSize(14);
  dc.SetTextForeground(wxColour(247,250,70));
  dc.SetFont(font);
  txt_size = dc.GetMultiLineTextExtent(ss_helptxt.str());
  SetSize(wxSize(txt_size.GetWidth() + 20, txt_size.GetHeight() + 20));

  dc.DrawText(ss_helptxt.str(), 0, 0);
}
