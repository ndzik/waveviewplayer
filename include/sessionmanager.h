#ifndef H_SESSIONMANAGER
#define H_SESSIONMANAGER
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include <wx/wxprec.h>
#include <wx/listctrl.h>
#include <wx/dir.h>
#include <wx/file.h>
#include <wx/stdpaths.h>
#include <vector>
#include <list>
#include <deque>
#include <sstream>
#include <unordered_map>
#include "constants.h"

struct item
{
  item(wxString name, wxString path_to_file, wxString path_to_cache)
  {
    filename = name;
    filepath = path_to_file;
    cachepath = path_to_cache;
  }
  wxString filename;
  wxString filepath;
  wxString cachepath;
} typedef session_item;

class SessionManager : public wxWindow
{

  public:
    SessionManager(wxFrame *p_parent);
    virtual ~SessionManager();
    void OnLoadSession(wxCommandEvent &evt);
    void OnNewSession(wxCommandEvent &evt);
    void OnDeleteSession(wxCommandEvent &evt);
    void OnActivate(wxListEvent &evt);
    void OnSelect(wxListEvent &evt);
    void OnDeselect(wxListEvent &evt);
    void OnShow(wxShowEvent &evt);
    void OnPaint(wxPaintEvent &evt);

  private:
    wxListCtrl *m_listcontrol;
    wxPanel *m_metapanel;
    wxBoxSizer *vmainboxsizer;
    wxBoxSizer *hsubboxsizer;
    wxBoxSizer *hbuttonboxsizer;
    wxButton *new_button;
    wxButton *delete_button;
    wxButton *load_button;
    std::string s_session_path;
    std::string s_sessionbase_path;
    std::string s_cache_path;
    std::string s_cachebase_path;
    std::list<session_item*> list_session_items;
    std::list<wxString> list_active_items;

    void obtainSessions(std::list <session_item*> &list);
    void loadSession(const wxString &session);
    void populateList();
    void render(wxDC &dc);
};
#endif
