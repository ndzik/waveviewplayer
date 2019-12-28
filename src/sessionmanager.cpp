#include "sessionmanager.h"

SessionManager::SessionManager(wxFrame *p_parent)
  : wxWindow(p_parent, SESSION_MANAGER, wxDefaultPosition, wxDefaultSize
      )
{
  this->SetBackgroundColour(wxColour("#111111"));
  vmainboxsizer = new wxBoxSizer(wxVERTICAL);
  hbuttonboxsizer = new wxBoxSizer(wxHORIZONTAL);
  hsubboxsizer = new wxBoxSizer(wxHORIZONTAL);
  m_metapanel = new wxPanel(this, wxID_ANY, wxDefaultPosition,
    wxDefaultSize);

  m_listcontrol = new wxListCtrl(this, wxID_ANY, wxDefaultPosition,
      wxDefaultSize, wxLC_REPORT |  wxLC_HRULES);

  m_listcontrol->SetBackgroundColour(wxColour("#191919"));


  /* add all existing sessions to vector*/
  obtainSessions(list_session_items);
  populateList();

   /***********/
   /* BUTTONS */
   /***********/
  load_button = new wxButton(this, LOAD_BUTTON, wxT("load"),
      wxDefaultPosition, wxSize(100, 25), wxBORDER_NONE);
  load_button->SetForegroundColour(wxColor(255,255,255));
  load_button->SetBackgroundColour(wxColor(12,12,12));
  load_button->SetToolTip(wxT("Loads a selected session from the list"));
  new_button = new wxButton(this, NEW_BUTTON, wxT("new"),
      wxDefaultPosition, wxSize(100, 25), wxBORDER_NONE);
  new_button->SetForegroundColour(wxColor(255,255,255));
  new_button->SetBackgroundColour(wxColor(12,12,12));
  new_button->SetToolTip(wxT("Create a new session"));
  delete_button = new wxButton(this, DELETE_BUTTON, wxT("delete"),
      wxDefaultPosition, wxSize(100, 25), wxBORDER_NONE);
  delete_button->SetForegroundColour(wxColor(255,255,255));
  delete_button->SetBackgroundColour(wxColor(12,12,12));
  delete_button->SetToolTip(wxT("Deletes selected sessions from list"));
  hbuttonboxsizer->Add(load_button, 0, wxEXPAND | wxALL, 2);
  hbuttonboxsizer->Add(new_button, 0, wxEXPAND | wxALL, 2);
  hbuttonboxsizer->Add(delete_button, 0, wxEXPAND | wxALL, 2);

  /********************/
  /* SIZER HIERARCHIE */
  /********************/

  hsubboxsizer->Add(m_listcontrol, 2, wxEXPAND | wxALL, 10);
  hsubboxsizer->Add(m_metapanel, 1, wxEXPAND | wxALL, 10);
  vmainboxsizer->Add(hsubboxsizer, 1, wxEXPAND | wxALL, 10);
  vmainboxsizer->Add(hbuttonboxsizer, 0, wxEXPAND | wxALL, 10);
  this->SetSizer(vmainboxsizer);

  /***************/
  /* BIND EVENTS */
  /***************/
  Bind(wxEVT_COMMAND_BUTTON_CLICKED, &SessionManager::OnLoadSession,
      this, LOAD_BUTTON);
  Bind(wxEVT_COMMAND_BUTTON_CLICKED, &SessionManager::OnNewSession,
      this, NEW_BUTTON);
  Bind(wxEVT_COMMAND_BUTTON_CLICKED, &SessionManager::OnDeleteSession,
      this, DELETE_BUTTON);
  Bind(wxEVT_LIST_ITEM_SELECTED, &SessionManager::OnSelect,
      this, wxID_ANY);
  Bind(wxEVT_LIST_ITEM_ACTIVATED, &SessionManager::OnActivate,
      this, wxID_ANY);
  Bind(wxEVT_LIST_ITEM_DESELECTED, &SessionManager::OnDeselect,
      this, wxID_ANY);
  m_metapanel->Bind(wxEVT_PAINT, &SessionManager::OnPaint,
      this, m_metapanel->GetId());
}

SessionManager::~SessionManager()
{
  for (session_item *itm : list_session_items)
    delete itm;
}

void SessionManager::obtainSessions(std::list<session_item*> &list)
{
  wxArrayString arr_sessions;
#ifdef _WIN32
  std::stringstream documents;
  wxStandardPathsBase &stdp = wxStandardPaths::Get();
  wxString docs = stdp.GetDocumentsDir();
  documents << docs << "\\wfv\\sessions";
  s_session_path = documents.str();
  documents << "\\";
  s_sessionbase_path = documents.str();
  documents.str(std::string());
  documents << docs << "\\wfv\\cache";
  s_cache_path = documents.str();
  documents << "\\";
  s_cachebase_path = documents.str();
#elif __linux__
  std::stringstream documents;
  wxString home = wxGetUserHome();
  documents << home << "/.config/wfv";
  s_session_path = documents.str();
  documents << "/";
  s_sessionbase_path = documents.str();
  documents.str(std::string());
  documents << home << "/.cache/wfv";
  s_cache_path = documents.str();
  documents << "/";
  s_cachebase_path = documents.str();
#endif

  /* Get all files from folder */
  wxDir dir;
  wxFile file;
  wxString str;
  dir.GetAllFiles(s_session_path, &arr_sessions, "*.xml");

  std::string filename;
  std::string tmp_cache;
  for (wxString file : arr_sessions)
  {
#ifdef __linux__
    filename = file.substr(file.find_last_of('/') + 1);
    filename = filename.substr(0, filename.find_last_of('.'));
    tmp_cache = s_cachebase_path  + filename + '/';
#elif _WIN32
    filename = file.substr(file.find_last_of('\\') + 1);
    filename = filename.substr(0, filename.find_last_of('.'));
    tmp_cache = s_cachebase_path + filename + '\\';
#endif
    list.push_back(new session_item(filename, file, tmp_cache));
  }
}

void SessionManager::populateList()
{
  std::list<session_item*>::iterator it = list_session_items.begin();

  wxListItem itemCol1;
  itemCol1.SetText(_T("Sessions"));
  itemCol1.SetWidth(100);
  m_listcontrol->InsertColumn(0, itemCol1);

  wxListItem itemCol2;
  itemCol2.SetText(_T("Path"));
  itemCol2.SetWidth(250);
  m_listcontrol->InsertColumn(1, itemCol2);

  if (list_session_items.size() > 0)
    for (int i=0; i<list_session_items.size(); ++i)
    {
      /* Fill 1st column */
      wxListItem list_name;
      list_name.SetId(i);
      list_name.SetColumn(0);
      list_name.SetWidth(300);
      list_name.SetText((*it)->filename);
      list_name.SetTextColour(wxColour("#F1F1F1"));
      list_name.SetBackgroundColour(wxColour("#212121"));
      m_listcontrol->InsertItem(list_name);

      /* Fill 2nd column */
      m_listcontrol->SetItem(i, 1, (*it)->filepath, -1);

      it++;
    }
}

void SessionManager::loadSession(const wxString &session)
{
  wxCommandEvent evt(UPDATE_PATH_EVT, GetId());
  std::list<session_item*>::iterator it = list_session_items.begin();

  while (it != list_session_items.end())
  {
    if ((*it)->filename == session)
    {
      std::cout << "filename: " << (*it)->filename << std::endl;
      std::cout << "filepath: " << (*it)->filepath << std::endl;
      std::cout << "cachepath: " << (*it)->cachepath << std::endl;
      evt.SetClientData(*it);
      evt.SetEventObject(this);
      ProcessWindowEvent(evt);
      return;
    }
    ++it;
  }
  list_active_items.clear();
  list_session_items.clear();
}

void SessionManager::render(wxDC &dc)
{
  wxString txt;
  wxSize size = m_metapanel->GetClientSize();
  wxFont font = GetFont();
  font.SetFamily(wxTELETYPE);
  font.SetPointSize(12);
  dc.SetFont(font);
  dc.SetTextForeground(wxColour(230,230,230));

  dc.SetFont(font.Bold());
  dc.SetTextForeground(wxColour(247,250,70));
  txt.Printf("JR's WaveViewPlayer");
  dc.DrawText(txt, 0, 0);
  dc.SetFont(font);
  dc.SetTextForeground(wxColour(230,230,230));
  txt.Printf("Welcome to JRWVP. Here you\n"
             "can manage your sessions and\n"
             "use them as playlists if you\n"
             "wish to do so.\n"
             "\n"
             "For help on keybindings press\n"
             "the helpbutton in the\n"
             "top-right corner.\n"
  );
  dc.DrawText(txt, 0, 15);
  dc.SetFont(font.Bold());
  dc.SetTextForeground(wxColour(247,250,70));
  txt.Printf("CHANGELOG v0.4.3");
  dc.DrawText(txt, 0, 170);
  dc.SetFont(font);
  dc.SetTextForeground(wxColour(230,230,230));
  txt.Printf("- Fixed flickering issues\n"
             "  of GUI-Elements\n"
             "- Improved responsiveness\n"
             "  of GUI\n"
             "- Overall change in style\n"
             "- More bugfixes because I\n"
             "  was dumb when starting\n"
             "  out with this framework\n"
             "- Reduced visual clutter\n"
  );
  dc.DrawText(txt, 0, 200);
  dc.SetTextForeground(wxColour(247,250,70));
  txt.Printf("IMPORTANT NOTE:");
  dc.DrawText(txt, 0, 380);
  dc.SetFont(font);
  dc.SetTextForeground(wxColour(230,230,230));
  txt.Printf("Please just delete old setup\n"
             "files/sessions if you start\n"
             "using a new version.\n"
             "An automated way will be\n"
             "implemented in the future (:\n"
  );
  dc.DrawText(txt, 0, 400);
}

/************************/
/* EVENT IMPLEMENTATION */
/************************/

void SessionManager::OnPaint(wxPaintEvent &evt)
{
  wxPaintDC dc(m_metapanel);
  render(dc);
}

/*
 * Sends an update event to mainframe passing updated cache and
 * session path
 */
void SessionManager::OnLoadSession(wxCommandEvent &evt)
{
  if (list_active_items.size() < 1)
  {
    wxMessageBox("Please select only a single session to load");
    return;
  }
  loadSession(list_active_items.front());
}

void SessionManager::OnNewSession(wxCommandEvent &evt)
{
    /* Ask for user specified session name */
    wxString user_input = wxGetTextFromUser("Give your session a name",
        wxEmptyString, wxEmptyString, this);

    if (user_input == wxEmptyString)
    {
      wxMessageBox("Please provide a valid session-name");
      return;
    }
    std::stringstream tmp;
    std::stringstream tmp_cache;
    tmp << s_sessionbase_path << user_input << ".xml";
#ifdef __linux__
    tmp_cache << s_cachebase_path  << user_input << '/';
#elif _WIN32
    tmp_cache << s_cachebase_path << user_input << '\\';
#endif

    /* Create folder if not already existing */
    if (!wxDirExists(tmp_cache.str()))
      wxMkdir(tmp_cache.str());
    if (!wxFileExists(tmp.str()))
      wxFile().Create(tmp.str());

    list_session_items.push_back(new session_item(user_input,
      tmp.str(), tmp_cache.str()));
    m_listcontrol->ClearAll();
    list_active_items.clear();
    populateList();
}

/*
 * Deletes directories and setup-files connected to selected
 * session.
 */
void SessionManager::OnDeleteSession(wxCommandEvent &evt)
{
  if (!(list_active_items.size() >= 1))
    return;

  std::list<session_item*>::iterator it;
  for (wxString active : list_active_items)
  {
    it = list_session_items.begin();
    while ((*it)->filename != active && it != list_session_items.end())
      it++;
    std::cout << "Found item" << std::endl;
    std::cout << "filename: " << (*it)->filename << std::endl;
    std::cout << "active: " << active << std::endl;

    if ((*it)->filename == active)
    {
      std::remove((*it)->filepath);
      if(wxDirExists(s_cachebase_path + (*it)->filename))
      {
        wxArrayString files;
        wxDir().GetAllFiles(s_cachebase_path + (*it)->filename, &files);

        for (wxString file : files)
          wxRemoveFile(file);

        wxRmdir(s_cachebase_path + (*it)->filename);
      }
      delete (*it);
      list_session_items.erase(it);
    }
  }
  list_active_items.clear();
  m_listcontrol->ClearAll();
  populateList();
}

void SessionManager::OnSelect(wxListEvent &evt)
{
  wxListItem itm = evt.GetItem();
  list_active_items.push_back(itm.GetText());
  std::cout << "Currently selected" << std::endl;
  for (wxString s : list_active_items)
    std::cout << s << std::endl;
}

void SessionManager::OnDeselect(wxListEvent &evt)
{
  wxListItem itm = evt.GetItem();
  std::list<wxString>::iterator it = list_active_items.begin();

  while (*it != itm.GetText() && it != list_active_items.end())
    it++;

  if (*it == itm.GetText())
    list_active_items.erase(it);
}

void SessionManager::OnShow(wxShowEvent &evt)
{
  list_active_items.clear();
  m_listcontrol->ClearAll();
  populateList();
}

/*
 * Handles double-click event on a session
 * essentially doing the same as the button
 * [load] does
 */
void SessionManager::OnActivate(wxListEvent &evt)
{
  wxListItem itm = evt.GetItem();
  loadSession(itm.GetText());
}
