#ifndef LISTITEM_H
#define LISTITEM_H
#include <wx/wx.h>
#include <wx/wxprec.h>
#include <wx/gdicmn.h>
#include <wx/kbdstate.h>
#include <wx/dcbuffer.h>
#include <wx/dnd.h>
#include <wx/dialog.h>
#include <string>
#include <sstream>
#include <fstream>
#include <cstdint>
#include <cmath>
#include <vector>
#include "custombutton.h"
#include "jrpopupmenu.h"
#include "constants.h"

/*
 * JRListItem is responsible to hold all necessary information
 * to be displayed as a "soundwave" and adjust the display
 * and Resolution
 */
class JRListItem : public wxWindow
{
  public:
    /* Constructor */
    JRListItem(wxFrame* parent, const std::string &s_path_to_orig_file,
        const std::string &s_audio_name, const std::string &s_path_to_dsampled_file,
        const uint32_t &channels, const uint32_t &sampling_rate,
        const uint32_t &lengthInSeconds, const uint32_t &nr_of_samples,
        const uint32_t &chunk_size, const float &heightfactor,
        const uint32_t &resolution);
    /* Destructor */
    ~JRListItem();

    /* Misc-Functions */
    void adjResHeight();
    void readIntoBuffer();

    /* wxEvents */
    void OnPaint(wxPaintEvent &evt);
    void OnClick(wxMouseEvent &evt);
    void OnTimer(wxTimerEvent &evt);
    void OnKeyDown(wxKeyEvent &evt);
    void OnMouseEnter(wxMouseEvent &evt);
    void OnMouseLeave(wxMouseEvent &evt);
    void OnMouseMotion(wxMouseEvent &evt);
    void OnContextMenu(wxContextMenuEvent &evt);
    void OnPopupClick(wxCommandEvent &evt);
    void OnRezoom(wxMouseEvent &evt);
    void OnHeight(wxMouseEvent &evt);
    void OnIncHeight(wxMouseEvent &evt);
    void OnDecHeight(wxMouseEvent &evt);
    void OnRstHeight(wxMouseEvent &evt);
    void OnLeftDown(wxMouseEvent &evt);
    void OnLeftUp(wxMouseEvent &evt);
    void OnSetFocus(wxFocusEvent &evt);
    void OnKillFocus(wxFocusEvent  &evt);
    void OnResize(wxSizeEvent &evt);
    void OnChildFocus(wxChildFocusEvent  &evt);
    void OnTxtTimer(wxTimerEvent &WXUNUSED(evt));
    void render(wxDC &dc);
    void refresh();

    /* Getters- & Setters */
    void setWidth(const int &val);
    int getWidth();
    void setHeight(const int &val);
    void setPlaying(const bool &val);
    void setProgress(const int &val);
    void setHeightFactor(const float &val);
    float getHeightFactor();
    int getChunkSize();
    int getProgress();
    void setLengthInMS(const int &val);
    int getHeight();
    std::string getOGFilePath();
    std::string getRSFilePath();
    std::string getSongName();
    int getChannels();
    int getLengthInMS();
    int getSamplingRate();
    int getNumberOfSamples();
    int getResolution();
    void setResolution(const int &val);
    void strip();
    void unstrip();
    void visibleButtons(const bool &val);

  private:
    void drawWaveSkeleton(const std::vector<float> &a, int width, int height,
        wxDC *dc);
    void drawWaveFilled(const std::vector<float> &a, int width, wxBrush brsh,
        wxDC *dc);
    void drawWaveFilledLine(const std::vector<float> &a, int width, int height,
        wxDC *dc);
    void drawProgress(const int &lengthInSeconds,
        const wxSize &size, wxDC *dc);
    void drawZoomHint(const wxSize &size, wxDC *dc);
    void drawName(wxFont &font, wxDC *dc);
    void drawHeightFactor(wxFont &font, const int &width, wxDC *dc);
    void drawBorder(const wxSize &size, wxDC *dc);
    void drawZoomedText(const wxSize &size, wxDC *dc);
    void drawInfoText(const wxSize &size, wxFont &font, wxDC *dc);
    void calcAvgOfBuffersamples(const wxSize &size);
    void drawActiveBorder(const wxSize &size, wxDC *dc);
    void resetZoom();
    void resetHeight();
    void changeRes(const bool &input=true);

    wxTimer *p_timer;
    wxTimer *p_txt_offset_timer;
    wxBitmap cached_bitmap = wxNullBitmap;

    std::string s_path_to_orig_file;      // Path to original track
    std::string s_path_to_dsampled_file;      // Path to file containing downsampled information about a track
    std::string s_audio_name;         // The name that should be displayed
    float f_file_offset=0;
    bool b_playing=false;
    bool b_zooming=false;
    bool b_zoomed=false;
    bool b_focused=false;
    bool b_poor_res=false;
    bool b_bitmap_cached=false;
    bool b_dirty=false;             // Tracks if waveform has to be actively redrawn and calc'd
    float arr_tmp_zoom[2];               // Describes begin and end of the samples that have to be observed
    float arr_cur_zoom[2];               // Describes begin and end of the samples that have to be observed
    float HEIGHTFACTOR=1.f;           // Responsible to scale waveheight
    int PADDING=0;                  // Amount of 'nulled' pixels for drawing because #samples < #width
    int TOOLBAR_SPACING=24;
    float NOISEGATE=1;               // Cut noise
    int  CUR_SSS=0;                  // Current amount of samples visible
    int CUR_RESOLUTION=0;
    std::vector<float> vec_samples;  // Buffer for Waveformcoordinates
    std::vector<float> vec_AVG;          // Vector for average values
    std::vector<float> vec_RMS;          // Vector for root mean square values
    int channels;                   // Original Channels
    int length_in_ms;                 // Length in Seconds
    int sampling_rate;               // Original Sampling Rate
    int nr_of_samples;            // Number of all samples
    int chunk_size;                  // Chunksize of downmixed samples, important for maximum zoom level
    int progress;                   // Current progress of song
    int card_height;                // Height of Item
    wxSize text_ext = wxSize(0,0);
    int TEXT_OFFSET = 0;
    int txt_progress = 0;                   // Current progress of song

    /* GUI */
    CustomButton *rst_zoom;
    CustomButton *rst_height;
    CustomButton *inc_height;
    CustomButton *dec_height;
};
#endif
