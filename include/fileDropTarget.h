#ifndef FILEDROPTARGET_H
#define FILEDROPTARGET_H
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include <wx/wxprec.h>
#include <wx/panel.h>
#include <wx/dnd.h>
#include <wx/progdlg.h>
#include "constants.h"

class DDSoundFile : public wxFileDropTarget
{
public:
	DDSoundFile(class JRWVPMainWin *pParentFrame, wxPanel *pOwner = NULL);

	virtual bool OnDropFiles(wxCoord x, wxCoord y,
		const wxArrayString &filenames) override;

private:
	wxPanel *m_pOwner;
	class JRWVPMainWin *m_pParentFrame;

	friend class JRWVPMainWin;
};
#endif
