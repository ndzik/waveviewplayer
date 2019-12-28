#include "custombutton.h"

CustomButton::CustomButton(wxWindow* parent, const int &id, const wxString &label) : wxWindow(parent, id, wxDefaultPosition, wxSize(20,20), wxFULL_REPAINT_ON_RESIZE)
{
  this->label = label;
  this->p_parent = parent;
  Bind(wxEVT_PAINT, &CustomButton::OnPaint, this, id);
  Bind(wxEVT_LEFT_DOWN, &CustomButton::OnClick, this, id);
  Bind(wxEVT_LEFT_UP, &CustomButton::OnRelease, this, id);
  SetBackgroundColour(wxColor(20,20,20));
  SetBackgroundStyle(wxBG_STYLE_PAINT);
}

CustomButton::~CustomButton(){}

void CustomButton::OnPaint(wxPaintEvent &WXUNUSED(evt))
{
  wxBufferedPaintDC dc(this);
  render(dc);
}

void CustomButton::OnClick(wxMouseEvent &evt)
{
  SetBackgroundColour(wxColour(wxT("#444343")));
  evt.ResumePropagation(wxEVENT_PROPAGATE_MAX);
  evt.Skip();
}

void CustomButton::OnRelease(wxMouseEvent &evt)
{
  SetBackgroundColour(wxColour(wxT("#070707")));
  evt.ResumePropagation(wxEVENT_PROPAGATE_MAX);
  evt.Skip();
}

void CustomButton::render(wxDC& dc)
{
  dc.Clear();
  wxFont font = dc.GetFont();
  font.SetPointSize(14);
  dc.SetFont(font);
  dc.SetTextForeground(wxColour(220,220,220));
  wxSize txt_size = dc.GetTextExtent(label);
  dc.DrawText(label, 10-txt_size.GetWidth()/2, 10-txt_size.GetHeight()/2);
}
