// Standard headers
#include <iostream>
#include <iomanip>

// Medusa headers
#include "gtkmmview.h"
#include "gtkmmhorizontalslider.h"
#include "gtkmmmainwindow.h"
#include "gtkmmtracklist.h"

// ** cGtkmmMainWindow

cGtkmmMainWindow::cGtkmmMainWindow(cGtkmmView& _view) :
  view(_view),
  pMenuPopup(nullptr),
  boxToolbar(Gtk::ORIENTATION_VERTICAL),
  textVolumeMinus("-"),
  pVolumeSlider(nullptr),
  textVolumePlus("+"),
  textPosition("0:00"),
  pPositionSlider(nullptr),
  textLength("0:00"),
  dummyCategories("Categories"),
  dummyStatusBar("StatusBar"),
  pTrackList(nullptr)
{
  set_title("Medusa");
  set_border_width(5);
  set_size_request(400, 300);
  set_default_size(800, 600);

  // Menu and toolbar

  // Create actions for menus and toolbars
  m_refActionGroup = Gtk::ActionGroup::create();

  // File|New sub menu
  m_refActionGroup->add(Gtk::Action::create("FileNewStandard",
              Gtk::Stock::NEW, "_New", "Create a new file"),
          sigc::mem_fun(*this, &cGtkmmMainWindow::on_menu_file_new_generic));

  m_refActionGroup->add(Gtk::Action::create("FileNewFoo",
              Gtk::Stock::NEW, "New Foo", "Create a new foo"),
          sigc::mem_fun(*this, &cGtkmmMainWindow::on_menu_file_new_generic));

  m_refActionGroup->add(Gtk::Action::create("FileNewGoo",
              Gtk::Stock::NEW, "_New Goo", "Create a new goo"),
          sigc::mem_fun(*this, &cGtkmmMainWindow::on_menu_file_new_generic));

  // File menu
  m_refActionGroup->add(Gtk::Action::create("FileMenu", "File"));
  // Sub-menu
  m_refActionGroup->add(Gtk::Action::create("FileNew", Gtk::Stock::NEW));
  m_refActionGroup->add(Gtk::Action::create("FileQuit", Gtk::Stock::QUIT),
          sigc::mem_fun(*this, &cGtkmmMainWindow::on_menu_file_quit));

  // Edit menu
  m_refActionGroup->add(Gtk::Action::create("EditMenu", "Edit"));
  m_refActionGroup->add(Gtk::Action::create("EditCopy", Gtk::Stock::COPY),
          sigc::mem_fun(*this, &cGtkmmMainWindow::on_menu_others));
  m_refActionGroup->add(Gtk::Action::create("EditPaste", Gtk::Stock::PASTE),
          sigc::mem_fun(*this, &cGtkmmMainWindow::on_menu_others));
  m_refActionGroup->add(Gtk::Action::create("EditSomething", "Something"),
          Gtk::AccelKey("<control><alt>S"),
          sigc::mem_fun(*this, &cGtkmmMainWindow::on_menu_others));

  // Playback menu
  m_refActionGroup->add(Gtk::Action::create("PlaybackMenu", "Playback"));
  m_refActionGroup->add(Gtk::Action::create("PlaybackPrevious", Gtk::Stock::MEDIA_PREVIOUS),
          sigc::mem_fun(*this, &cGtkmmMainWindow::OnPlaybackPreviousClicked));
  m_refActionGroup->add(Gtk::Action::create("PlaybackPlayPause", Gtk::Stock::MEDIA_PLAY),
          sigc::mem_fun(*this, &cGtkmmMainWindow::OnPlaybackPlayPauseClicked));
  m_refActionGroup->add(Gtk::Action::create("PlaybackNext", Gtk::Stock::MEDIA_NEXT),
          Gtk::AccelKey("<control><alt>S"),
          sigc::mem_fun(*this, &cGtkmmMainWindow::OnPlaybackNextClicked));

  // Help menu
  m_refActionGroup->add( Gtk::Action::create("HelpMenu", "Help") );
  m_refActionGroup->add( Gtk::Action::create("HelpAbout", Gtk::Stock::HELP),
          sigc::mem_fun(*this, &cGtkmmMainWindow::on_menu_others) );

  m_refUIManager = Gtk::UIManager::create();
  m_refUIManager->insert_action_group(m_refActionGroup);

  add_accel_group(m_refUIManager->get_accel_group());

  // Layout the actions in a menubar and toolbar
  {
    Glib::ustring ui_info =
      "<ui>"
      "  <menubar name='MenuBar'>"
      "    <menu action='FileMenu'>"
      "      <menu action='FileNew'>"
      "        <menuitem action='FileNewStandard'/>"
      "        <menuitem action='FileNewFoo'/>"
      "        <menuitem action='FileNewGoo'/>"
      "      </menu>"
      "      <separator/>"
      "      <menuitem action='FileQuit'/>"
      "    </menu>"
      "    <menu action='EditMenu'>"
      "      <menuitem action='EditCopy'/>"
      "      <menuitem action='EditPaste'/>"
      "      <menuitem action='EditSomething'/>"
      "    </menu>"
      "    <menu action='PlaybackMenu'>"
      "      <menuitem action='PlaybackPrevious'/>"
      "      <menuitem action='PlaybackPlayPause'/>"
      "      <menuitem action='PlaybackNext'/>"
      "    </menu>"
      "    <menu action='HelpMenu'>"
      "      <menuitem action='HelpAbout'/>"
      "    </menu>"
      "  </menubar>"
      "  <toolbar  name='ToolBar'>"
      "    <toolitem action='PlaybackPrevious'/>"
      "    <toolitem action='PlaybackPlayPause'/>"
      "    <toolitem action='PlaybackNext'/>"
      "  </toolbar>"
      "</ui>";

    try {
      m_refUIManager->add_ui_from_string(ui_info);
    }
    catch(const Glib::Error& ex) {
      std::cerr<<"building menus failed: "<<ex.what();
    }
  }

  // Get the menubar and toolbar widgets, and add them to a container widget
  Gtk::Widget* pMenubar = m_refUIManager->get_widget("/MenuBar");
  if (pMenubar != nullptr) boxMainWindow.pack_start(*pMenubar, Gtk::PACK_SHRINK);

  Gtk::Toolbar* pToolbar = dynamic_cast<Gtk::Toolbar*>(m_refUIManager->get_widget("/ToolBar"));
  if (pToolbar != nullptr) {
    pToolbar->set_orientation(Gtk::ORIENTATION_VERTICAL);
    pToolbar->set_toolbar_style(Gtk::TOOLBAR_ICONS);
    boxToolbar.pack_start(*pToolbar);
  }



  // Popup menu
  popupActionGroupRef = Gtk::ActionGroup::create();

  //File|New sub menu:
  //These menu actions would normally already exist for a main menu, because a
  //context menu should not normally contain menu items that are only available
  //via a context menu.
  popupActionGroupRef->add(Gtk::Action::create("ContextMenu", "Context Menu"));

  popupActionGroupRef->add(Gtk::Action::create("ContextEdit", "Edit"),
          sigc::mem_fun(*this, &cGtkmmMainWindow::on_menu_file_popup_generic));

  popupActionGroupRef->add(Gtk::Action::create("ContextProcess", "Process"),
          Gtk::AccelKey("<control>P"),
          sigc::mem_fun(*this, &cGtkmmMainWindow::on_menu_file_popup_generic));

  popupActionGroupRef->add(Gtk::Action::create("ContextRemove", "Remove"),
          sigc::mem_fun(*this, &cGtkmmMainWindow::on_menu_file_popup_generic));

  popupUIManagerRef = Gtk::UIManager::create();
  popupUIManagerRef->insert_action_group(popupActionGroupRef);

  add_accel_group(popupUIManagerRef->get_accel_group());

  // Layout the actions in our popup menu
  {
    Glib::ustring ui_info =
      "<ui>"
      "  <popup name='PopupMenu'>"
      "    <menuitem action='ContextEdit'/>"
      "    <menuitem action='ContextProcess'/>"
      "    <menuitem action='ContextRemove'/>"
      "  </popup>"
      "</ui>";

    try
    {
      popupUIManagerRef->add_ui_from_string(ui_info);
    }
    catch(const Glib::Error& ex)
    {
      std::cerr<<"building menus failed: "<<ex.what();
    }
  }

  //Get the menu:
  pMenuPopup = dynamic_cast<Gtk::Menu*>(popupUIManagerRef->get_widget("/PopupMenu"));
  if (pMenuPopup == nullptr) g_warning("menu not found");



  // Controls

  // Set the currently playing song information
  textCurrentlyPlaying.set_use_markup(true);
  textCurrentlyPlaying.set_markup("<b><big>Artist</big></b> - <b><big>Title</big></b>, track 1 on Album");

  pVolumeSlider = new cGtkmmHorizontalSlider(*this);
  pVolumeSlider->SetValue(100);
  pVolumeSlider->SetRange(0, 100);


  pPositionSlider = new cGtkmmHorizontalSlider(*this);

  SetPlaybackLengthMS(1000);

  pVolumeSlider->set_size_request(100, -1);

  boxPlaybackButtons.pack_start(textCurrentlyPlaying, Gtk::PACK_SHRINK);
  boxPlaybackButtons.pack_start(*Gtk::manage(new Gtk::Label()), Gtk::PACK_EXPAND_WIDGET); // Spacer
  boxPlaybackButtons.pack_start(textVolumeMinus, Gtk::PACK_SHRINK);
  boxPlaybackButtons.pack_start(*pVolumeSlider, Gtk::PACK_SHRINK);
  boxPlaybackButtons.pack_start(textVolumePlus, Gtk::PACK_SHRINK);


  dummyCategories.set_size_request(150, -1);


  pTrackList = new cGtkmmTrackList(*this);


  boxCategoriesAndPlaylist.pack_start(dummyCategories, Gtk::PACK_SHRINK);
  boxCategoriesAndPlaylist.pack_start(pTrackList->GetWidget(), Gtk::PACK_EXPAND_WIDGET);

  boxPositionSlider.pack_start(textPosition, Gtk::PACK_SHRINK);
  boxPositionSlider.pack_start(*pPositionSlider, Gtk::PACK_EXPAND_WIDGET);
  boxPositionSlider.pack_start(textLength, Gtk::PACK_SHRINK);

  boxControls.pack_start(boxPlaybackButtons, Gtk::PACK_SHRINK);
  boxControls.pack_start(boxPositionSlider, Gtk::PACK_SHRINK);
  boxControls.pack_start(boxCategoriesAndPlaylist, Gtk::PACK_EXPAND_WIDGET);

  boxControlsAndToolbar.pack_start(boxControls, Gtk::PACK_EXPAND_WIDGET);
  boxControlsAndToolbar.pack_start(boxToolbar, Gtk::PACK_SHRINK);

  boxMainWindow.pack_start(boxControlsAndToolbar, Gtk::PACK_EXPAND_WIDGET);
  boxMainWindow.pack_start(dummyStatusBar, Gtk::PACK_SHRINK);

  // Add the box layout to the main window
  add(boxMainWindow);

  show_all_children();
}

void cGtkmmMainWindow::on_menu_file_quit()
{
  hide(); //Closes the main window to stop the Gtk::Main::run().
}

void cGtkmmMainWindow::on_menu_file_new_generic()
{
   std::cout<<"A File|New menu item was selected."<<std::endl;
}

void cGtkmmMainWindow::on_menu_others()
{
  std::cout<<"A menu item was selected."<<std::endl;
}

void cGtkmmMainWindow::on_menu_file_popup_generic()
{
   std::cout<<"cGtkmmMainWindow::on_menu_file_popup_generic A popup menu item was selected"<<std::endl;
   cGtkmmTrackListSelectedIterator iter(*pTrackList);
   while (iter.IsValid()) {
     std::cout<<"cGtkmmMainWindow::on_menu_file_popup_generic Item was selected"<<std::endl;
     const Gtk::TreeModel::Row& row = iter.GetRow();
     const cTrack* pTrack = pTrackList->GetTrackFromRow(row);
     if (pTrack != nullptr) std::wcout<<"Properties selected for track "<<pTrack->metaData.sArtist<<" - "<<pTrack->metaData.sTitle<<std::endl;
     else std::cout<<"cGtkmmMainWindow::on_menu_file_popup_generic Could not get track from row"<<std::endl;

     iter.Next();
   }
}

void cGtkmmMainWindow::OnActionPlaylistRightClick(GdkEventButton* event)
{
  if (pMenuPopup != nullptr) pMenuPopup->popup(event->button, event->time);
}

void cGtkmmMainWindow::OnActionPlaybackPositionValueChanged(uint64_t uiValue)
{
  view.OnActionPlaybackPositionChanged(uiValue);
}

void cGtkmmMainWindow::OnActionVolumeValueChanged(unsigned int uiVolume0To100)
{
  view.OnActionVolumeChanged(uiVolume0To100);
}

void cGtkmmMainWindow::OnActionSliderValueChanged(const cGtkmmHorizontalSlider& slider, uint64_t uiValue)
{
  if (&slider == pPositionSlider) OnActionPlaybackPositionValueChanged(uiValue);
  else OnActionVolumeValueChanged(uiValue);
}

void cGtkmmMainWindow::OnActionPlayTrack(const cTrack* pTrack)
{
  view.OnActionPlayTrack(pTrack);
}

void cGtkmmMainWindow::OnPlaybackPreviousClicked()
{
  std::cout<<"cGtkmmMainWindow::OnPlaybackPreviousClicked"<<std::endl;
  //view.OnActionPlayPause();
}

void cGtkmmMainWindow::OnPlaybackPlayPauseClicked()
{
  view.OnActionPlayPause();
}

void cGtkmmMainWindow::OnPlaybackNextClicked()
{
  std::cout<<"cGtkmmMainWindow::OnPlaybackNextClicked"<<std::endl;
  //view.OnActionPlayPause();
}

bool cGtkmmMainWindow::OnTimerPlaybackPosition()
{
  view.OnActionTimerUpdatePlaybackPosition();

  return true;
}

std::string cGtkmmMainWindow::TimeToString(uint64_t milliseconds) const
{
  std::ostringstream o;

  o<<std::right<<std::setfill('0');

  uint64_t time = milliseconds / 1000;

  const uint64_t seconds = time % 60;
  time /= 60;

  const uint64_t minutes = time % 60;
  time /= 60;

  const uint64_t hours = time % 24;
  time /= 24;

  const uint64_t days = hours;

  if (days != 0) {
    o<<days<<":";
    o<<std::setw(2); // Set width of 2 for the next value
  }

  if (hours != 0)  {
    o<<hours<<":";
    o<<std::setw(2); // Set width of 2 for the next value
  }

  o<<minutes<<":";
  o<<std::setw(2)<<seconds;

  return o.str();
}

void cGtkmmMainWindow::SetPlaybackPositionMS(uint64_t milliseconds)
{
  //std::cout<<"cGtkmmMainWindow::SetPlaybackPositionMS "<<milliseconds<<"\n";
  pPositionSlider->SetValue(double(milliseconds) / 1000.0f);

  textPosition.set_text(TimeToString(milliseconds).c_str());
}

void cGtkmmMainWindow::SetPlaybackLengthMS(uint64_t milliseconds)
{
  std::cout<<"cGtkmmMainWindow::SetPlaybackLengthMS "<<milliseconds<<"\n";
  pPositionSlider->SetRange(0, double(milliseconds) / 1000.0f);

  textLength.set_text(TimeToString(milliseconds).c_str());
}

void cGtkmmMainWindow::SetStatePlaying(const cTrack* pTrack)
{
  //buttonPlay.set_label("Pause");

  // Update position slider
  SetPlaybackLengthMS(pTrack->metaData.uiDurationMilliSeconds);

  pTrackList->SetStatePlaying(pTrack);

  // Call OnTimerPlaybackPosition function at a 200ms
  // interval to regularly update the position of the stream
  timeoutConnection = Glib::signal_timeout().connect(sigc::mem_fun(*this, &cGtkmmMainWindow::OnTimerPlaybackPosition), 200);
}

void cGtkmmMainWindow::SetStatePaused()
{
  //buttonPlay.set_label("Play");

  pTrackList->SetStatePaused(view.GetTrack());

  // Disconnect the progress signal handler:
  timeoutConnection.disconnect();
}
