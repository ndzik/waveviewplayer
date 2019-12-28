#ifndef WVP_H
#define WVP_H
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif
#include <wx/sound.h>
#include <wx/stdpaths.h>
#include <errno.h>
#include <stdio.h>
#include <iostream>
#include "jrwvpmainwin.h"
#include "constants.h"

class JRWaveViewPlayer: public wxApp
{
public:
    virtual bool OnInit();
};
#endif
