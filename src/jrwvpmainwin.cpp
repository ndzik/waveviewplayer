#include "jrwvpmainwin.h"

wxDEFINE_EVENT(SONG_ACTIVATED, wxCommandEvent);
wxDEFINE_EVENT(TELL_SEEK, wxCommandEvent);
wxDEFINE_EVENT(DECODE_ME_EVT, wxCommandEvent);
wxDEFINE_EVENT(UPDATE_PATH_EVT, wxCommandEvent);
wxDEFINE_EVENT(LIST_ITEM_IN_FOCUS_EVT, wxCommandEvent);

/**
 * Mainwindow for JRWVP
 * Allows to Play/Pause/Skip Music in a List
 * List displays waveforms of each audio file
 * If window_width > Threshold; Display +1 more Column of music items
 */
JRWVPMainWin::JRWVPMainWin(const wxString& title, const wxPoint& pos,
    const wxSize& size)
  : wxFrame(NULL, wxID_ANY, title, pos, size)
{
  /**********/
  /* COLORS */
  /**********/

  wxColour col1, col2, col3;
  col1.Set(wxT("#4f5049"));
  col2.Set(wxT("#111111"));
  col3.Set(wxT("#113366"));

  /*******/
  /* GUI */
  /*******/

  SetMinSize(wxSize(1000, 800));
  this->SetBackgroundColour(col1);

  p_vmainwinbox = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *prgrsbarbox = new wxBoxSizer(wxVERTICAL);
  p_hboxlist = new wxBoxSizer(wxHORIZONTAL);

  p_maindisplay = new wxBoxSizer(wxHORIZONTAL);
  p_controlbar = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);
  p_metapanel = new MetaPanel(p_controlbar);
  p_list_container = new wxPanel(this, wxID_ANY, wxDefaultPosition,
      wxDefaultSize, wxFULL_REPAINT_ON_RESIZE | wxWANTS_CHARS);

  p_controlbar->SetBackgroundColour(wxColour(_("#1D1B1B")));
  p_list_container->SetBackgroundColour(col2);
  p_list_container->SetSizer(p_hboxlist);

  /* Add drag&drop capability */
  p_list_container->SetDropTarget(new DDSoundFile(this, p_list_container));

  p_vmainwinbox->Add(p_controlbar, 0, wxEXPAND | wxALL);
  p_maindisplay->Add(p_list_container, 1, wxEXPAND | wxTOP, 5);
  p_vmainwinbox->Add(p_maindisplay, 1, wxEXPAND | wxTOP, 5);
  this->SetSizer(p_vmainwinbox);

  /*****************/
  /** MAINDISPLAY **/
  /*****************/
  p_sm = new SessionManager(this);
  p_sm->Hide();

  /*********************/
  /* BUTTONS & CONTROL */
  /*********************/

  loadDirButton = new wxButton(p_controlbar, LOAD_DIR_BUTTON,
      wxT("Add Dir"), wxDefaultPosition, wxSize(100,25), wxBORDER_NONE);
  loadDirButton->SetBackgroundColour(wxColor(20,20,20));
  loadDirButton->SetForegroundColour(wxColor(255,255,255));
  loadDirButton->SetToolTip("Choose a Directory to load Files from");
  loadFileButton = new wxButton(p_controlbar, LOAD_FILE_BUTTON,
      wxT("Add File"), wxDefaultPosition, wxSize(100,25), wxBORDER_NONE);
  loadFileButton->SetBackgroundColour(wxColor(20,20,20));
  loadFileButton->SetForegroundColour(wxColor(255,255,255));
  loadFileButton->SetToolTip("Choose a file to load");
  refreshbutton = new wxButton(p_controlbar, REFRESH_BUTTON,
      wxT("Refresh"), wxDefaultPosition, wxSize(100,25), wxBORDER_NONE);
  refreshbutton->SetBackgroundColour(wxColor(20,20,20));
  refreshbutton->SetForegroundColour(wxColor(255,255,255));
  refreshbutton->SetToolTip("Refresh List");
  clearbutton = new wxButton(p_controlbar, CLEAR_BUTTON,
      wxT("Clear List"), wxDefaultPosition, wxSize(100,25), wxBORDER_NONE);
  clearbutton->SetBackgroundColour(wxColor(20,20,20));
  clearbutton->SetForegroundColour(wxColor(255,255,255));
  clearbutton->SetToolTip("Clear all Items");
  exitbutton = new wxButton(p_controlbar, wxID_EXIT,
      wxT("Exit"), wxDefaultPosition, wxSize(100,25), wxBORDER_NONE);
  exitbutton->SetBackgroundColour(wxColor(20,20,20));
  exitbutton->SetForegroundColour(wxColor(255,255,255));
  cachebutton = new wxButton(p_controlbar, CACHE_BUTTON,
      wxT("Clear Cache"), wxDefaultPosition, wxSize(100,25), wxBORDER_NONE);
  cachebutton->SetBackgroundColour(wxColor(20,20,20));
  cachebutton->SetForegroundColour(wxColor(255,255,255));
  cachebutton->SetToolTip("Clears all cached soundwave data for current session");
  stripbutton = new wxButton(p_controlbar, STRIP_BUTTON,
      wxT("Strip"), wxDefaultPosition, wxSize(100,25), wxBORDER_NONE);
  stripbutton->SetBackgroundColour(wxColor(20,20,20));
  stripbutton->SetForegroundColour(wxColor(255,255,255));
  stripbutton->SetToolTip("Zooms into each audiofile stripping zeros");
  contbutton = new wxButton(p_controlbar, CONT_BUTTON,
      wxT("Single"), wxDefaultPosition, wxSize(100,25), wxBORDER_NONE);
  contbutton->SetBackgroundColour(wxColor(20,20,20));
  contbutton->SetForegroundColour(wxColor(255,255,255));
  contbutton->SetToolTip("Wether next song in list should be played "
    "automatically");
  sessionbutton = new wxButton(p_controlbar, SESSION_BUTTON,
      wxT("Sessions"), wxDefaultPosition, wxSize(100,25), wxBORDER_NONE);
  sessionbutton->SetBackgroundColour(wxColor(20,20,20));
  sessionbutton->SetForegroundColour(wxColor(255,255,255));
  sessionbutton->SetToolTip("Manage your sessions");
  helpbutton = new wxButton(p_controlbar, HELP_BUTTON,
      wxT("Help"), wxDefaultPosition, wxSize(100,25), wxBORDER_NONE);
  helpbutton->SetBackgroundColour(wxColor(20,20,20));
  helpbutton->SetForegroundColour(wxColor(255,255,255));
  helpbutton->SetToolTip("Help & Keybinds");

  m_timer = new wxTimer(this, TIMER_ID);

  p_controlgrid = new wxBoxSizer(wxHORIZONTAL);
  p_fstcontrol = new wxBoxSizer(wxVERTICAL);
  p_sndcontrol = new wxBoxSizer(wxVERTICAL);

  /********************************/
  /* first part of top controlbar */
  /********************************/

  /* first row of top-control */
  wxBoxSizer *p_topctrl1 = new wxBoxSizer(wxHORIZONTAL);
  p_topctrl1->Add(loadDirButton, 0, wxEXPAND | wxALL, 2);
  p_topctrl1->Add(loadFileButton, 0, wxEXPAND | wxALL, 2);
  p_topctrl1->Add(refreshbutton, 0, wxEXPAND | wxALL, 2);
  p_topctrl1->Add(clearbutton, 0, wxEXPAND | wxALL, 2);

  /* second row of top-control */
  wxBoxSizer *p_botctrl1 = new wxBoxSizer(wxHORIZONTAL);
  p_botctrl1->Add(cachebutton, 0, wxEXPAND | wxALL, 2);
  p_botctrl1->Add(stripbutton, 0, wxEXPAND | wxALL, 2);
  p_botctrl1->Add(contbutton, 0, wxEXPAND | wxALL, 2);
  p_botctrl1->Add(sessionbutton, 0, wxEXPAND | wxALL, 2);

  p_fstcontrol->Add(p_topctrl1, 0, wxEXPAND);
  p_fstcontrol->Add(p_botctrl1, 0, wxEXPAND);

  /*********************************/
  /* second part of top controlbar */
  /*********************************/

  /* first row of top-control */
  wxBoxSizer *p_topctrl2 = new wxBoxSizer(wxHORIZONTAL);
  p_topctrl2->Add(exitbutton, 0, wxEXPAND | wxALL, 2);

  /* second row of top-control */
  wxBoxSizer *p_botctrl2 = new wxBoxSizer(wxHORIZONTAL);
  p_botctrl2->Add(helpbutton, 0, wxEXPAND | wxALL, 2);

  p_sndcontrol->Add(p_topctrl2, 0, wxEXPAND);
  p_sndcontrol->Add(p_botctrl2, 0, wxEXPAND);

  /********************************/
  /* top controlgrid final layout */
  /********************************/

  p_controlgrid->Add(p_fstcontrol, 0, wxEXPAND);
  p_controlgrid->Add(p_metapanel, 1, wxEXPAND);
  p_controlgrid->Add(p_sndcontrol, 0, wxALIGN_RIGHT | wxEXPAND);

  prgrsbarbox->Add(p_controlgrid, 0, wxEXPAND);
  p_controlbar->SetSizer(prgrsbarbox);

  /* MEDIA-CONTROL */
  mediaCtrl = new MediaWrap(this, MEDIA_CTRL);
  mediaCtrl->SetBackgroundColour(wxColour(_("#1D1B1B")));
  p_vmainwinbox->Add(mediaCtrl, 0, wxEXPAND | wxTOP, 5);

  /* MEDIA-CONTROL EVENTS */
  mediaCtrl->Bind(wxEVT_MEDIA_LOADED, &JRWVPMainWin::OnMediaLoaded, this,
      MEDIA_CTRL);
  /* BUTTON EVENTS */
  mediaCtrl->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &JRWVPMainWin::OnPlay, this,
       PLAY_BUTTON);
  mediaCtrl->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &JRWVPMainWin::OnStop, this,
       PAUSE_BUTTON);
  mediaCtrl->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &JRWVPMainWin::OnSkip, this,
       SKIP_BUTTON);

  /**********/
  /* EVENTS */
  /**********/

  /* BUTTONS-EVENTS */
  Bind(wxEVT_COMMAND_BUTTON_CLICKED, &JRWVPMainWin::OnExit,
      this, wxID_EXIT);
  Bind(wxEVT_COMMAND_BUTTON_CLICKED, &JRWVPMainWin::OnHelp,
      this, HELP_BUTTON);
  Bind(wxEVT_COMMAND_BUTTON_CLICKED, &JRWVPMainWin::OnContinue,
      this, CONT_BUTTON);
  Bind(wxEVT_COMMAND_BUTTON_CLICKED, &JRWVPMainWin::OnManageSession,
      this, SESSION_BUTTON);

  /* LISTITEMS-EVENTS */
  Bind(SONG_ACTIVATED, &JRWVPMainWin::OnDClickItem, this, LIST_ITEM);
  Bind(DECODE_ME_EVT, &JRWVPMainWin::OnDecodeMe, this, LIST_ITEM);
  Bind(LIST_ITEM_IN_FOCUS_EVT, &JRWVPMainWin::OnListitemFocus,
      this, LIST_ITEM);

  /* SESSIONMANAGER-EVENTS */
  Bind(UPDATE_PATH_EVT, &JRWVPMainWin::OnUpdatePaths, this,
    SESSION_MANAGER);

  /* TIMER-EVENTS */
  Bind(wxEVT_TIMER, &JRWVPMainWin::OnTimer, this, TIMER_ID);

  /* WINDOW-CONTROLS-EVENT */
  Bind(wxEVT_SIZE, &JRWVPMainWin::OnWinResize, this);
  Bind(wxEVT_IDLE, &JRWVPMainWin::OnIdle, this);

  /***********************/
  /* Data initialization */
  /***********************/

  m_timer->Start(100);
  manageSession();

  /* HOT-FIX */
  deq_songs.push_back(new JRListItem((wxFrame*)p_list_container, "",
          "", "", 0, 0, 0, 0, 0, 0, 0));
  it_cur_playing = deq_songs.begin();
}

/************************/
/* EVENT IMPLEMENTATION */
/************************/

void JRWVPMainWin::OnListitemFocus(wxCommandEvent &event)
{
  JRListItem *itm = (JRListItem*) event.GetEventObject();

  p_metapanel->setName(itm->getSongName());
  p_metapanel->setSampleRate(itm->getSamplingRate());
  p_metapanel->setChannels(itm->getChannels());
  p_metapanel->setLength(itm->getLengthInMS());
  p_metapanel->setChunkSize(itm->getChunkSize());
  p_metapanel->setCurrentResolution(itm->getResolution());
  p_metapanel->Refresh();
}

/*
 * Responsible for calling createList()
 */
void JRWVPMainWin::OnCopyFiles(wxCommandEvent& WXUNUSED(event))
{
  createList();
}

/*
 * Timerevent handler
 * Responsible for synchronizing audio-playback, visual cues
 * and updates progress in <time-passed>/<total-time>.
 */
void JRWVPMainWin::OnTimer(wxTimerEvent& WXUNUSED(event))
{
  if ( (deq_songs.size() > 0) && (it_cur_playing >= deq_songs.begin())
      && !b_loading_files)
  {
    JRListItem *cur_item = ((JRListItem*)(*it_cur_playing));

    cur_item->setProgress(mediaCtrl->m_mediaCtrl->Tell());
    wxLongLong llLength = mediaCtrl->m_mediaCtrl->Length();
    wxLongLong llTell = mediaCtrl->m_mediaCtrl->Tell();

    mediaCtrl->m_slider->SetRange(0, (int)(llLength / 1000).GetValue());
    mediaCtrl->setProgress(llTell.GetValue());

    if (mediaCtrl->IsBeingDragged() == false)
      mediaCtrl->m_slider->SetValue((long)(llTell / 1000).GetValue());
  }
}


/*
 * Let's user load a previously stored session
 */
void JRWVPMainWin::OnManageSession(wxCommandEvent& WXUNUSED(event))
{
  if (deq_songs.size() > 0)
    saveSession();

  manageSession();
}

/*
 * Dynamically (un-)binds the OnMediaFinished-Event
 * to allow this program to be used as a simple media-player
 * toggles a "b_continue" bool on a button press
 */
void JRWVPMainWin::OnContinue(wxCommandEvent& WXUNUSED(event))
{
  if (b_continue)
  {
    Unbind(wxEVT_MEDIA_FINISHED, &JRWVPMainWin::OnFinish, this, MEDIA_CTRL);
    contbutton->SetLabel("Single");
    b_continue = false;
  }
  else
  {
    Bind(wxEVT_MEDIA_FINISHED, &JRWVPMainWin::OnFinish, this, MEDIA_CTRL);
    contbutton->SetLabel("All");
    b_continue = true;
  }
}

/*
 * Mediactrl event
 * Responsible for loading the next available song and sanity
 * checking if it_cur_playing-Pointer is pointing to a valid
 * entry.
 *
 * TODO: Actually implement conditional branch, where user can
 * decide if the player should just continue to play everything
 */
void JRWVPMainWin::OnFinish(wxCommandEvent& WXUNUSED(event))
{
  JRListItem *cur_item = *it_cur_playing;
  cur_item->setPlaying(false);

  if (it_cur_playing != deq_songs.end())
    it_cur_playing++;
  if (it_cur_playing == deq_songs.end())
    it_cur_playing = deq_songs.begin();

  cur_item = *it_cur_playing;

  mediaCtrl->m_mediaCtrl->Load(cur_item->getOGFilePath());
}

/*
 * Clear cache button
 * Responsible for deleting all temporary associated files
 * in the cache directory
 */
void JRWVPMainWin::OnCache(wxCommandEvent& WXUNUSED(event))
{
  wxDir dir;
  wxFile file;
  wxArrayString name;
  wxString str;
  dir.GetAllFiles(s_cur_cache, &name);

  for (wxString s : name)
    if (std::remove(s) != 0)
    {
      str.Printf("Error deleting '%s'", s);
      wxMessageBox(str);
    }
}

/*
 * Help button
 * Spawns a new Frame containing usage-information, credits,
 * Recent changes.
 *
 * TODO: Make it pretty...
 */
void JRWVPMainWin::OnHelp(wxCommandEvent& WXUNUSED(event))
{
  HelpFrame *help_frame = new HelpFrame(this, "Help Window", wxPoint(50, 50),
      wxSize(700, 500));
  help_frame->Show();
}

/*
 * Clears the deq_songs removes all listitems and frees memory
 */
void JRWVPMainWin::OnClear(wxCommandEvent& WXUNUSED(event))
{
  clearList();
}

/*
 * Load 1 to n amount of files from a single folder
 * Spawns a dialog asking the user to select x-amount of
 * mp3/wav files to be parsed and b_loaded.
 */
void JRWVPMainWin::OnSF(wxCommandEvent& WXUNUSED(event))
{
  wxFileDialog* OpenDialog = new wxFileDialog(
      this, _("Choose one or more MP3/WAV-Files"), wxEmptyString,
      wxEmptyString, wxEmptyString, wxFD_MULTIPLE,
      wxDefaultPosition);

  wxArrayString names;
  wxString curDocPath;
  std::stringstream ss;

  if (OpenDialog->ShowModal() == wxID_OK)
  {
    OpenDialog->GetFilenames(names);
    curDocPath = OpenDialog->GetDirectory();
  }
  else
  {
    OpenDialog->Destroy();
    return;
  }

  size_t nItems = names.GetCount();

  if (!(names.GetCount() > 0))
  {
    OpenDialog->Destroy();
    return;
  }

  toggleVisibleList();
  wxProgressDialog dialog(_("Parser"), _("Decoding your audiofiles"), nItems, this);
  /* Need to create thread(s) to parse all files, keep GUI interactive */
  for (size_t i = 0; i < nItems; i++)
  {
    if (!dialog.Update(i))
      break;
#ifdef _WIN32
    ss << curDocPath << "\\" << names[i];
#elif __linux__
    ss << curDocPath << "/" << names[i];
#endif
    handleFile(ss.str());
    ss.str(std::string());
    wxTheApp->Yield();
  }

  createList();
  toggleVisibleList();
  saveSession();
}

/*
 * Load all possible files from user-selected folder and
 * recursivly traverse downwards.
 * Spawns a dialog asking the user to select path to a
 * desired folder containing mp3/wav files to be parsed
 * and b_loaded.
 */
void JRWVPMainWin::OnFD(wxCommandEvent& WXUNUSED(event))
{
  const wxString& sdir = wxDirSelector("Choose a folder containing MP3/WAV-Files");
  if (sdir.empty())
    return;
  wxDir dir;

  wxArrayString names;
  wxString filename;

  dir.GetAllFiles(sdir, &names);
  size_t nItems = names.GetCount();

  if (!(names.GetCount() > 0))
    return;

  toggleVisibleList();
  wxProgressDialog dialog(_("Parser"), _("Decoding your audiofiles"), nItems, this);
  wxString help_txt;
  for (size_t n = 0; n < nItems; n++)
  {
    help_txt.Printf("Working on: %s", names[n].mb_str());
    if (!dialog.Update(n))
      break;

    handleFile(std::string(names[n].mb_str()));
    wxTheApp->Yield();
  }

  createList();
  toggleVisibleList();
  saveSession();
}


/********/
/* MISC */
/********/

void JRWVPMainWin::manageSession()
{
  switch (active_mainwin)
  {
    case 0:
      p_maindisplay->Detach(0);
      p_sm->Hide();
      p_maindisplay->Prepend(p_list_container, 1, wxEXPAND | wxALL);
      p_list_container->Show();
      p_maindisplay->Layout();
      sessionbutton->SetLabel("Sessions");
      sessionbutton->SetToolTip("Manage your sessions");
      active_mainwin = 1;
      Bind(wxEVT_COMMAND_BUTTON_CLICKED, &JRWVPMainWin::OnFD,
          this, LOAD_DIR_BUTTON);
      Bind(wxEVT_COMMAND_BUTTON_CLICKED, &JRWVPMainWin::OnSF,
          this, LOAD_FILE_BUTTON);
      Bind(wxEVT_COMMAND_BUTTON_CLICKED, &JRWVPMainWin::OnClear,
          this, CLEAR_BUTTON);
      Bind(wxEVT_COMMAND_BUTTON_CLICKED, &JRWVPMainWin::OnCache,
          this, CACHE_BUTTON);
      Bind(wxEVT_COMMAND_BUTTON_CLICKED, &JRWVPMainWin::OnStrip,
          this, STRIP_BUTTON);
      break;
    case 1:
      p_maindisplay->Detach(0);
      p_list_container->Hide();
      p_maindisplay->Prepend(p_sm, 1, wxEXPAND | wxALL);
      p_sm->Show();
      p_maindisplay->Layout();
      sessionbutton->SetLabel("Waveview");
      sessionbutton->SetHelpText("View your audiofiles waveform");
      active_mainwin = 0;
      Unbind(wxEVT_COMMAND_BUTTON_CLICKED, &JRWVPMainWin::OnFD,
          this, LOAD_DIR_BUTTON);
      Unbind(wxEVT_COMMAND_BUTTON_CLICKED, &JRWVPMainWin::OnSF,
          this, LOAD_FILE_BUTTON);
      Unbind(wxEVT_COMMAND_BUTTON_CLICKED, &JRWVPMainWin::OnClear,
          this, CLEAR_BUTTON);
      Unbind(wxEVT_COMMAND_BUTTON_CLICKED, &JRWVPMainWin::OnCache,
          this, CACHE_BUTTON);
      Unbind(wxEVT_COMMAND_BUTTON_CLICKED, &JRWVPMainWin::OnStrip,
          this, STRIP_BUTTON);
      break;
  }
}

void JRWVPMainWin::clearList()
{
  if (mediaCtrl->m_mediaCtrl->GetState() == wxMEDIASTATE_PLAYING ||
      mediaCtrl->m_mediaCtrl->GetState() == wxMEDIASTATE_PAUSED)
  {
    mediaCtrl->m_mediaCtrl->Stop();
    mediaCtrl->m_slider->SetValue(0);
  }

  std::cout << "destroying items" << std::endl;
  for (JRListItem* itm : deq_songs)
    itm->Destroy();

  deq_songs.clear();
  b_loaded = false;
  p_hboxlist->Clear(true);
}

/*
 * Checks if a file really exists
 * @param string file - the path to a file we want to check
 * @return bool - true if exists, false if file doesnt exists
 */
bool JRWVPMainWin::exists(const std::string &file)
{
  /* Home & tmp */
  wxString home = wxGetUserHome();
  std::stringstream tmp;
  std::string filename = findFileName(file);

#ifdef __linux__
  tmp << home << "/.cache/wfv/" << filename << ".tmp";
#elif _WIN32
  tmp << stdp.GetDocumentsDir() << "\\wfv\\" << filename << ".tmp";
#endif

  if (deq_songs.size() < 1) return false;

  /* Check if file was already parsed */
  if (std::ifstream(tmp.str()))
  {
    std::cout << file << " is already parsed" << std::endl;
    /* Check if file is already existing as a list item */
    for (JRListItem* &item : deq_songs)
    {
      if (item->getOGFilePath() == file)
      {
        std::cout << "OGFP: " << item->getOGFilePath() << std::endl;
        std::cout << "file: " << file << std::endl;
        std::cout << "filename:" << filename << std::endl;
        return true;
      }
    }
  }
  return false;
}


void JRWVPMainWin::OnUpdatePaths(wxCommandEvent& event)
{
  b_loading_files = true;
  session_item *itm = (session_item*) event.GetClientData();

  s_cur_cache = itm->cachepath;
  s_cur_session = itm->filepath;

  /* Switch back to listview of audiofiles */
  active_mainwin = 0;
  manageSession();
  clearList();

  /* Start loading what might be already existing */
  toggleVisibleList();
  loadSession();
  toggleVisibleList();
  b_loading_files = false;
}

/*
 * Handles the desire for a listitem to get decoded
 * Disables rendering of the listitem, decodes audiofile,
 * refreshes state and displays updated listitem again.
 */
void JRWVPMainWin::OnDecodeMe(wxCommandEvent& event)
{
  /* Item that sent the event */
  JRListItem *p_item = wxDynamicCast(event.GetEventObject(), JRListItem);
  int resolution = event.GetInt();

  p_item->Hide();

  if (!std::ifstream(p_item->getOGFilePath()))
  {
    std::cout << "Error could not open file" << std::endl;
    return;
  }
  if (!decoder.load(p_item->getOGFilePath()))
  {
    std::cout << "Could not parse file... Wrong format?" << std::endl;
    return;
  }

  wxProgressDialog dialog(_("Decoder"),
//      wxString("Decoding your audiofile to %i resolution", resolution),
      _("Decoding your audiofile"),
      1, this);
  /* Need to create thread(s) to parse all files, keep GUI interactive */
  for (size_t i = 0; i < 1; i++)
  {
    if (!dialog.Update(i))
      break;
  decoder.downmix(resolution);
  if (!decoder.writeToFile(p_item->getRSFilePath()))
  {
    std::cout << "Trouble writing tmp data to file..." << std::endl;
    return;
  }
  std::cout << "Wrote to File" << std::endl;
    wxTheApp->Yield();
  }

  p_item->readIntoBuffer();
  p_item->refresh();
  p_item->Show();
}

/*
 * Mouseevent handler for double-clicks
 * Tries to start audioplayback connected to desired
 * listitem, that is being clicked on.
 *
 * Listens for custom SONG_ACTIVATED event.
 * event.GetInt() == -2 -> standard double-click, user
 *              wants this song to start playing
 * event.GetInt() != -2 -> user wants to seek
 *
 * TODO: Clean this mess up...
 */
void JRWVPMainWin::OnDClickItem(wxCommandEvent& event)
{
  JRListItem *cur_item = *it_cur_playing;
  JRListItem *req_item = (JRListItem*) event.GetEventObject();


  if ((cur_item->getOGFilePath() != req_item->getOGFilePath())
      || (mediaCtrl->m_mediaCtrl->GetState() == wxMEDIASTATE_STOPPED))
  {
    std::cout << "starting up... " <<
      req_item->getOGFilePath() << std::endl;
    cur_item->setPlaying(false);
    req_item->setPlaying(true);

    if (!mediaCtrl->m_mediaCtrl->Load(req_item->getOGFilePath()))
      wxMessageBox("Could not load media");

//    /* Allows to seek into a not already loaded song from the get go */
//    if (event.GetInt() != -2)
//    {
//      wxMicroSleep(100);
//      seekMedia(*((float*)event.GetClientData()));
//    }

    it_cur_playing = std::find(deq_songs.begin(), deq_songs.end(), req_item);

    std::cout << "setting currently playing: " << cur_item->getOGFilePath()
      << std::endl;
  }
  else if (event.GetInt() != -2)
    seekMedia(*((float*)event.GetClientData()));
}

void JRWVPMainWin::seekMedia(const float &offset)
{
  long llong = mediaCtrl->m_mediaCtrl->Length();
  if (!mediaCtrl->m_mediaCtrl->Seek(offset*llong))
    std::cout << "Could not seek media" << std::endl;
}

/*
 * Responsible to toggle visibility of all listitems
 * whenever there is a need to add or remove more items
 * Avoid rendering-bugs
 */
void JRWVPMainWin::toggleVisibleList()
{
  if(b_list_visible)
  {
    b_list_visible = false;
    p_list_container->HideWithEffect(wxSHOW_EFFECT_BLEND);
  }
  else
  {
    b_list_visible = true;
    p_list_container->ShowWithEffect(wxSHOW_EFFECT_BLEND);
    this->Layout();
  }
}

/*
 * Mediaevent-b_loaded handler
 * Only start playing and set the state of corresponding listitem to playing
 * when the mediaplayer finally b_loaded the file the user wants to be played
 */
void JRWVPMainWin::OnMediaLoaded(wxMediaEvent &WXUNUSED(event))
{
  JRListItem *cur_item = *it_cur_playing;

  cur_item->setLengthInMS(mediaCtrl->m_mediaCtrl->Length());
  cur_item->setPlaying(true);
  if (!mediaCtrl->m_mediaCtrl->Play())
    wxMessageBox("Could not start playing");
  cur_item->Refresh();
  cur_item->Update();
}

/*
 * Buttonevent handler for Play/Pause/Skipping
 */
void JRWVPMainWin::OnPlay(wxCommandEvent& WXUNUSED(event))
{
  if (deq_songs.size() == 0) // If deque-iterator points at invalid target
  {
    mediaCtrl->m_mediaCtrl->Pause();
    mediaCtrl->m_mediaCtrl->Stop();
    return;
  }
  if (mediaCtrl->m_mediaCtrl->GetState() == wxMEDIASTATE_PLAYING)
  {
    ((JRListItem*)(*it_cur_playing))->setPlaying(false);
    mediaCtrl->m_mediaCtrl->Pause();
    mediaCtrl->m_playButton->SetLabel(">");
  }
  else
  {
    ((JRListItem*)(*it_cur_playing))->setPlaying(true);
    mediaCtrl->m_mediaCtrl->Play();
    mediaCtrl->m_playButton->SetLabel("||");
  }
}

void JRWVPMainWin::OnPause(wxCommandEvent& WXUNUSED(event))
{
  if (mediaCtrl->m_mediaCtrl->GetState() == wxMEDIASTATE_PLAYING)
  {
    ((JRListItem*)(*it_cur_playing))->setPlaying(false);
    mediaCtrl->m_mediaCtrl->Pause();
  }
}

void JRWVPMainWin::OnStop(wxCommandEvent& WXUNUSED(event))
{
  if (mediaCtrl->m_mediaCtrl->GetState() != wxMEDIASTATE_STOPPED)
  {
    ((JRListItem*)(*it_cur_playing))->setPlaying(false);
    mediaCtrl->m_mediaCtrl->Stop();
    mediaCtrl->m_playButton->SetLabel(">");
  }
}

void JRWVPMainWin::OnSkip(wxCommandEvent& WXUNUSED(event))
{
  if (!b_loaded)
  {
    mediaCtrl->m_mediaCtrl->Stop();
    return;
  }
  if (((mediaCtrl->m_mediaCtrl->GetState() == wxMEDIASTATE_PLAYING) ||
        (mediaCtrl->m_mediaCtrl->GetState() == wxMEDIASTATE_PAUSED)))
    ((JRListItem*)(*it_cur_playing))->setPlaying(false);
  if (it_cur_playing != deq_songs.end()
      && it_cur_playing <= deq_songs.end())
    it_cur_playing++;
  if (it_cur_playing == deq_songs.end())
    it_cur_playing = deq_songs.begin();

  mediaCtrl->m_mediaCtrl->Load(((JRListItem*)(*it_cur_playing))
      ->getOGFilePath());
}

/*
 * Called whenever user exits the application. Saves state
 * and cleans up memory.
 */
void JRWVPMainWin::OnExit(wxCommandEvent& WXUNUSED(event))
{
  if (m_timer->IsRunning())
    m_timer->Stop();
  delete m_timer;
  std::cout << "checked timer" << std::endl;

  mediaCtrl->m_mediaCtrl->Stop();
  std::cout << "checked mediactrl" << std::endl;

  saveSession();
  std::cout << "Saved" << std::endl;
  Close();
}

/*
 * Sets a zoom for each listitem so that only relevant
 * non-zero parts of a soundwave are shown.
 * Strips near-zero values in the beginning and end of a
 * soundwave by zooming into approriate parts
 */
void JRWVPMainWin::OnStrip(wxCommandEvent &WXUNUSED(event))
{
  if (b_stripped)
  {
    stripbutton->SetLabel(wxT("Strip"));
    stripbutton->SetToolTip("Zooms into each audiofile stripping zeros");
    for (JRListItem *itm : deq_songs)
      itm->unstrip();
    b_stripped = false;
  }
  else
  {
    stripbutton->SetLabel(wxT("Un-Strip"));
    stripbutton->SetToolTip("Resets zoom of all audiofiles");
    for (JRListItem *itm : deq_songs)
      itm->strip();
    b_stripped = true;
  }
}

/*
 * Windowevent-resize handler
 */
void JRWVPMainWin::OnWinResize(wxSizeEvent& evt)
{
  b_resized = true;
  evt.Skip();
}

void JRWVPMainWin::OnIdle(wxIdleEvent &evt)
{
  if (b_resized)
    createList();
  b_resized = false;
}

/*
 * Responsible for generating n-amount of lists, containing soundwaves.
 * If appropriate width is given...
 *      ... 1 item will be displayed in a single list
 *      ... (1,4] items will be displayed in 2 lists
 *      ... (4,5] items will be displayed in 3 lists
 *      ... (5,n] items will be displayed in 4 lists
 * If something goes wrong will always use 2 lists
 */
void JRWVPMainWin::createList()
{
  if (deq_songs.size() < 1)
    return;

  /* Determine "optimal" number of lists and compare to current number of lists */
  int winSize = wxDisplay().GetGeometry().GetWidth();
  int curSize = GetClientSize().GetWidth();
  int nrOfLists;

  if ((curSize > 0 && curSize <= winSize / 4) || deq_songs.size() < 2)
  {
    nrOfLists = 1;
    std::cout << "1 List" << std::endl;
  }
  else if ((curSize > winSize / 4 && curSize <= winSize / 2)
      || deq_songs.size() < 5)
  {
    nrOfLists = 2;
    std::cout << "2 List" << std::endl;
  }
  else if ((curSize > winSize / 2 && curSize < winSize)
      || deq_songs.size() < 6)
  {
    nrOfLists = 3;
    std::cout << "3 List" << std::endl;
  }
  else if (curSize == winSize)
  {
    nrOfLists = 4;
    std::cout << "4 List" << std::endl;
  }
  else
  {
    nrOfLists = 2;
    std::cout << "Fallback Listnumber" << std::endl;
  }

  if (nrOfLists == cur_nr_of_lists)
    return;

  /* Clear current p_hboxlist from all items to enable redrawing */
  if (!(p_hboxlist->IsEmpty()))
  {
    for (wxScrolledWindow* &itm : vec_scrolwins)
      itm->Hide();

    /* Clear and delete already existing sizers */
    if (!vec_sizers.empty())
    {
      // detaches all windows from sizers inside vec_sizers
      for (wxSizer *sz : vec_sizers)
        sz->Clear();

      // deletes all instances of wxBoxSizers inside vec_scrolwins
      for (wxScrolledWindow *sz : vec_scrolwins)
        sz->SetSizer(NULL, true);

      // clear vectors
      vec_scrolwins.clear();
      vec_sizers.clear();
    }

    p_hboxlist->Clear();
    for (JRListItem *itm : deq_songs)
      itm->SetContainingSizer(NULL);
  }

  /* Create corresponding nr of listPanels inside p_hboxlist-sizer */
  int itemsPerList = deq_songs.size() / nrOfLists;

  itemsPerList = deq_songs.size() % nrOfLists == 0
    ? itemsPerList : itemsPerList + 1;

  vec_scrolwins.resize(nrOfLists);
  vec_sizers.resize(nrOfLists);
  for (int i = 0; i < nrOfLists; i++)
  {
    vec_sizers[i] = new wxBoxSizer(wxVERTICAL);
    vec_scrolwins[i] = new wxScrolledWindow(p_list_container, wxID_ANY);
    vec_scrolwins[i]->SetScrollRate(0, 25);
    std::cout << "created list scrollable listpanel[" << i << "]" << std::endl;
    p_hboxlist->Add(vec_scrolwins[i], wxEXPAND | wxLEFT | wxRIGHT, 5);
    vec_scrolwins[i]->SetSizer(vec_sizers[i]);

    for (int j = i * itemsPerList; (unsigned int)j < deq_songs.size(); j++)
    {
      if (j%itemsPerList == 0 && j != (i*itemsPerList))
        break;
      JRListItem *itm = deq_songs[j];

      /* Reparent each itm to the newly created ScrolledWindow */
      itm->Reparent(vec_scrolwins[i]);
      vec_sizers[i]->Add(itm, 0, wxEXPAND | wxLEFT | wxRIGHT, 4);
      vec_sizers[i]->AddSpacer(2);
      vec_sizers[i]->SetSizeHints(vec_scrolwins[i]);
    }
  }
  //Force Layout of all sizers
  p_vmainwinbox->Layout();
}

/*
 * Iterates over all listitems and stores required information for each file
 */
void JRWVPMainWin::saveSession()
{
  if (deq_songs.size() < 1)
    return;

  tinyxml2::XMLDocument* doc = new tinyxml2::XMLDocument();

  tinyxml2::XMLNode* root = doc->NewElement("WaveViewPlayer");
  doc->InsertFirstChild(root);
  tinyxml2::XMLNode* element = root->InsertEndChild(doc->NewElement("SongList"));
  std::vector<tinyxml2::XMLElement*> list;
  list.resize(deq_songs.size());
  std::cout << "Prepared document iterating over deq_songs" << std::endl;

  int length = 0;

  for (auto i = 0; (unsigned long)i < deq_songs.size(); i++)
  {
    /* Retrieve listitem to get information */
    std::string ss = deq_songs[i]->getOGFilePath();
    std::cout << ss << std::endl;
    /* Populate list of already parsed songs */
    list[length] = doc->NewElement("song");
    list[length]->SetAttribute("pathOG",
        deq_songs[i]->getOGFilePath().c_str());
    list[length]->SetAttribute("pathRS",
        deq_songs[i]->getRSFilePath().c_str());

    std::cout << "inserting list[" << length << "]: " <<
      deq_songs[i]->getOGFilePath().c_str() << std::endl;

    /* Give each song its attributes */
    tinyxml2::XMLElement* pSongSamplingRate = doc->NewElement("SamplingRate");
    tinyxml2::XMLElement* pSongSampleCount = doc->NewElement("SampleCount");
    tinyxml2::XMLElement* pSongName = doc->NewElement("SongName");
    tinyxml2::XMLElement* pSongChannelCount = doc->NewElement("ChannelCount");
    tinyxml2::XMLElement* pSongLength = doc->NewElement("SongLength");
    tinyxml2::XMLElement* pChunkSize = doc->NewElement("ChunkSize");
    tinyxml2::XMLElement* pHeightFactor = doc->NewElement("HeightFactor");
    tinyxml2::XMLElement* pResolution = doc->NewElement("Resolution");

    pSongSamplingRate->SetText(deq_songs[i]->getSamplingRate());
    pSongSampleCount->SetText(deq_songs[i]->getNumberOfSamples());
    pSongName->SetText(deq_songs[i]->getSongName().c_str());
    pSongChannelCount->SetText(deq_songs[i]->getChannels());
    pSongLength->SetText(deq_songs[i]->getLengthInMS());
    pChunkSize->SetText(deq_songs[i]->getChunkSize());
    pHeightFactor->SetText(deq_songs[i]->getHeightFactor());
    pResolution->SetText(deq_songs[i]->getResolution());

    list[length]->InsertEndChild(pSongSamplingRate);
    list[length]->InsertEndChild(pSongSampleCount);
    list[length]->InsertEndChild(pSongChannelCount);
    list[length]->InsertEndChild(pSongName);
    list[length]->InsertEndChild(pSongLength);
    list[length]->InsertEndChild(pChunkSize);
    list[length]->InsertEndChild(pHeightFactor);
    list[length]->InsertEndChild(pResolution);

    element->InsertEndChild(list[length]);
    length++;
  }
  std::cout << "Saving to " << s_cur_session << std::endl;
  doc->SaveFile(s_cur_session.c_str());
  delete doc;
  b_tracked = true;
}

/*
 * Loads all already available songs/clips from setup file into
 * a deque called deq_songs
 */
void JRWVPMainWin::loadSession()
{
  tinyxml2::XMLDocument doc;
  tinyxml2::XMLError err = doc.LoadFile(s_cur_session.c_str());

  //Check if file exists
  if (!std::ifstream(s_cur_session))
  {
    std::cout << "Setup file doesn't exist" << std::endl;
    return;
  }

  if (err != tinyxml2::XML_SUCCESS)
  {
    std::cout << "could not open xml file" << std::endl;
    doc.PrintError();
    return;
  }

  tinyxml2::XMLNode* pRoot = doc.FirstChild();

  if (pRoot == nullptr)
  {
    std::cout << "No Rootnode was able to be parsed, aborting..." << std::endl;
    return;
  }

  /* Find "SongList"-Node */
  tinyxml2::XMLElement* pEl = pRoot->FirstChildElement("SongList");

  if (pEl == nullptr)
  {
    std::cout << "Could not obtain child element" << std::endl;
    return;
  }

  tinyxml2::XMLElement* pList = pEl->FirstChildElement("song");
  size_t count = 0;
  for (pList = pEl->FirstChildElement("song"); pList; pList =
                                             pList->NextSiblingElement("song"))
    count++;

  pList = pEl->FirstChildElement("song");

  std::vector<std::vector<std::string>> vecList;

  wxProgressDialog dialog(_("Decoder"), _("Loading setup.xml"), count, this);
  int i = 0;

  while (pList != nullptr)
  {
    dialog.Update(i); // Update p_progressbar
    i++;
    std::string songName;
    int channelcount;
    int samplingrate;
    int samplecount;
    int length;
    int chunksize;
    float heightfactor;
    int resolution;
    pList->FirstChildElement("ChannelCount")->QueryIntText(&channelcount);
    pList->FirstChildElement("SamplingRate")->QueryIntText(&samplingrate);
    pList->FirstChildElement("SampleCount")->QueryIntText(&samplecount);
    pList->FirstChildElement("SongLength")->QueryIntText(&length);
    pList->FirstChildElement("ChunkSize")->QueryIntText(&chunksize);
    pList->FirstChildElement("HeightFactor")->QueryFloatText(&heightfactor);
    pList->FirstChildElement("Resolution")->QueryIntText(&resolution);
    songName = pList->FirstChildElement("SongName")->GetText();
    std::string pathOG = std::string(pList->Attribute("pathOG"));
    std::string pathRS = std::string(pList->Attribute("pathRS"));

    /* Create ListItems from database if song and resampled file exists*/
    std::cout << "Loading: " << pathOG << std::endl;
    std::cout << "Checking: " << pathRS << std::endl;
    if (std::ifstream(pathOG) && std::ifstream(pathRS))
    {
      JRListItem *item = new JRListItem((wxFrame*)p_list_container, pathOG,
          songName,
          pathRS,
          channelcount,
          samplingrate,
          length,
          samplecount,
          chunksize,
          heightfactor,
          resolution);
      item->readIntoBuffer();
      deq_songs.push_back(item);
    }

    pList = pList->NextSiblingElement("song");
  }

  toggleVisibleList();
  createList();
  toggleVisibleList();

  it_cur_playing = deq_songs.begin();
}

/*
 * Responsible for decoding/parsing/storing meta-data
 * of an audiofile in mp3/wav format.
 * @param string file - file that shall be decoded/parsed
 *                      and stored
 * @param int resolution - how many samples will be averaged to a single value
 *                         DEFAULT = 256
 * @returns bool - true if success, false if failure
 */
bool JRWVPMainWin::handleFile(const std::string &file, const int &resolution)
{
  if (!std::ifstream(file))
  {
    std::cout << "Error could not open file" << std::endl;
    return false;
  }
  if (exists(file))
  {
    std::cout << "File already parsed and inside list" << std::endl;
    return false;
  }
  if (!decoder.load(file))
  {
    std::cout << "Could not parse file... Wrong format?" << std::endl;
    return false;
  }

  wxString home = wxGetUserHome();
  std::stringstream tmp;
  std::string filename = findFileName(file);
  std::cout << "filename: " << filename << std::endl;
#ifdef __linux__
  tmp << s_cur_cache << filename << ".tmp";
#elif _WIN32
  tmp << s_cur_cache << filename << ".tmp";
#endif

  int res;
  /* Force higher resolution for audio with less than 8 seconds length */
  res = (decoder.getLength() <= 8000) ? 32 : resolution;
  if (!std::ifstream(tmp.str()))
  {
    decoder.downmix(res);

    if (!decoder.writeToFile(tmp.str()))
    {
      std::cout << "Trouble writing tmp data to file..." << std::endl;
      return false;
    }
    std::cout << "Wrote to File" << std::endl;
  }

  if (!deq_songs.empty())
  {
    /* Add to itemlist if not already existing otherwise update item */
    for (JRListItem* &itm : deq_songs)
    {
      if (itm->getOGFilePath() == file)
      {
        std::cout << "Updating listitem" << std::endl;
        itm->Show();
        return false;
      }
    }
  }
  std::cout << "Creating new list item" << std::endl;
  JRListItem *item = new JRListItem((wxFrame*)p_list_container, file,
      decoder.getFilename(),
      tmp.str(),
      decoder.getNrOfChannels(),
      decoder.getSampleRate(),
      decoder.getLength(),
      decoder.getNrOfSamples(),
      decoder.getChunkSize(),
      1,
      res);
  item->readIntoBuffer();
  deq_songs.push_back(item);
  if (!(it_cur_playing <= deq_songs.end()
        && it_cur_playing >= deq_songs.begin()) || !b_loaded)
  {
    it_cur_playing = deq_songs.begin();
    b_loaded = true;
  }
  return true;
}

/*
 * Strips the filename from a given path
 * @param string file - path to a file that should be stripped
 * @return string - filename without extension
 */
std::string JRWVPMainWin::findFileName(const std::string &file)
{
  std::string filename;
#ifdef __linux__
    filename = file.substr(file.find_last_of('/')+1);
    filename = filename.substr(0, filename.find_last_of('.'));
#elif _WIN32
    filename = file.substr(file.find_last_of('\\') + 1);
    filename = filename.substr(0, filename.find_last_of('.'));
#endif
  return filename;
}
