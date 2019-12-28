#include "metaPanel.h"

/* Constructor */
MetaPanel::MetaPanel(wxPanel* parent) : wxPanel(parent, META_PANEL, wxDefaultPosition, wxDefaultSize, 80)
{
  this->SetBackgroundColour(wxColour(_("#1D1B1B")));

  Bind(wxEVT_PAINT, &MetaPanel::OnPaint, this, META_PANEL);
}

/* Destructor */
MetaPanel::~MetaPanel(){}

/* MISC */
void MetaPanel::render(wxDC &dc)
{
  dc.Clear();

  wxString txt;
  wxSize size = GetClientSize();
  wxFont font = GetFont();
  font.SetFamily(wxTELETYPE);
  font.SetPointSize(8);
  dc.SetFont(font);
  dc.SetTextForeground(wxColour(230,230,230));

  /* name */
  dc.DrawText(name, 5, 2);

  /* sample rate */
  txt.Printf("Samplerate: %i", sample_rate);
  dc.DrawText(txt, 5, 12);

  /* channels */
  txt.Printf("Channels: %i", channels);
  dc.DrawText(txt, 5, 22);

  /* resolution */
  txt.Printf("Resolution: %i", cur_resolution);
  dc.DrawText(txt, 5, 32);

  /* length */
  int nMinutes = (int)(length / 60000);
  int nSeconds = (int)((length % 60000) / 1000);
  txt.Printf("Length: %2i:%02i", nMinutes, nSeconds);
  dc.DrawText(txt, 5, 42);
}

/* EVENT FUNCTIONS */
void MetaPanel::OnPaint(wxPaintEvent &evt)
{
  wxBufferedPaintDC dc(this);
  render(dc);
}

/* GETTER & SETTER */
int MetaPanel::getMetaState()
{
  return this->meta_state;
}
void MetaPanel::setMetaState(const int &val)
{
  this->meta_state = val;
}

wxString MetaPanel::getName()
{
  return this->name;
}
void MetaPanel::setName(const wxString &val)
{
  this->name = val;
}

int MetaPanel::getSampleRate()
{
  return this->sample_rate;
}
void MetaPanel::setSampleRate(const int &val)
{
  this->sample_rate = val;
}

int MetaPanel::getChannels()
{
  return this->channels;
}
void MetaPanel::setChannels(const int &val)
{
  this->channels = val;
}

int MetaPanel::getLength()
{
  return this->length;
}
void MetaPanel::setLength(const int &val)
{
  this->length = val;
}

int MetaPanel::getChunkSize()
{
  return this->chunk_size;
}
void MetaPanel::setChunkSize(const int &val)
{
  this->chunk_size = val;
}

int MetaPanel::getCurrentResolution()
{
  return this->cur_resolution;
}
void MetaPanel::setCurrentResolution(const int &val)
{
  this->cur_resolution = val;
}
