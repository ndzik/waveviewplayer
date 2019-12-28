#ifndef METAPANEL_H
#define METAPANEL_H
#include <wx/wx.h>
#include <wx/wxprec.h>
#include <wx/dcbuffer.h>
#include "constants.h"
#include "listitem.h"

/*
 * JRListItem is responsible to hold all necessary information
 * to be displayed as a "soundwave" and adjust the display
 * and Resolution
 */
class MetaPanel : public wxPanel
{
  public:
    /* Constructor */
    MetaPanel(wxPanel* parent);
    /* Destructor */
    virtual ~MetaPanel();

    /* Event Functions */
    void OnPaint(wxPaintEvent &evt);

    /* GETTER & SETTER */
    int getMetaState();
    void setMetaState(const int &val);
    wxString getName();
    void setName(const wxString &val);
    int getSampleRate();
    void setSampleRate(const int &val);
    int getChannels();
    void setChannels(const int &val);
    int getLength();
    void setLength(const int &val);
    int getChunkSize();
    void setChunkSize(const int &val);
    int getCurrentResolution();
    void setCurrentResolution(const int &val);

  private:
    int meta_state=0;                 // what will be shown

    /* meta_state == 0 */
    wxString name = "Nothing";
    int sample_rate = 0;
    int channels = 0;
    int length = 0;
    int chunk_size = 0;
    int cur_resolution = 0;

    /* MISC */
    void render(wxDC &dc);
};
#endif
