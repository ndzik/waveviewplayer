#include "jrpopupmenu.h"

JRPopupMenu::JRPopupMenu() : wxMenu()
{

  /* MENU ITEMS */
  wxMenuItem *p_res_zoom = new wxMenuItem(this, RESET_ZOOM_MENU,
      "Reset zoom");
  wxMenuItem *p_res_height = new wxMenuItem(this, RESET_HEIGHT_MENU,
      "Reset height");
  wxMenuItem *p_chg_res = new wxMenuItem(this, CHANGE_RES_MENU,
      "Change resolution");

  this->Append(p_res_zoom);
  this->Append(p_res_height);
  this->Append(p_chg_res);
}
