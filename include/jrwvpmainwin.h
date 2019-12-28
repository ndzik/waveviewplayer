#ifndef WVPMAINWIN_H
#define WVPMAINWIN_H
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include <wx/wxprec.h>
#include <string>
#include <sstream>
#include <algorithm>
#include <deque>
#include <iostream>
#include <wx/filefn.h>
#include <wx/timer.h>
#include <wx/event.h>
#include <wx/display.h>
#include <wx/stdpaths.h>
#include <wx/scrolwin.h>
#include <wx/mediactrl.h>
#include <wx/dnd.h>
#include <wx/progdlg.h>
#include <wx/popupwin.h>
#include <wx/dir.h>
#include <wx/file.h>
#include <tinyxml2.h>
#include "constants.h"
#include "decoder.h"
#include "listitem.h"
#include "mediaWrap.h"
#include "helpframe.h"
#include "fileDropTarget.h"
#include "sessionmanager.h"
#include "metaPanel.h"

#if defined(__WXMSW__) && !defined(WXUSINGDLL)
#include "wx/link.h"
   wxFORCE_LINK_MODULE(wxmediabackend_am)
   wxFORCE_LINK_MODULE(wxmediabackend_qt)
   wxFORCE_LINK_MODULE(wxmediabackend_wmp10)
#endif // static wxMSW build

class JRWVPMainWin : public wxFrame
{
  public:
    JRWVPMainWin(const wxString& title, const wxPoint& pos,
        const wxSize& size);
    MediaWrap *mediaCtrl;
    wxTimer *m_timer;
  private:
    Decoder decoder;
    wxPanel *p_list_container;
    int cur_nr_of_lists=0;
    int active_mainwin=1;
    bool b_loaded = false;                     // bool guarding wether it_cur_playing iterator is pointing at a valid item
    bool b_loading_files = false;
    bool b_list_visible = true;
    bool b_resized = false;
    bool b_continue = false;
    bool b_tracked = false;
    bool b_stripped = false;
    wxPanel *p_controlbar;
    MetaPanel *p_metapanel;
    wxBoxSizer *p_vmainwinbox;
    wxBoxSizer *p_hboxlist;
    wxBoxSizer *p_controlgrid;
    wxBoxSizer *p_fstcontrol;
    wxBoxSizer *p_sndcontrol;
    wxBoxSizer *p_maindisplay;
    wxSlider *p_progressbar;
    std::deque<JRListItem*> deq_songs;
    std::vector<wxSizer*> vec_sizers;
    std::vector<wxScrolledWindow*> vec_scrolwins;
    std::deque<JRListItem*>::iterator it_cur_playing;
    std::string s_cur_session_path;
    std::string s_cur_cache_path;
    std::string s_cur_cache;
    std::string s_cur_session;
    wxButton *loadDirButton;
    wxButton *loadFileButton;
    wxButton *refreshbutton;
    wxButton *clearbutton;
    wxButton *helpbutton;
    wxButton *exitbutton;
    wxButton *cachebutton;
    wxButton *stripbutton;
    wxButton *contbutton;
    wxButton *sessionbutton;
    wxButton *newbutton;
    wxButton *deletebutton;
    SessionManager *p_sm;
#ifdef _WIN32
    wxStandardPathsBase &stdp = wxStandardPathsBase::Get();
#endif

  public:
    void OnExit(wxCommandEvent& event);
    void OnCache(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnCopyFiles(wxCommandEvent& event);
    void OnManageSession(wxCommandEvent& event);
    void OnMediaLoaded(wxMediaEvent &WXUNUSED(event));
    void OnFD(wxCommandEvent& WXUNUSED(event));
    void OnSF(wxCommandEvent& WXUNUSED(event));
    void OnStrip(wxCommandEvent& WXUNUSED(event));
    void OnPlay(wxCommandEvent& WXUNUSED(event));
    void OnPause(wxCommandEvent& WXUNUSED(event));
    void OnStop(wxCommandEvent& WXUNUSED(event));
    void OnSkip(wxCommandEvent& WXUNUSED(event));
    void OnWinResize(wxSizeEvent& WXUNUSED(event));
    void OnClear(wxCommandEvent& WXUNUSED(event));
    void OnContinue(wxCommandEvent& WXUNUSED(event));
    void OnTimer(wxTimerEvent& WXUNUSED(event));;
    void OnDClickItem(wxCommandEvent& event);
    void OnFinish(wxCommandEvent& WXUNUSED(event));
    void OnHelp(wxCommandEvent& WXUNUSED(event));
    void OnDecodeMe(wxCommandEvent& event);
    void OnUpdatePaths(wxCommandEvent& event);
    void OnIdle(wxIdleEvent& WXUNUSED(event));
    void OnListitemFocus(wxCommandEvent &event);

    bool handleFile(const std::string &file, const int &resolution=256);
    void toggleVisibleList();

    void createList();
    void saveSession();
    void loadSession();
    void clearList();
    void seekMedia(const float &offset);
    bool exists(const std::string &file);
    void manageSession();
    std::string findFileName(const std::string &file);
};
#endif
