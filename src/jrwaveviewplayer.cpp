#include "jrwaveviewplayer.h"


wxIMPLEMENT_APP(JRWaveViewPlayer);

bool JRWaveViewPlayer::OnInit()
{
  if (!wxApp::OnInit())
    return false;
  SetAppName("JR's WaveViewPlayer");

  /* Check if on windows corresponding folders exist */
  std::stringstream documents;

#ifdef _WIN32
  wxStandardPathsBase &stdp = wxStandardPaths::Get();
  wxString docs = stdp.GetDocumentsDir();
  documents << docs << "\\wfv\\sessions";
  wxString location = (documents.str()).c_str();
  if (!wxDirExists(location))
    wxMkdir(location);
#elif __linux__
  std::stringstream config;
  std::stringstream cache;
  wxString home = wxGetUserHome();
  config << home << "/.config/wfv";
  cache << home << "/.cache/wfv";
  if (!wxDirExists(cache.str()))
    wxMkdir(cache.str());
  if (!wxDirExists(config.str()))
    wxMkdir(config.str());
#endif

  JRWVPMainWin *frame = new JRWVPMainWin("JRWaveViewPlayer", wxPoint(50, 50),
      wxSize(600, 500));
  frame->Show(true);
  return true;
}
