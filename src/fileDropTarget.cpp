#include "fileDropTarget.h"
#include "jrwvpmainwin.h"

DDSoundFile::DDSoundFile(JRWVPMainWin *pParentFrame, wxPanel *pOwner)
{
  m_pOwner = pOwner; m_pParentFrame = pParentFrame;
}

bool DDSoundFile::OnDropFiles(wxCoord, wxCoord, const wxArrayString &filenames)
{
  size_t nFiles = filenames.GetCount();

  if (m_pOwner != NULL && nFiles >= 1)
  {
    m_pParentFrame->toggleVisibleList();
    wxProgressDialog dialog(_("LOADER"), _("Parsing Files"), nFiles, m_pParentFrame);
    for (size_t n = 0; n < nFiles; n++)
    {
      if (!dialog.Update(n))
        break;
      m_pParentFrame->handleFile(std::string(filenames[n].mb_str()));
      wxTheApp->Yield();
    }
    m_pParentFrame->createList();
    m_pParentFrame->toggleVisibleList();
  }

  return true;
}
