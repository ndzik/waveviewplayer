#ifndef MEDIAWRAP_H
#define MEDIAWRAP_H
#include <wx/wx.h>
#include <wx/wxprec.h>
#include <wx/mediactrl.h>
#include <wx/panel.h>
#include <wx/dcbuffer.h>
#include "constants.h"

class MediaWrap : public wxPanel
{
public:
  friend class JRWVPMainWin;
  MediaWrap(class JRWVPMainWin* parentFrame, wxWindowID winid,
      bool looping=false);
  ~MediaWrap();

  bool IsBeingDragged();

  /* Media event handlers */
  void OnMediaPlay(wxMediaEvent &evt);
  void OnMediaPause(wxMediaEvent &evt);
  void OnMediaFinished(wxMediaEvent &evt);
  void OnBeginSeek(wxScrollEvent &evt);
  void OnEndSeek(wxScrollEvent &evt);
  void OnVolChange(wxScrollEvent &evt);
  void OnPaint(wxPaintEvent &evt);
  void OnTimer(wxTimerEvent &evt);
  void render(wxDC &dc);
  void setProgress(const int &val);

  wxMediaCtrl *m_mediaCtrl;
  bool m_looping = false;
  std::string sCurPlayingFile;            // Name of currently playing file
  wxSlider* m_slider;                     // Slider below our media-control
  wxSlider* m_volSlider;                  // Slider next to progressbar for volume
  bool isBeingDragged;                    // Wether user is dragging progressbar
  class JRWVPMainWin *m_parentFrame;      // Main wxFrame for this Application
  wxButton* m_playButton;                 // Play/Pause file button
  wxButton* m_stopButton;                 // Stop playing file button
  wxButton* m_nextButton;                 // Next file button
  wxTimer* m_timer;

  int progress;
  int old_progress = 0;
};
#endif
