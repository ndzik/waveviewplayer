#include "mediaWrap.h"

MediaWrap::MediaWrap(JRWVPMainWin* parentFrame, wxWindowID winid,
  bool looping)
  : wxPanel((wxFrame*) parentFrame, wxID_ANY, wxDefaultPosition, wxDefaultSize,
    wxFULL_REPAINT_ON_RESIZE),
	m_parentFrame(parentFrame),
	isBeingDragged(false)
{
  m_looping = looping;

  /*
   * LAYOUT
   *  [MEDIA_SHOW]	[PLAY/PAUSE] [STOP] [NEXT]
   *  [VOL-------]	[PROGRESS----------------]
   */

  wxFlexGridSizer *sizer = new wxFlexGridSizer(2);
  sizer->AddGrowableCol(1);
  this->SetSizer(sizer);

  m_mediaCtrl = new wxMediaCtrl();

  /*
   * wxMEDIABACKEND_DIRECTSHOW does not fking send a wxEVT_MEDIA_LOADED
   * event on WINDOWS. I am dying right now, took to long to figure out
   */
  bool bOK = m_mediaCtrl->Create(this, winid, wxEmptyString,
	  wxDefaultPosition, wxDefaultSize, 0,
#ifdef _WIN32
	  wxMEDIABACKEND_WMP10);
#elif __linux__
	  wxEmptyString);
#endif

  wxASSERT_MSG(bOK, "Could not create media control!");
  wxUnusedVar(bOK);

  sizer->Add(m_mediaCtrl, wxSizerFlags().Expand().Border());

  /* Control Buttons */
  wxBoxSizer* horsizer1 = new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer* vertsizer = new wxBoxSizer(wxHORIZONTAL);

  m_playButton = new wxButton();
  m_stopButton = new wxButton();
  m_nextButton = new wxButton();

  m_playButton->Create(this, PLAY_BUTTON, ">", wxDefaultPosition,
      wxDefaultSize, wxBORDER_NONE);
  m_playButton->SetBackgroundColour(wxColor(20,20,20));
  m_playButton->SetForegroundColour(wxColor(255,255,255));
  m_playButton->SetToolTip("Play");
  m_stopButton->Create(this, PAUSE_BUTTON, "[]", wxDefaultPosition,
      wxDefaultSize, wxBORDER_NONE);
  m_stopButton->SetBackgroundColour(wxColor(20,20,20));
  m_stopButton->SetForegroundColour(wxColor(255,255,255));
  m_stopButton->SetToolTip("Stop");
  m_nextButton->Create(this, SKIP_BUTTON, ">>", wxDefaultPosition,
      wxDefaultSize, wxBORDER_NONE);
  m_nextButton->SetBackgroundColour(wxColor(20,20,20));
  m_nextButton->SetForegroundColour(wxColor(255,255,255));
  m_nextButton->SetToolTip("Next");

  vertsizer->Add(m_playButton, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
  vertsizer->Add(m_stopButton, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
  vertsizer->Add(m_nextButton, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
  horsizer1->Add(vertsizer, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
  sizer->Add(horsizer1, 0,
      wxALIGN_CENTER_VERTICAL|wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

  /* Progressbar */
  m_slider = new wxSlider(this, wxID_SLIDER, 0, 0, 1000, wxDefaultPosition,
      wxDefaultSize, wxSL_HORIZONTAL);

  /* Volume slider */
  wxBoxSizer *horsizer3 = new wxBoxSizer(wxHORIZONTAL);

  m_volSlider = new wxSlider(this, wxID_VOLSLIDER, 100,
    0,
    100,
    wxDefaultPosition, wxSize(100, 20),
    wxSL_HORIZONTAL );
  sizer->Add(m_volSlider, 0, wxALIGN_CENTER_HORIZONTAL | wxALL | wxEXPAND, 5);
  horsizer3->Add(m_slider, 1, wxALL | wxEXPAND, 5);

  sizer->Add(horsizer3, 0, wxCENTRE | wxALL | wxEXPAND, 5);

  /* BIND ALL EVENTS */

  /* SLIDER EVENTS */
  Bind(wxEVT_SCROLL_THUMBTRACK, &MediaWrap::OnBeginSeek, this,
       wxID_SLIDER);
  Bind(wxEVT_SCROLL_THUMBRELEASE, &MediaWrap::OnEndSeek, this,
       wxID_SLIDER);
  Bind(wxEVT_SCROLL_THUMBRELEASE, &MediaWrap::OnVolChange, this,
       wxID_VOLSLIDER);

  /* PAINT EVENTS */
  Bind(wxEVT_PAINT, &MediaWrap::OnPaint, this, GetId());

  /* MEDIA CTRL EVENTS */
  Bind(wxEVT_MEDIA_PLAY, &MediaWrap::OnMediaPlay, this, MEDIA_CTRL);
  Bind(wxEVT_MEDIA_PAUSE, &MediaWrap::OnMediaPause, this, MEDIA_CTRL);

  /* TIMER EVENTS */
  Bind(wxEVT_TIMER, &MediaWrap::OnTimer, this, MEDIA_TIMER_ID);

  /* Initialization */
  m_timer = new wxTimer(this, MEDIA_TIMER_ID);
  m_timer->Start(250);
}

MediaWrap::~MediaWrap()
{
  m_timer->Stop();
}

/********/
/* MISC */
/********/

void MediaWrap::render(wxDC &dc)
{
  dc.Clear();
  wxSize size = GetClientSize();
  wxFont font = GetFont();
  font.SetFamily(wxTELETYPE);
  font.SetPointSize(10);
  dc.SetFont(font);
  dc.SetTextForeground(wxColour(230,230,230));

  wxLongLong llLength = m_mediaCtrl->Length();
  int nMinutes = (int)(llLength / 60000).GetValue();
  int nSeconds = (int)((llLength % 60000) / 1000).GetValue();

  // Duration string
  wxString sDuration;
  sDuration.Printf("%2i:%02i", nMinutes, nSeconds);

  // Number of minutes/seconds total
  nMinutes = (int)(progress / 60000);
  nSeconds = (int)((progress % 60000) / 1000);

  // Position string
  wxString sPosition;
  sPosition.Printf("%2i:%02i", nMinutes, nSeconds);

  wxString txt;
  txt.Printf("%s/%s", sPosition, sDuration);

  dc.DrawText(txt, GetClientSize().GetWidth()/2,
      GetClientSize().GetHeight()/2);

  if (progress != old_progress)
  {
    old_progress = progress;
    Refresh();
  }
}

/**********/
/* EVENTS */
/**********/

void MediaWrap::OnTimer(wxTimerEvent &evt)
{
  if (old_progress != progress)
  {
    old_progress = progress;
    Refresh();
  }
}

void MediaWrap::OnPaint(wxPaintEvent &evt)
{
  wxPaintDC dc(this);
  render(dc);
}

void MediaWrap::OnMediaPlay(wxMediaEvent &WXUNUSED(event))
{
  m_playButton->SetLabelText("||");
  m_playButton->SetToolTip("Pause");
}

void MediaWrap::OnMediaPause(wxMediaEvent &WXUNUSED(event))
{
  m_playButton->SetLabelText(">");
  m_playButton->SetToolTip("Play");
}

void MediaWrap::OnVolChange(wxScrollEvent &WXUNUSED(event))
{
  if (!m_mediaCtrl->SetVolume(
        m_volSlider->GetValue() / 100.0) )
    wxMessageBox("Could not set volume!");
}

void MediaWrap::OnMediaFinished(wxMediaEvent &WXUNUSED(evt))
{
  if (m_looping)
    m_mediaCtrl->Play();
}

void MediaWrap::OnBeginSeek(wxScrollEvent &WXUNUSED(event))
{
  isBeingDragged = true;
}

void MediaWrap::OnEndSeek(wxScrollEvent &WXUNUSED(event))
{
  if (m_mediaCtrl->Seek(m_slider->GetValue() * 1000) == wxInvalidOffset)
    wxMessageBox("Couldn't seek song");

  isBeingDragged = false;
}

bool MediaWrap::IsBeingDragged()
{
  return isBeingDragged;
}

void MediaWrap::setProgress(const int &val)
{
  this->progress = val;
}
