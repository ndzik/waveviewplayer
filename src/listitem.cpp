#include "listitem.h"

/*
 * JRListItem is responsible to hold all necessary information
 * to be displayed as a "soundwave/waveform" and adjust the display
 * and Resolution
 */

/* Constructor */
JRListItem::JRListItem(wxFrame* parent, const std::string &s_path_to_orig_file,
    const std::string &s_audio_name, const std::string &s_path_to_dsampled_file,
    const uint32_t &channel_count, const uint32_t &sampling_rate,
    const uint32_t &length_in_ms, const uint32_t &nr_of_samples,
    const uint32_t &chunk_size,const float &height_factor,
    const uint32_t &cur_resolution)
  : wxWindow(parent, wxID_ANY, wxDefaultPosition, wxSize(160,80),
      wxFULL_REPAINT_ON_RESIZE | wxWANTS_CHARS )
{
  /* wxwidget stuff */
  SetBackgroundStyle(wxBG_STYLE_PAINT);
  wxColour background;
  background.Set(wxT("#2b2b2b"));
  this->SetBackgroundColour(background);
  this->SetId(LIST_ITEM);
  this->s_path_to_orig_file = s_path_to_orig_file;
  this->s_audio_name = s_audio_name;
  this->s_path_to_dsampled_file = s_path_to_dsampled_file;
  this->channels = channel_count;
  this->sampling_rate = sampling_rate;
  this->length_in_ms = length_in_ms;
  this->nr_of_samples = nr_of_samples;
  this->chunk_size = chunk_size;
  this->card_height = 80;
  this->HEIGHTFACTOR = height_factor;
  this->CUR_RESOLUTION = cur_resolution;

  rst_zoom = new CustomButton(this, REZOOM_BUTTON, wxT("R"));
  rst_height = new CustomButton(this, RSTH_BUTTON, wxT("o"));
  inc_height = new CustomButton(this, INCH_BUTTON, wxT("+"));
  dec_height = new CustomButton(this, DECH_BUTTON, wxT("-"));

  /* Design card-ui */
  wxBoxSizer *h_box = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *v1_box = new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer *v2_box = new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer *v3_box = new wxBoxSizer(wxHORIZONTAL);

  h_box->Add(v1_box, 1, wxEXPAND | wxTOP | wxRIGHT, 1);
  h_box->Add(v2_box, 1, wxEXPAND | wxTOP | wxRIGHT, 1);
  h_box->Add(v3_box, 1, wxEXPAND | wxTOP | wxRIGHT | wxLEFT | wxBOTTOM, 1);
  v1_box->Add(10, 10, 1, wxEXPAND);
  v1_box->Add(inc_height, 0, wxEXPAND | wxALIGN_RIGHT | wxTOP, 1);
  v1_box->AddSpacer(1);
  v2_box->Add(10, 10, 1, wxEXPAND);
  v2_box->Add(rst_height, 0, wxEXPAND | wxALIGN_RIGHT);
  v2_box->AddSpacer(1);
  v3_box->Add(rst_zoom, 0, wxSHAPED | wxALIGN_BOTTOM | wxLEFT | wxBOTTOM, 1);
  v3_box->Add(10, 10, 1, wxEXPAND);
  v3_box->Add(dec_height, 0, wxEXPAND | wxALIGN_RIGHT| wxBOTTOM, 1);
  v3_box->AddSpacer(1);
  this->SetSizer(h_box);

  /**********/
  /* EVENTS */
  /**********/

  /* BIND GENERAL EVENTS */
  Bind(wxEVT_PAINT, &JRListItem::OnPaint, this, LIST_ITEM);
  Bind(wxEVT_LEFT_DCLICK, &JRListItem::OnClick, this, LIST_ITEM);
  Bind(wxEVT_LEFT_DOWN, &JRListItem::OnLeftDown, this, LIST_ITEM);
  Bind(wxEVT_LEFT_UP, &JRListItem::OnLeftUp, this, LIST_ITEM);
  Bind(wxEVT_SET_FOCUS, &JRListItem::OnSetFocus, this, LIST_ITEM);
  Bind(wxEVT_KILL_FOCUS, &JRListItem::OnKillFocus, this, LIST_ITEM);
  Bind(wxEVT_ENTER_WINDOW, &JRListItem::OnMouseEnter, this, LIST_ITEM);
  Bind(wxEVT_LEAVE_WINDOW, &JRListItem::OnMouseLeave, this, LIST_ITEM);
  Bind(wxEVT_SIZE, &JRListItem::OnResize, this, LIST_ITEM);
  Bind(wxEVT_CHILD_FOCUS, &JRListItem::OnChildFocus, this, wxID_ANY);
  Bind(wxEVT_MOUSEWHEEL, &JRListItem::OnHeight, this, LIST_ITEM);
  Bind(wxEVT_CHAR_HOOK, &JRListItem::OnKeyDown, this, LIST_ITEM);
  Bind(wxEVT_TIMER, &JRListItem::OnTimer, this, DELAY_DRAG_TIMER_ID);
  Bind(wxEVT_TIMER, &JRListItem::OnTxtTimer, this, TXT_TIMER_ID);
  Bind(wxEVT_CONTEXT_MENU, &JRListItem::OnContextMenu, this, LIST_ITEM);
  Bind(wxEVT_COMMAND_MENU_SELECTED, &JRListItem::OnPopupClick, this, wxID_ANY);

  /* BIND CUSTOM-BUTTON EVENTS */
  rst_zoom->Bind(wxEVT_LEFT_DOWN, &JRListItem::OnRezoom,
      this, REZOOM_BUTTON);
  rst_zoom->Bind(wxEVT_ENTER_WINDOW, &JRListItem::OnMouseEnter,
      this, REZOOM_BUTTON);
  rst_height->Bind(wxEVT_LEFT_DOWN, &JRListItem::OnRstHeight,
      this, RSTH_BUTTON);
  rst_height->Bind(wxEVT_ENTER_WINDOW, &JRListItem::OnMouseEnter,
      this, RSTH_BUTTON);
  inc_height->Bind(wxEVT_LEFT_DOWN, &JRListItem::OnIncHeight,
      this, INCH_BUTTON);
  inc_height->Bind(wxEVT_ENTER_WINDOW, &JRListItem::OnMouseEnter,
      this, INCH_BUTTON);
  dec_height->Bind(wxEVT_LEFT_DOWN, &JRListItem::OnDecHeight,
      this, DECH_BUTTON);
  dec_height->Bind(wxEVT_ENTER_WINDOW, &JRListItem::OnMouseEnter,
      this, DECH_BUTTON);


  /* Variable initialization and start of p_timer */
  arr_cur_zoom[0] = 0;
  arr_cur_zoom[1] = 1;
  p_timer = new wxTimer(this, DELAY_DRAG_TIMER_ID);
  p_txt_offset_timer = new wxTimer(this, TXT_TIMER_ID);
  visibleButtons(false);
}
  /* Destructor */
JRListItem::~JRListItem(){}

void JRListItem::visibleButtons(const bool &val)
{
  if (val)
  {
    rst_zoom->Show();
    rst_height->Show();
    inc_height->Show();
    dec_height->Show();
    this->Layout();
  }
  else
  {
    rst_zoom->Hide();
    rst_height->Hide();
    inc_height->Hide();
    dec_height->Hide();
  }
}

/*
 * Timerevent handler
 * Responsible for starting the drag&drop operation if
 * user held mousebutton for long enough
 */
void JRListItem::OnTimer(wxTimerEvent &WXUNUSED(evt))
{
  wxFileDataObject my_data;
  my_data.AddFile(s_path_to_orig_file);
  wxDropSource dragSource(this);
  dragSource.SetData(my_data);
  wxDragResult result = dragSource.DoDragDrop(wxDrag_CopyOnly);
}

/*
 * Timerevent handler
 * Responsible for updating name position of currently active item
 */
void JRListItem::OnTxtTimer(wxTimerEvent &WXUNUSED(evt))
{
  float max_off = std::fabs(GetClientSize().GetWidth()-102-text_ext.GetWidth());

  TEXT_OFFSET = TEXT_OFFSET >= max_off ? 0 : TEXT_OFFSET+1;
  refresh();
}

/******************/
/* Misc-Functions */
/******************/

/*
 * Draws a filled waveform by simply connecting each value with its couterpart
 * with a line.
 * Seems unnecessary, drawing points should be enough.
 * @param vector<float> a : Vector containing normalized [0,1] that should
 *                          be drawn onto the item
 * @param int tmpwidth : The available canvas-width that can be drawn to
 * @param wxBrush brsh : A wxBrush for intended color
 * @param wxDC *dc : A pointer to the device context that manages graphics
 */
void JRListItem::drawWaveFilledLine(const std::vector<float> &a, int tmpwidth,
    int tmpheight, wxDC *dc)
{
  for(auto k=0; k<tmpwidth; k++)
    dc->DrawLine(k,  a[k], k,   -a[k]+tmpheight);
}

/*
 * Draws a wave skeleton by simply connecting adjacent values with a line.
 * Seems unnecessary, drawing points should be enough.
 * @param vector<float> a : Vector containing normalized [0,1] that should
 *                          be drawn onto the item
 * @param int tmpwidth : The available canvas-width that can be drawn to
 * @param wxBrush brsh : A wxBrush for intended color
 * @param wxDC *dc : A pointer to the device context that manages graphics
 */
void JRListItem::drawWaveSkeleton(const std::vector<float> &a, int tmpwidth,
    int tmpheight, wxDC *dc)
{
  for(auto k=0; k<tmpwidth; k++)
  {
    if(k==0)
    {
      dc->DrawLine(k, a[k], k+1, a[k+1]);
      dc->DrawLine(k, -a[k]+tmpheight, k+1, -a[k+1]+tmpheight);
    }
    else
    {
      dc->DrawLine(k-1, a[k-1], k, a[k]);
      dc->DrawLine(k-1, -a[k-1]+tmpheight, k, -a[k]+tmpheight);
    }
  }
}

/*
 * Draws a filled waveform.
 * @param vector<float> a : Vector containing normalized [0,1] that should
 *                          be drawn onto the item
 * @param int tmpwidth : The available canvas-width that can be drawn to
 * @param wxBrush brsh : A wxBrush for intended color
 * @param wxDC *dc : A pointer to the device context that manages graphics
 */
void JRListItem::drawWaveFilled(const std::vector<float> &a, int tmpwidth,
    wxBrush brsh, wxDC *dc)
{
  dc->SetBrush(brsh);
  dc->SetPen(wxPen(wxColor(200,20,20), 1));
  wxPointList plT;
  wxPointList plB;
  float coord_zero = card_height*0.5;
  plT.push_back(new wxPoint(0,coord_zero));
  plB.push_back(new wxPoint(0,coord_zero));
  for(auto k=0; k<tmpwidth; k++)
  {
    // Set point to '0' if it overshoots
    float tmp_res = a[k];
    float point_y = (tmp_res < coord_zero+NOISEGATE) ? coord_zero : tmp_res;
    // Populate wxPointList
    plT.push_back(new wxPoint(k, point_y));
    plB.push_back(new wxPoint(k, -point_y+card_height));
  }
  plT.push_back(new wxPoint(tmpwidth,coord_zero));
  plB.push_back(new wxPoint(tmpwidth,coord_zero));
  dc->DrawPolygon(&plT);
  dc->DrawPolygon(&plB);

  plT.DeleteContents(true);
  plB.DeleteContents(true);
  plT.Clear();
  plB.Clear();
}

/*
 * Keyevent Handler, responsible for implementing basic
 * keyboard shortcuts
 */
void JRListItem::OnKeyDown(wxKeyEvent &evt)
{
  /* R without CTRL as modifier */
  if(b_zoomed && evt.GetModifiers() != wxMOD_CONTROL && evt.GetKeyCode() == 82)
  {
    this->b_zoomed = false;
    arr_cur_zoom[0] = 0;
    arr_cur_zoom[1] = 1;
    b_bitmap_cached = false;
    b_dirty = true;
    refresh();
  }
  /* R with CTRL as modifier */
  else if( evt.GetModifiers() == wxMOD_CONTROL && evt.GetKeyCode() == 82 )
  {
    HEIGHTFACTOR=1;
    b_bitmap_cached = false;
    b_dirty = true;
    refresh();
  }
  else if ( evt.GetModifiers() == (wxMOD_CONTROL | wxMOD_SHIFT)
      && evt.GetKeyCode() == 88 )
  {
    changeRes();
  }
  return;
}

/*
 * Mousewheel event handler, responsible for manipulating
 * the H-Factor of a listitem
 */
void JRListItem::OnHeight(wxMouseEvent& evt)
{
  /*
   * negative delta -> lower tmpheight
   * positive delta -> bigger tmpheight
   */
  if( evt.GetModifiers() == wxMOD_SHIFT )
  {
    if(evt.GetWheelRotation() > 0 && evt.GetWheelDelta() > 0)
    {
      HEIGHTFACTOR += 0.02;
      b_bitmap_cached = false;
      b_dirty = true;
    }
    else if(evt.GetWheelRotation() < 0 && evt.GetWheelDelta() > 0)
    {
      HEIGHTFACTOR -= 0.02;
      b_bitmap_cached = false;
      b_dirty = true;
    }
  }
  else
  {
    evt.Skip();
  }
  Refresh();
}

/*
 * Mouseclick event handler (!double-click).
 * Responsible for distinguishing a simple intended click
 * by the user, or a dedicated dragging (as in drag&drop
 * from/to this/other programs)
 */
void JRListItem::OnLeftDown(wxMouseEvent& evt)
{
  if(evt.GetModifiers() == wxMOD_SHIFT)
  {
    b_zooming = true;
    wxSize size = GetClientSize();
    wxPoint pt = wxGetMousePosition();
    int mouseX = pt.x - this->GetScreenPosition().x;
    arr_tmp_zoom[0] = static_cast<float>(mouseX)
                      / static_cast<float>(size.GetWidth()-TOOLBAR_SPACING);
  }
  else // commence drag and drop
  {
    p_timer->Start(250, wxTIMER_ONE_SHOT);
  }
  evt.Skip();
  Refresh();
}

void JRListItem::OnPopupClick(wxCommandEvent &evt)
{
  switch(evt.GetId())
  {
    case RESET_ZOOM_MENU:
      resetZoom();
      break;
    case RESET_HEIGHT_MENU:
      resetHeight();
      break;
    case CHANGE_RES_MENU:
      changeRes();
      break;
    default:
      break;
  }
  Refresh();
}

void JRListItem::OnContextMenu(wxContextMenuEvent& WXUNUSED(evt))
{
  JRPopupMenu popupMenu;
  PopupMenu(&popupMenu);
}

void JRListItem::OnLeftUp(wxMouseEvent &evt)
{
  p_timer->Stop();
  if(!b_zooming)
    return;
  wxSize size = GetClientSize();
  wxPoint pt = wxGetMousePosition();
  int mouseX = pt.x - this->GetScreenPosition().x;
  arr_tmp_zoom[1] = static_cast<float>(mouseX)
                    / static_cast<float>(size.GetWidth()-TOOLBAR_SPACING);
  b_zooming=false;
  arr_cur_zoom[0] = arr_tmp_zoom[0]>arr_tmp_zoom[1] ? arr_tmp_zoom[1] : arr_tmp_zoom[0];
  arr_cur_zoom[1] = arr_tmp_zoom[0]>arr_tmp_zoom[1] ? arr_tmp_zoom[0] : arr_tmp_zoom[1];
  b_zoomed = true;
  b_dirty = true;
  evt.Skip();
  Refresh();
}

/*
 * Responsible for resetting the state of this listitem
 * whenever a user wants to undo a zoom
 */
void JRListItem::OnRezoom(wxMouseEvent& evt)
{
  resetZoom();
  evt.Skip();
  refresh();
}

void JRListItem::OnSetFocus(wxFocusEvent& WXUNUSED(evt))
{
  b_focused = true;
  Refresh();
}

void JRListItem::OnKillFocus(wxFocusEvent& WXUNUSED(evt))
{
  b_focused = false;
  Refresh();
}

/*
 * Userdefined buttons to manipulate H-Factor
 */
void JRListItem::OnIncHeight(wxMouseEvent& evt)
{
  HEIGHTFACTOR += 0.02;
  refresh();
  evt.Skip();
}
void JRListItem::OnDecHeight(wxMouseEvent& evt)
{
  HEIGHTFACTOR -= 0.02;
  refresh();
  evt.Skip();
}
void JRListItem::OnRstHeight(wxMouseEvent& evt)
{
  resetHeight();
  evt.Skip();
}

/*
 * Responsible to regain focus, after user enters a custom
 * button on a listitem with his mouse.
 */
void JRListItem::OnChildFocus(wxChildFocusEvent& WXUNUSED(evt))
{
  this->SetFocus();
}

void JRListItem::OnResize(wxSizeEvent& evt)
{
  refresh();
  evt.Skip();
}

void JRListItem::refresh()
{
  b_bitmap_cached = false; /* triggers recaching of cached_bitmap */
  b_dirty = true;
  Refresh();
}

/*
 * Responsible for triggering another turn of rendering to
 * refresh the cached_bitmap, that is cached in the variable 'cached_bitmap'
 */
void JRListItem::OnMouseLeave(wxMouseEvent& evt)
{
  // force recaching of list when mouse leaves to remove border
  wxPoint position = GetScreenPosition();
  wxPoint mouse_pos = evt.GetPosition();

  wxPoint top_left(0, 0);
  wxPoint top_right(GetClientSize().GetWidth(), 0);
  wxPoint bottom_left(0, GetClientSize().GetHeight());
  if (!(mouse_pos.x > top_left.x && mouse_pos.x < top_right.x
    && mouse_pos.y < bottom_left.y && mouse_pos.y > top_left.y))
  {
    visibleButtons(false);
    b_focused = false;
  }
  TEXT_OFFSET = 0;
  txt_progress = 0;
  if (p_txt_offset_timer->IsRunning())
    p_txt_offset_timer->Stop();
  refresh();
}

void JRListItem::OnMouseEnter(wxMouseEvent& evt)
{
  // SetFocus leads to the button getting focused and catching all key events
  this->SetFocus();
  b_focused = true;
  wxCommandEvent event(LIST_ITEM_IN_FOCUS_EVT, GetId());
  event.SetEventObject(this);
  ProcessWindowEvent(event);
  evt.Skip();
  /* Start p_txt_offset_timer if necessary */
  if (text_ext.GetWidth() > GetClientSize().GetWidth()-102)
    p_txt_offset_timer->Start(200);

  visibleButtons(true);
  Refresh();
}

void JRListItem::OnMouseMotion(wxMouseEvent& WXUNUSED(evt))
{
  this->SetFocus();
}

/*
 * If user double-clicks a listitem, this event gets triggered.
 * Responsible for either starting audioplayback and seeking
 *
 * Propagates custom SONG_ACTIVED event for JRWVPMainWin:
 *
 *  event->SetInt(0) if CTRL-modifier was pressed while this event
 *           occured -> User wants to seek somewhere into audio
 *
 *  event->SetInt(-2) else indicating it was just a standard dclick,
 *  to start playback of the audiofile
 *
 *  TODO: Fix 'fishy' looking seek-logic
 */
void JRListItem::OnClick(wxMouseEvent& evt)
{
  evt.GetEventObject();
  wxSize size = GetClientSize();
  wxPoint pt = wxGetMousePosition();
  int mouseX = pt.x - this->GetScreenPosition().x;
  wxCommandEvent *event = new wxCommandEvent(SONG_ACTIVATED, GetId());
  event->SetEventObject(this);
  if( evt.GetModifiers() == wxMOD_CONTROL )
  {
    float zoomLvl = (1.f/(arr_cur_zoom[1]-arr_cur_zoom[0]));
    float focusedWidth = static_cast<float>(size.GetWidth()-TOOLBAR_SPACING)*zoomLvl;
	//~approx. correct but still a bit fishy
    f_file_offset = (static_cast<float>(mouseX)/focusedWidth)+arr_cur_zoom[0];
    event->SetInt(0);
    event->SetClientData(&f_file_offset);
  }
  else
    event->SetInt(-2);
  QueueEvent(event);
  evt.Skip();
  Refresh();
}

/***************************/
/* Statechanging functions */
/***************************/

/*
 * Strips near-zero values in the beginning and end of the soundwave
 * by zooming into relevant parts
 */
void JRListItem::strip()
{
  /* iterate over vec_samples from beginning */
  for (unsigned int i=0; i<vec_samples.size(); ++i)
    if (!(std::fabs(vec_samples[i]) <= 0.005))
      arr_cur_zoom[1] = static_cast<float>(i)
                        / static_cast<float>(vec_samples.size());

  /* iterate over vec_samples from end */
  for (unsigned int i=vec_samples.size()-1; i>0; --i)
      if (!(std::fabs(vec_samples[i]) <= 0.005))
      arr_cur_zoom[0] = static_cast<float>(i)
                        / static_cast<float>(vec_samples.size());

  if (!(arr_cur_zoom[0] == 0 && arr_cur_zoom[1] == 1))
    b_zoomed = true;

  /* adapt resolution if its to low to be displayed */
  wxSize sz = GetClientSize();
  unsigned int samples = vec_samples.size();
  // -20 to account for rounding errors
  unsigned int draw_width = sz.GetWidth()-TOOLBAR_SPACING-20;
  unsigned int sample_amount = samples*arr_cur_zoom[1]
                               - samples*arr_cur_zoom[0];

  if (sample_amount < draw_width)
    changeRes(false);

  b_bitmap_cached = false;
  b_dirty = true;
  Refresh();
}

void JRListItem::unstrip()
{
  arr_cur_zoom[0] = 0;
  arr_cur_zoom[1] = 1;
  b_zoomed = false;
  b_bitmap_cached = false;
  b_dirty = true;
  Refresh();
}

void JRListItem::resetZoom()
{
  this->b_zoomed = false;
  b_dirty = true;
  arr_tmp_zoom[0]=0;
  arr_tmp_zoom[1]=1;
  arr_cur_zoom[0]=0;
  arr_cur_zoom[1]=1;
  Refresh();
}

void JRListItem::resetHeight()
{
  HEIGHTFACTOR = 1;
  b_bitmap_cached = false;
  b_dirty = true;
  Refresh();
}

/*
 * Allows user to change the sample rate resolution
 * Pops up a seperate window with an explanation
 * what exactly adjusting of resolution means for
 * performance and memory impact.
 *
 * Fires a DECODE_ME_EVT when an option was chosen,
 * else nothing.
 *
 * @param bool input - True if user should be asked
 *                     for a desired value, else
 *                     will use default resolution (1)
 */
void JRListItem::changeRes(const bool &input)
{
  std::cout << "changing resolution" << std::endl;

  int choice;

  if(input)
  {
    wxArrayString arr_choices;
    arr_choices.Add("512");
    arr_choices.Add("256");
    arr_choices.Add("128");
    arr_choices.Add("64");
    arr_choices.Add("32");
    arr_choices.Add("8");
    arr_choices.Add("1");

    std::stringstream ss_desc;
    ss_desc << "Increasing the resolution of a waveform leads to more points\n"
      "which have to be available in memory, thus increasing the amount of RAM\n"
      "used. A ~4 MB big mp3 with a samplingrate of 48 kHz leads to ~100 MB\n"
      "of sampledata that have to be hold in memory.\n"
      "\n"
      "It will also lead to a more fine grained display of your waveform,\n"
      "especially if you consider zooming in and getting rid of 0 values in the\n"
      "beginning and end of your audiofile.\n";
    choice = wxGetSingleChoiceIndex(ss_desc.str(), "Change Resolution",
        arr_choices, 1, this);

    if (choice == -1)
      return;
  }
  else
  {
    choice = 6;
  }

  /* Send DECODE_ME_EVT to mainframe */
  wxCommandEvent event(DECODE_ME_EVT, GetId());

  switch(choice)
  {
    case 0:
      CUR_RESOLUTION = 512;
      break;
    case 1:
      CUR_RESOLUTION = 256;
      break;
    case 2:
      CUR_RESOLUTION = 128;
      break;
    case 3:
      CUR_RESOLUTION = 64;
      break;
    case 4:
      CUR_RESOLUTION = 32;
      break;
    case 5:
      CUR_RESOLUTION = 8;
      break;
    case 6:
      CUR_RESOLUTION = 1;
      break;
    default:
      return;
  }

  event.SetInt(CUR_RESOLUTION);
  event.SetEventObject(this);
  ProcessWindowEvent(event);
}

/* wxRendering */
void JRListItem::OnPaint(wxPaintEvent &WXUNUSED(evt))
{
  if (!b_focused && b_bitmap_cached && !b_playing)
  {
    wxPaintDC paint_dc(this);
    paint_dc.DrawBitmap(cached_bitmap, 0, 0);
  }
  else
  {
    wxBufferedPaintDC dc(this);
    render(dc);
  }
}

/*
 * Reads normalized PCM-values from a file into the
 * vec_samples-Buffer, where they will be cached
 * as long as a listitem is alive.
 */
void JRListItem::readIntoBuffer()
{
  /* If vec_samples is already filled clear it */
  if (vec_samples.size() > 1)
	vec_samples.resize(0);
  /* Read from file */
  float xs=0;

  //Check if s_path_to_dsampled_file even exists
  std::ifstream in(s_path_to_dsampled_file);
  if(!in.is_open())
  {
    this->Hide();
  }

  while(in >> xs)
      vec_samples.push_back(xs);
}

/*
 * Render-loop
 * Redraws all, or only progressbar layer of a listitem. Depending
 * on if it is currently playing or not
 */
void JRListItem::render(wxDC &dc)
{
  dc.Clear();
  wxBrush brsh(wxColour(wxT("#FF0000")));
  wxFont font = GetFont();
  /* Get size of panel */
  wxSize size = GetClientSize();
  int width = size.GetWidth() - TOOLBAR_SPACING;

  /* Calculate necessary avgs of buffercontents for vec_RMS and plain vec_AVG */
  if(b_dirty)
    calcAvgOfBuffersamples(size);

  /* Draw waveform filled */
  drawWaveFilled(vec_AVG, width, brsh, &dc);
  brsh.SetColour(wxT("#F57070"));
  drawWaveFilled(vec_RMS, width, brsh, &dc);

  /* Draw media progress */
  if(b_playing)
    drawProgress(length_in_ms, size, &dc);

  /* Draw zoom hinting */
  drawZoomHint(size, &dc);
  drawInfoText(size, font, &dc);

  /* Add name of file and zoom-hint*/
  drawName(font, &dc);
  drawZoomedText(size, &dc);

  /* Heightfactor */
  drawHeightFactor(font, width, &dc);

  /* Draw Border */
  drawActiveBorder(size, &dc);
  drawBorder(size, &dc);

  /* Store current image as cached_bitmap */
  cached_bitmap = dc.GetAsBitmap();
  b_bitmap_cached = true;
}

/**********************************/
/* HELPER FUNCTIONS FOR RENDERING */
/**********************************/

/*
 * Draws a 'zoomed' hint in the bottom left corner of an audio-
 * file when the user magnified a part of the waveform
 */
void JRListItem::drawZoomHint(const wxSize &size, wxDC *dc)
{
  wxPoint mousePos = wxGetMousePosition();
  int width = size.GetWidth()-TOOLBAR_SPACING;
  int mouseX = mousePos.x - this->GetScreenPosition().x;
  /* Draw hinting for start-end of zoom selection */
  dc->SetPen(wxPen(wxColor(24, 21, 200)));
  if(b_zooming)
  {
    dc->DrawLine(arr_tmp_zoom[0]*width, 0, arr_tmp_zoom[0]*width, size.GetHeight());
    dc->DrawLine(mouseX, 0, mouseX, size.GetHeight());
    Refresh();
  }
}

void JRListItem::drawActiveBorder(const wxSize &size, wxDC *dc)
{
  if (b_playing)
  {
    dc->SetBrush(wxBrush("blue", wxTRANSPARENT));
    dc->SetPen(wxPen(wxColor(252,122,23), 2));
    dc->DrawRectangle(1, 1, size.GetWidth()-1, size.GetHeight()-1);
  }
}

/*
 * Responsible for drawing the yellow line along with the song
 * giving a visual representation of where we are in the waveform
 */
void JRListItem::drawProgress(const int &length_in_ms,
    const wxSize &size, wxDC *dc)
{
  int drawable_area_width = size.GetWidth()-TOOLBAR_SPACING;
  float rel_pos = static_cast<float>(progress)/static_cast<float>(length_in_ms);
  float abs_pos = rel_pos*CUR_SSS;
  float start_of_interval = arr_cur_zoom[0]*CUR_SSS;
  int width_of_interval = arr_cur_zoom[1]*CUR_SSS - arr_cur_zoom[0]*CUR_SSS;
  float rel_pos_in_int = abs_pos - start_of_interval;
  float positionx = static_cast<float>(drawable_area_width)/width_of_interval
                    * rel_pos_in_int;

  dc->SetPen(wxPen(wxColor(247,250,70), 1));
  dc->DrawLine(positionx, 0, positionx, size.GetHeight());
}

void JRListItem::drawInfoText(const wxSize &size, wxFont &font, wxDC *dc)
{
  int width = size.GetWidth() - TOOLBAR_SPACING;
  if (b_poor_res)
  {
    dc->SetTextForeground(wxColour(200,20,20));
    dc->DrawText("Poor Resolution", width-100, card_height-20);
  }
  else
  {
    wxString text;
    text.Printf("Res: %d", CUR_RESOLUTION);
    font.SetPointSize(8);
    dc->SetFont(font);
    dc->SetTextForeground(wxColour(247,250,70));
    dc->DrawText(text, width-40, card_height-20);
  }
}

void JRListItem::drawName(wxFont &font, wxDC *dc)
{
  wxSize sz = GetClientSize();
  wxDCClipper clip(*dc, 2, 0, sz.GetWidth()-100, sz.GetHeight());
  font.SetPointSize(10);
  dc->SetTextForeground(wxColour(220,220,220));
  dc->SetFont(font.Bold());
  text_ext = dc->GetTextExtent(s_audio_name);
  dc->DrawText(s_audio_name, 2-TEXT_OFFSET, 1);
}

void JRListItem::drawZoomedText(const wxSize &size, wxDC *dc)
{
  dc->SetTextForeground(wxColour(247,250,70));
  if( b_zoomed )
    dc->DrawText( "Zoomed", 24, size.GetHeight()-20);
}

void JRListItem::drawHeightFactor(wxFont &font, const int &width, wxDC *dc)
{
  font.SetPointSize(8);
  dc->SetFont(font);
  wxString h_lvl;
  h_lvl.Printf("H: %.2f", HEIGHTFACTOR);
  dc->DrawText(h_lvl, width-40, 2);
}

/* Returns the amount of samples current viewport is showing in songsmplength
 * Still off-by-one error somewhere
 */
void JRListItem::calcAvgOfBuffersamples(const wxSize &size)
{
  vec_AVG.clear();
  vec_RMS.clear();
  int songsmplength = 0;

  /* Drawvars */
  // amount of pixels within desired zoom-in range
  int interval = vec_samples.size()*(arr_cur_zoom[1]-arr_cur_zoom[0]);
  int width = size.GetWidth()-TOOLBAR_SPACING; // 1244

  /* samples per pixel
   *  if we have less samples available than pixel to draw, simply
   *  set spp (samples per pixel) to 1. (One sample per pixel)
   */
  int spp = interval <= width ? 1 : interval/width; // n/1244
  float sum=0;
  float squaredSum=0;
  int lowerBound = arr_cur_zoom[0]*vec_samples.size();
  int upperBound = arr_cur_zoom[1]*vec_samples.size();

  /* defines max height (y-axis) a sample can be drawn at */
  float tmp_height = static_cast<float>(card_height)*HEIGHTFACTOR/2;
  int tmp=0;

  /* Avg vec_samples per pixel values */
  /* If #smp == #pixels no issues
   * If #smp < #pixels -> Pad with 0's
   * If #smp > #pixels -> Put rest of smp into last fragment
   */
  for(int k=lowerBound; k<upperBound; ++k)
  {
    sum = vec_samples[k]<0 ? sum-vec_samples[k] : sum+vec_samples[k];
    squaredSum = squaredSum + (vec_samples[k]*vec_samples[k]);
    if( ((k+1)%spp==0) && songsmplength <= width )
    {
      /* Potential pitfall because of close subtraction for to fp-values */
      float quot = ((k+1)%spp==0) ? spp : static_cast<float>(k)
                                    - static_cast<float>(songsmplength)*spp;
      /* vec_AVG */
      sum = (std::fabs(sum/quot) <= 1.0) ? (sum*2/quot) : 0;
      vec_AVG.push_back((sum)*(tmp_height)+(card_height/2));
      /* vec_RMS */
      squaredSum = (std::fabs(squaredSum/quot) <= 1.0) ?
                      squaredSum/quot : 0;
      vec_RMS.push_back((std::sqrt(squaredSum)*(tmp_height)+(card_height/2)));

      sum = 0;
      ++songsmplength;
      tmp=k;
    }
    if (k == (upperBound - 1))
    {
      float quot = songsmplength-tmp;
      /* vec_AVG */
      sum = (std::fabs(sum/quot) <= 1.0) ? (sum*2/quot) : 0;
      float tmp = vec_AVG[songsmplength];
      vec_AVG[songsmplength] = ((sum+tmp)*(tmp_height)+(card_height/2));
      /* vec_RMS */
      tmp = vec_RMS[songsmplength];
      squaredSum = (fabs(squaredSum/quot) <= 1.0) ?
                      squaredSum/quot : 0;
      vec_RMS[songsmplength] = (std::sqrt(squaredSum)+tmp)*(tmp_height)+(card_height/2);
      break;
    }
  }

  if (songsmplength < width)
  {
    PADDING = 0;
    b_poor_res = true;
    for( int i=songsmplength; i<width; ++i)
    {
      vec_AVG.push_back(card_height/2);
      vec_RMS.push_back(card_height/2);
      ++PADDING;
    }
  }
  else
    b_poor_res = false;

  b_dirty = false;
  CUR_SSS = songsmplength-1;
}

void JRListItem::drawBorder(const wxSize &size, wxDC *dc)
{
  if( b_focused )
  {
    dc->SetBrush(wxBrush("blue", wxTRANSPARENT));
    dc->SetPen(wxPen(wxColor(200,20,20), 2));
    dc->DrawRectangle(1, 1, size.GetWidth()-1, size.GetHeight()-1);
  }
}

/**********************/
/* Getters- & Setters */
/**********************/

void JRListItem::setHeight(const int &val)
{
  this->card_height = val;
}
int JRListItem::getHeight()
{
  return this->card_height;
}
std::string JRListItem::getOGFilePath()
{
  return s_path_to_orig_file;
}
std::string JRListItem::getRSFilePath()
{
  return s_path_to_dsampled_file;
}
std::string JRListItem::getSongName()
{
  return s_audio_name;
}
int JRListItem::getChannels()
{
  return channels;
}
int JRListItem::getLengthInMS()
{
  return length_in_ms;
}
int JRListItem::getSamplingRate()
{
  return sampling_rate;
}
int JRListItem::getNumberOfSamples()
{
  return nr_of_samples;
}
void JRListItem::setPlaying(const bool &val)
{
  this->b_playing = val;
  this->refresh();
}
void JRListItem::setProgress(const int &val)
{
  this->progress = val;
  this->Refresh();
}
int JRListItem::getProgress()
{
  return progress;
}
int JRListItem::getChunkSize()
{
  return chunk_size;
}
void JRListItem::setHeightFactor(const float &val)
{
  this->HEIGHTFACTOR = val;
}
float JRListItem::getHeightFactor()
{
  return HEIGHTFACTOR;
}
void JRListItem::setLengthInMS(const int &val)
{
  this->length_in_ms = val;
}
int JRListItem::getResolution()
{
  return CUR_RESOLUTION;
}
void JRListItem::setResolution(const int &val)
{
  CUR_RESOLUTION = val;
}
