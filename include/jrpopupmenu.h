#ifndef POPUPMENU_H
#define POPUPMENU_H
#include <wx/wx.h>
#include <wx/wxprec.h>
#include <wx/menu.h>
#include <wx/menuitem.h>
#include "constants.h"

class JRPopupMenu : public wxMenu
{
public:
  JRPopupMenu();

  wxMenu *p_menu;
};
#endif
