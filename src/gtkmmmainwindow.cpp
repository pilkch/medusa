// Standard headers
#include <iostream>

// Medusa headers
#include "gtkmmview.h"
#include "gtkmmslider.h"
#include "gtkmmmainwindow.h"
#include "gtkmmtracklist.h"
#include "gtkmmfilebrowse.h"
#include "util.h"

// ** cGtkmmMainWindow

cGtkmmMainWindow::cGtkmmMainWindow(cGtkmmView& _view, cSettings& _settings) :
  view(_view),
  settings(_settings),
  bIsIconified(false),
  pMenuPopup(nullptr),
  boxToolbarAndVolume(Gtk::ORIENTATION_VERTICAL),
  textPosition("0:00"),
  pPositionSlider(nullptr),
  textLength("0:00"),
  textVolumePlus("+"),
  pVolumeSlider(nullptr),
  textVolumeMinus("-"),
  dummyCategories("Categories"),
  dummyStatusBar("StatusBar"),
  pTrackList(nullptr),
  bIsTogglingPlayPause(false),
  bIsTogglingRepeat(false)
{
  set_title("Medusa");
  set_icon_from_file("application.xpm");
  set_border_width(5);
  set_skip_taskbar_hint(true); // Minimise to status icon
  set_size_request(400, 300);
  set_default_size(800, 400);
  resize(400, 300);


  // Status icon
  pStatusIcon = Gtk::StatusIcon::create_from_file("application.xpm");
  pStatusIcon->signal_activate().connect(sigc::mem_fun(*this, &cGtkmmMainWindow::OnStatusIconActivate));
  pStatusIcon->signal_popup_menu().connect(sigc::mem_fun(*this, &cGtkmmMainWindow::OnStatusIconPopupMenu));

  SetStatusIconText(TEXT("Medusa"));

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
  m_refActionGroup->add(Gtk::Action::create("FileAddFiles", "Add files"),
          sigc::mem_fun(*this, &cGtkmmMainWindow::OnActionBrowseFiles));
  m_refActionGroup->add(Gtk::Action::create("FileAddFolder", "Add directory"),
          sigc::mem_fun(*this, &cGtkmmMainWindow::OnActionBrowseFolder));
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
  pPlayPauseAction = Gtk::ToggleAction::create("PlaybackPlayPause", Gtk::Stock::MEDIA_PLAY, "Play/Pause");
  m_refActionGroup->add(pPlayPauseAction, sigc::mem_fun(*this, &cGtkmmMainWindow::OnPlaybackPlayPauseMenuToggled));
  m_refActionGroup->add(Gtk::Action::create("PlaybackNext", Gtk::Stock::MEDIA_NEXT),
          Gtk::AccelKey("<control><alt>S"),
          sigc::mem_fun(*this, &cGtkmmMainWindow::OnPlaybackNextClicked));
  pRepeatAction = Gtk::ToggleAction::create("PlaybackRepeatToggle", Gtk::Stock::GOTO_TOP, "Repeat");
  m_refActionGroup->add(pRepeatAction, sigc::mem_fun(*this, &cGtkmmMainWindow::OnPlaybackRepeatMenuToggled));

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
      "      <menuitem action='FileAddFiles'/>"
      "      <menuitem action='FileAddFolder'/>"
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
      "      <menuitem action='PlaybackRepeatToggle'/>"
      "    </menu>"
      "    <menu action='HelpMenu'>"
      "      <menuitem action='HelpAbout'/>"
      "    </menu>"
      "  </menubar>"
      "  <toolbar name='ToolBar'>"
      "    <toolitem action='PlaybackPrevious'/>"
      "    <toolitem action='PlaybackPlayPause'/>"
      "    <toolitem action='PlaybackNext'/>"
      "    <toolitem action='PlaybackRepeatToggle'/>"
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


  // In gtkmm 3 set_orientation is not supported so we create our own toolbar out of plain old buttons
  //Gtk::Toolbar* pToolbar = dynamic_cast<Gtk::Toolbar*>(m_refUIManager->get_widget("/ToolBar"));
  //if (pToolbar != nullptr) {
  //  pToolbar->property_orientation() = Gtk::ORIENTATION_VERTICAL;
  //  //pToolbar->set_orientation(Gtk::ORIENTATION_VERTICAL);
  //  pToolbar->set_toolbar_style(Gtk::TOOLBAR_ICONS);
  //  boxToolbarAndVolume.pack_start(*pToolbar);
  //}


  buttonAddFiles.signal_clicked().connect(sigc::mem_fun(*this, &cGtkmmMainWindow::OnActionBrowseFiles));
  buttonAddFolder.signal_clicked().connect(sigc::mem_fun(*this, &cGtkmmMainWindow::OnActionBrowseFolder));

  boxToolbarAndVolume.pack_start(buttonAddFiles, Gtk::PACK_SHRINK);
  boxToolbarAndVolume.pack_start(buttonAddFolder, Gtk::PACK_SHRINK);

  buttonPrevious.signal_clicked().connect(sigc::mem_fun(*this, &cGtkmmMainWindow::OnPlaybackPreviousClicked));
  buttonPlayPause.signal_clicked().connect(sigc::mem_fun(*this, &cGtkmmMainWindow::OnPlaybackPlayPauseButtonToggled));
  buttonNext.signal_clicked().connect(sigc::mem_fun(*this, &cGtkmmMainWindow::OnPlaybackNextClicked));

  boxToolbarAndVolume.pack_start(buttonPrevious, Gtk::PACK_SHRINK);
  boxToolbarAndVolume.pack_start(buttonPlayPause, Gtk::PACK_SHRINK);
  boxToolbarAndVolume.pack_start(buttonNext, Gtk::PACK_SHRINK);

  pVolumeSlider = new cGtkmmSlider(*this, true);
  pVolumeSlider->SetRange(0, 100);
  pVolumeSlider->SetValue(settings.GetVolume0To100());

  pVolumeSlider->set_size_request(-1, 100);

  buttonRepeat.signal_clicked().connect(sigc::mem_fun(*this, &cGtkmmMainWindow::OnPlaybackRepeatButtonToggled));

  boxToolbarAndVolume.pack_start(buttonRepeat, Gtk::PACK_SHRINK);

  boxToolbarAndVolume.pack_start(textVolumePlus, Gtk::PACK_SHRINK);
  boxToolbarAndVolume.pack_start(*pVolumeSlider, Gtk::PACK_SHRINK);
  boxToolbarAndVolume.pack_start(textVolumeMinus, Gtk::PACK_SHRINK);


  iconTheme.RegisterThemeChangedListener(*this);

  SetPlaybackButtonIcons();


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
  textCurrentlyPlaying.set_markup("");


  pPositionSlider = new cGtkmmSlider(*this, false);
  pPositionSlider->SetRange(0, 0);

  boxPlaybackButtons.pack_start(textCurrentlyPlaying, Gtk::PACK_SHRINK);
  boxPlaybackButtons.pack_start(*Gtk::manage(new Gtk::Label()), Gtk::PACK_EXPAND_WIDGET); // Spacer


  dummyCategories.set_size_request(150, -1);


  pTrackList = new cGtkmmTrackList(*this);


  //boxCategoriesAndPlaylist.pack_start(dummyCategories, Gtk::PACK_SHRINK); // Hidden for the moment until it is actually functional
  boxCategoriesAndPlaylist.pack_start(pTrackList->GetWidget(), Gtk::PACK_EXPAND_WIDGET);

  boxPositionSlider.pack_start(textPosition, Gtk::PACK_SHRINK);
  boxPositionSlider.pack_start(*pPositionSlider, Gtk::PACK_EXPAND_WIDGET);
  boxPositionSlider.pack_start(textLength, Gtk::PACK_SHRINK);

  boxControls.pack_start(boxPlaybackButtons, Gtk::PACK_SHRINK);
  boxControls.pack_start(boxPositionSlider, Gtk::PACK_SHRINK);
  boxControls.pack_start(boxCategoriesAndPlaylist, Gtk::PACK_EXPAND_WIDGET);

  boxControlsAndToolbar.pack_start(boxControls, Gtk::PACK_EXPAND_WIDGET);
  boxControlsAndToolbar.pack_start(boxToolbarAndVolume, Gtk::PACK_SHRINK);

  boxMainWindow.pack_start(boxControlsAndToolbar, Gtk::PACK_EXPAND_WIDGET);
  boxMainWindow.pack_start(dummyStatusBar, Gtk::PACK_SHRINK);

  // Add the box layout to the main window
  add(boxMainWindow);

  show_all_children();

  if (settings.IsShowMainWindow()) show();
}

void cGtkmmMainWindow::OnThemeChanged()
{
  SetPlaybackButtonIcons();
}

const char* sICON_ADD = "gtk-add";
const char* sICON_DIRECTORY = "gtk-directory";
const char* sICON_MEDIA_PREVIOUS = "gtk-media-previous-ltr";
const char* sICON_MEDIA_PLAY = "gtk-media-play-ltr";
const char* sICON_MEDIA_NEXT = "gtk-media-next-ltr";
const char* sICON_REPEAT_TOGGLE = "gtk-goto-top";

void cGtkmmMainWindow::SetPlaybackButtonIcons()
{
  Gtk::Image* pImageFile = new Gtk::Image;
  iconTheme.LoadStockIcon(sICON_ADD, *pImageFile);
  buttonAddFiles.set_image(*pImageFile);
  Gtk::Image* pImageDirectory = new Gtk::Image;
  iconTheme.LoadStockIcon(sICON_DIRECTORY, *pImageDirectory);
  buttonAddFolder.set_image(*pImageDirectory);

  Gtk::Image* pImagePrevious = new Gtk::Image;
  iconTheme.LoadStockIconRotatedClockwise(sICON_MEDIA_PREVIOUS, *pImagePrevious);
  buttonPrevious.set_image(*pImagePrevious);
  Gtk::Image* pImagePlay = new Gtk::Image;
  iconTheme.LoadStockIcon(sICON_MEDIA_PLAY, *pImagePlay);
  buttonPlayPause.set_image(*pImagePlay);
  Gtk::Image* pImageNext = new Gtk::Image;
  iconTheme.LoadStockIconRotatedClockwise(sICON_MEDIA_NEXT, *pImageNext);
  buttonNext.set_image(*pImageNext);

  Gtk::Image* pImageRepeatToggle = new Gtk::Image;
  iconTheme.LoadStockIconRotatedClockwise(sICON_REPEAT_TOGGLE, *pImageRepeatToggle);
  buttonRepeat.set_image(*pImageRepeatToggle);
}

void cGtkmmMainWindow::OnActionBrowseFiles()
{
}

void cGtkmmMainWindow::OnActionBrowseFolder()
{
}

void cGtkmmMainWindow::SetStatusIconText(const spitfire::string_t& sText)
{
  pStatusIcon->set_tooltip_text(spitfire::string::ToUTF8(sText).c_str());
}

void cGtkmmMainWindow::ShowWindow()
{
  bIsIconified = false;
  deiconify();
  present();
}

void cGtkmmMainWindow::HideWindow()
{
  bIsIconified = true;
  iconify();
}

bool cGtkmmMainWindow::on_delete_event(GdkEventAny* event)
{
  std::cout<<"cGtkmmMainWindow::on_delete_event"<<std::endl;
  (void)event;
  HideWindow();
  return true;
}

void cGtkmmMainWindow::OnStatusIconActivate()
{
  std::cout<<"cGtkmmMainWindow::OnStatusIconActivate bIsIconified="<<(bIsIconified ? "true" : "false")<<std::endl;
  if (bIsIconified) ShowWindow();
  else HideWindow();
}

void cGtkmmMainWindow::OnStatusIconPopupMenu(guint button, guint32 activate_time)
{
  std::cout<<"cGtkmmMainWindow::OnStatusIconPopupMenu"<<std::endl;
  Gtk::Menu* pMenuPopup = dynamic_cast<Gtk::Menu*>(popupUIManagerRef->get_widget("/PopupMenu"));
  assert(pMenuPopup != nullptr);
  pStatusIcon->popup_menu_at_position(*pMenuPopup, button, activate_time);
}

void cGtkmmMainWindow::on_menu_file_quit()
{
  std::cout<<"cGtkmmMainWindow::on_menu_file_quit"<<std::endl;

  // Save volume settings
  settings.SetVolume0To100(pVolumeSlider->GetValue());

  //hide(); //Closes the main window to stop the Gtk::Main::run().
  Gtk::Main::quit();
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

void cGtkmmMainWindow::OnActionSliderValueChanged(const cGtkmmSlider& slider, uint64_t uiValue)
{
  if (&slider == pPositionSlider) OnActionPlaybackPositionValueChanged(uiValue);
  else OnActionVolumeValueChanged(uiValue);
}

void cGtkmmMainWindow::OnActionPlayTrack(const cTrack* pTrack)
{
  view.OnActionPlayTrack(pTrack);
}

void cGtkmmMainWindow::OnPlaybackPlayPauseMenuToggled()
{
  if (!bIsTogglingPlayPause) {
    // Toggle the play/pause button
    bIsTogglingPlayPause = true;
    buttonPlayPause.set_active(!buttonPlayPause.get_active());
    bIsTogglingPlayPause = false;

    // Perform the action
    view.OnActionPlayPause();
  }
}

void cGtkmmMainWindow::OnPlaybackPlayPauseButtonToggled()
{
  if (!bIsTogglingPlayPause) {
    // Toggle the play/pause button
    bIsTogglingPlayPause = true;
    pPlayPauseAction->set_active(!pPlayPauseAction->get_active());
    bIsTogglingPlayPause = false;

    // Perform the action
    view.OnActionPlayPause();
  }
}

void cGtkmmMainWindow::OnPlaybackRepeatMenuToggled()
{
  if (!bIsTogglingRepeat) {
    // Toggle the repeat button
    bIsTogglingRepeat = true;
    buttonRepeat.set_active(!buttonRepeat.get_active());
    bIsTogglingRepeat = false;

    // Perform the action
    //view.OnActionRepeatToggle();
  }
}

void cGtkmmMainWindow::OnPlaybackRepeatButtonToggled()
{
  if (!bIsTogglingRepeat) {
    // Toggle the repeat button
    bIsTogglingRepeat = true;
    pRepeatAction->set_active(!pRepeatAction->get_active());
    bIsTogglingRepeat = false;

    // Perform the action
    //view.OnActionRepeatToggle();
  }
}

void cGtkmmMainWindow::OnPlaybackPreviousClicked()
{
  std::cout<<"cGtkmmMainWindow::OnPlaybackPreviousClicked"<<std::endl;
  OnActionPlayPreviousTrack();
}

void cGtkmmMainWindow::OnPlaybackNextClicked()
{
  std::cout<<"cGtkmmMainWindow::OnPlaybackNextClicked"<<std::endl;
  OnActionPlayNextTrack();
}

const cTrack* cGtkmmMainWindow::GetPreviousTrack() const
{
  const cTrack* pCurrentTrack = view.GetTrack();
  if (pCurrentTrack == nullptr) {
    // There was no current track so just pick the last one
    const cTrack* pPreviousTrack = nullptr;
    cGtkmmTrackListIterator iter(*pTrackList);
    while (iter.IsValid()) {
      const Gtk::TreeModel::Row& row = iter.GetRow();
      pPreviousTrack = pTrackList->GetTrackFromRow(row);

      iter.Next();
    }

    return pPreviousTrack;
  } else {
    const cTrack* pPreviousTrack = nullptr;
    cGtkmmTrackListIterator iter(*pTrackList);
    while (iter.IsValid()) {
      const Gtk::TreeModel::Row& row = iter.GetRow();
      const cTrack* pThisTrack = pTrackList->GetTrackFromRow(row);
      if (pThisTrack == pCurrentTrack) {
        // Found our current track so return the previous one
        return pPreviousTrack;
      }

      pPreviousTrack = pThisTrack;

      iter.Next();
    }
  }

  return nullptr;
}

const cTrack* cGtkmmMainWindow::GetNextTrack() const
{
  const cTrack* pCurrentTrack = view.GetTrack();
  if (pCurrentTrack == nullptr) {
    // There was no current track so just pick the first one
    cGtkmmTrackListIterator iter(*pTrackList);
    if (iter.IsValid()) {
      const Gtk::TreeModel::Row& row = iter.GetRow();
      return pTrackList->GetTrackFromRow(row);
    }
  } else {
    cGtkmmTrackListIterator iter(*pTrackList);
    while (iter.IsValid()) {
      const Gtk::TreeModel::Row& row = iter.GetRow();
      const cTrack* pNextTrack = pTrackList->GetTrackFromRow(row);
      if (pNextTrack == pCurrentTrack) {
        // Found our current track so return the next one
        iter.Next();

        if (iter.IsValid()) {
          const Gtk::TreeModel::Row& row = iter.GetRow();
          return pTrackList->GetTrackFromRow(row);
        }

        break;
      }

      iter.Next();
    }
  }

  return nullptr;
}

void cGtkmmMainWindow::OnActionPlayPreviousTrack()
{
  const cTrack* pPreviousTrack = GetPreviousTrack();
  if (pPreviousTrack != nullptr) OnActionPlayTrack(pPreviousTrack);
}

void cGtkmmMainWindow::OnActionPlayNextTrack()
{
  const cTrack* pNextTrack = GetNextTrack();
  if (pNextTrack != nullptr) OnActionPlayTrack(pNextTrack);
}

void cGtkmmMainWindow::SetPlaybackPositionMS(uint64_t milliseconds)
{
  //std::cout<<"cGtkmmMainWindow::SetPlaybackPositionMS "<<milliseconds<<"\n";
  pPositionSlider->SetValue(double(milliseconds) / 1000.0f);

  textPosition.set_text(spitfire::string::ToUTF8(medusa::util::FormatTime(milliseconds)).c_str());
}

void cGtkmmMainWindow::SetPlaybackLengthMS(uint64_t milliseconds)
{
  std::cout<<"cGtkmmMainWindow::SetPlaybackLengthMS "<<milliseconds<<"\n";
  pPositionSlider->SetRange(0, double(milliseconds) / 1000.0f);

  textLength.set_text(spitfire::string::ToUTF8(medusa::util::FormatTime(milliseconds)).c_str());
}

void cGtkmmMainWindow::SetStatePlaying(const cTrack* pTrack)
{
  spitfire::ostringstream_t o;
  if (!pTrack->metaData.sArtist.empty()) o<<"<b><big>"<<pTrack->metaData.sArtist<<"</big></b>";
  if (!pTrack->metaData.sArtist.empty() && !pTrack->metaData.sTitle.empty()) o<<" - ";
  if (!pTrack->metaData.sTitle.empty()) o<<"<b><big>"<<pTrack->metaData.sTitle<<"</big></b>";
  if ((pTrack->metaData.uiTracknum != 0) && !pTrack->metaData.sAlbum.empty()) o<<",";
  if (pTrack->metaData.uiTracknum != 0) o<<" track "<<pTrack->metaData.uiTracknum;
  if (!pTrack->metaData.sAlbum.empty()) o<<" on "<<pTrack->metaData.sAlbum;

  textCurrentlyPlaying.set_markup(spitfire::string::ToUTF8(o.str()).c_str());

  // Update position slider
  SetPlaybackLengthMS(pTrack->metaData.uiDurationMilliSeconds);

  pTrackList->SetStatePlaying(pTrack);

  bIsTogglingPlayPause = true;
  buttonPlayPause.set_active(true);
  pPlayPauseAction->set_active(true);
  bIsTogglingPlayPause = false;
}

void cGtkmmMainWindow::SetStatePaused()
{
  pTrackList->SetStatePaused(view.GetTrack());

  bIsTogglingPlayPause = true;
  buttonPlayPause.set_active(false);
  pPlayPauseAction->set_active(false);
  bIsTogglingPlayPause = false;
}
