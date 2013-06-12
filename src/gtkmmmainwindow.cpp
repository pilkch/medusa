// Standard headers
#include <iostream>

// Spitfire headers
#include <spitfire/storage/filesystem.h>
#include <spitfire/platform/notifications.h>
#include <spitfire/platform/operatingsystem.h>

// Medusa headers
#include "discombobulator_lastfm_key.h"
#include "discombobulator_lastfm_secret.h"
#include "gtkmmabout.h"
#include "gtkmmview.h"
#include "gtkmmalertdialog.h"
#include "gtkmmslider.h"
#include "gtkmmmainwindow.h"
#include "gtkmmtracklist.h"
#include "gtkmmfilebrowse.h"
#include "gtkmmpreferencesdialog.h"
#include "trackmetadata.h"
#include "util.h"

namespace medusa
{
  const size_t NOTIFICATION_PLAYBACK = 400;
  const size_t NOTIFICATION_PREVIOUS = 401;
  const size_t NOTIFICATION_PAUSE = 402;
  const size_t NOTIFICATION_NEXT = 403;


  // ** cFolderList

  bool cFolderList::AreFullPathsUnique() const
  {
    const size_t n = folders.size();
    for (size_t i = 0; i < n; i++) {
      const string_t& sFullPath = folders[i].sFullPath;
      std::cout<<"cFolderList::AreFullPathsUnique Looking at "<<sFullPath<<std::endl;
      for (size_t j = i + 1; j < n; j++) {
        std::cout<<"cFolderList::AreFullPathsUnique Checking "<<folders[j].sFullPath<<std::endl;
        if (sFullPath == folders[j].sFullPath) return false;
      }
    }

    return true;
  }

  bool cFolderList::AreShortPathsUnique() const
  {
    const size_t n = folders.size();
    for (size_t i = 0; i < n; i++) {
      const string_t& sShortPath = folders[i].sShortPath;
      for (size_t j = i + 1; j < n; j++) {
        if (sShortPath == folders[j].sShortPath) return false;
      }
    }

    return true;
  }

  std::set<size_t> cFolderList::GetNonUniqueShortPaths() const
  {
    std::set<size_t> nonUniquePaths;

    const size_t n = folders.size();
    for (size_t i = 0; i < n; i++) {
      const string_t& sShortPath = folders[i].sShortPath;
      std::cout<<"cFolderList::GetNonUniqueShortPaths Looking at "<<sShortPath<<std::endl;
      for (size_t j = i + 1; j < n; j++) {
        std::cout<<"cFolderList::GetNonUniqueShortPaths Checking "<<folders[j].sShortPath<<std::endl;
        if (sShortPath == folders[j].sShortPath) {
          nonUniquePaths.insert(i);
          nonUniquePaths.insert(j);
        }
      }
    }

    return nonUniquePaths;
  }

  void cFolderList::AddPath(const string_t& sFullPath)
  {
    {
      // Return if we have already added this path
      const size_t n = folders.size();
      for (size_t i = 0; i < n; i++) {
        if (folders[i].sFullPath == sFullPath) return;
      }
    }

    // Add a new entry on the end if we have less than the maximum number of entries
    if (folders.size() < 5) folders.push_back(cFolder());

    // Shuffle the entries
    ASSERT(!folders.empty());
    const size_t n = folders.size() - 1;
    for (size_t i = 0; i < n; i++) folders[n - i] = folders[n - i - 1];

    // Set the first entry to the new path
    folders[0].sFullPath = sFullPath;
    folders[0].sShortPath = TEXT("");
  }

  const std::vector<cFolder>& cFolderList::GetPaths() const
  {
    return folders;
  }

  std::vector<string_t> cFolderList::GetFullPaths() const
  {
    std::vector<string_t> paths;
    const size_t n = folders.size();
    for (size_t i = 0; i < n; i++) paths.push_back(folders[i].sFullPath);
    return paths;
  }

  void cFolderList::UpdatePaths()
  {
    // Ensure full paths are unique
    ASSERT(AreFullPathsUnique());

    // Set all short paths to the lowest
    const size_t n = folders.size();
    for (size_t i = 0; i < n; i++) {
      folders[i].sShortPath = spitfire::filesystem::GetFile(folders[i].sFullPath);
    }

    // Keep track of how many parts each short folder is using
    std::vector<size_t> parts;
    parts.insert(parts.begin(), n, 1);

    // Ensure short paths are unique
    while (!AreShortPathsUnique()) {
      // Find all non unique paths
      std::set<size_t> nonUniquePaths = GetNonUniqueShortPaths();

      // Expand all non unique paths by one folder
      std::set<size_t>::const_iterator iter = nonUniquePaths.begin();
      const std::set<size_t>::const_iterator iterEnd = nonUniquePaths.end();
      while (iter != iterEnd) {
        const size_t i = *iter;
        spitfire::filesystem::cFilePathParser parser(folders[i].sFullPath);
        parts[i]++;
        ASSERT(parts[i] < parser.GetFolderCount());
        folders[i].sShortPath = parser.GetFolder(parser.GetFolderCount() - parts[i]) + spitfire::filesystem::sFilePathSeparator + folders[i].sShortPath;

        iter++;
      }
    }
  }


// ** cGtkmmMainWindow

cGtkmmMainWindow::cGtkmmMainWindow(cGtkmmView& _view, cSettings& _settings) :
  view(_view),
  settings(_settings),
  updateChecker(view),
  bIsIconified(false),
  pMenuPopup(nullptr),
  pMenuPopupRecentMoveToFolder(nullptr),
  bIsRecentMoveToFolderSeparatorAdded(false),
  pStatusIconPopupMenu(nullptr),
  boxToolbarAndVolume(Gtk::ORIENTATION_VERTICAL),
  textPosition("0:00"),
  pPositionSlider(nullptr),
  textLength("0:00"),
  textVolumePlus("+"),
  pVolumeSlider(nullptr),
  textVolumeMinus("-"),
  dummyCategories("Categories"),
  statusBar("0 tracks"),
  nTracksLoading(0),
  pTrackList(nullptr),
  bIsTogglingPlayPause(false),
  bIsTogglingRepeat(false)
{
  std::cout<<"cGtkmmMainWindow::cGtkmmMainWindow"<<std::endl;

  set_title("Medusa");
  //set_skip_taskbar_hint(true); // Minimise to status icon
  set_size_request(400, 300);
  set_default_size(800, 400);
  resize(400, 300);

  // Set icon list
  std::vector<Glib::RefPtr<Gdk::Pixbuf> > icons;
  icons.push_back(Gdk::Pixbuf::create_from_file("data/application_16x16.png"));
  icons.push_back(Gdk::Pixbuf::create_from_file("data/application_32x32.png"));
  icons.push_back(Gdk::Pixbuf::create_from_file("data/application_48x48.png"));
  icons.push_back(Gdk::Pixbuf::create_from_file("data/application_64x64.png"));
  icons.push_back(Gdk::Pixbuf::create_from_file("data/application_128x128.png"));
  icons.push_back(Gdk::Pixbuf::create_from_file("data/application_256x256.png"));
  // GTK complains if we pass icons larger than 256x256
  //icons.push_back(Gdk::Pixbuf::create_from_file("data/application_512x512.png"));
  set_icon_list(icons);

  // Status icon
  pStatusIcon = Gtk::StatusIcon::create_from_file("data/application_64x64.png");
  pStatusIcon->signal_activate().connect(sigc::mem_fun(*this, &cGtkmmMainWindow::OnStatusIconActivate));
  pStatusIcon->signal_popup_menu().connect(sigc::mem_fun(*this, &cGtkmmMainWindow::OnStatusIconPopupMenu));

  SetStatusIconText(TEXT("Medusa"));

  // Handle popup notifications
  spitfire::operatingsystem::NotificationInit(*this);

  // Drag and drop from Nautilus
  std::vector<Gtk::TargetEntry> listTargets;
  listTargets.push_back(Gtk::TargetEntry("text/uri-list"));

  // Set the whole window to be a drag target (We could also just make the listview a drag target instead)
  drag_dest_set(listTargets, Gtk::DEST_DEFAULT_MOTION | Gtk::DEST_DEFAULT_DROP, Gdk::ACTION_COPY | Gdk::ACTION_MOVE);
  // Install our drag target event handler
  signal_drag_data_received().connect(sigc::mem_fun(*this, &cGtkmmMainWindow::OnFileDroppedFromNautilus));


  // Menu and toolbar

  // Create actions for menus and toolbars
  m_refActionGroup = Gtk::ActionGroup::create();

  // File menu
  m_refActionGroup->add(Gtk::Action::create("FileMenu", "File"));
  m_refActionGroup->add(Gtk::Action::create("FileAddFiles", "Add Files"),
          sigc::mem_fun(*this, &cGtkmmMainWindow::OnActionBrowseFiles));
  m_refActionGroup->add(Gtk::Action::create("FileAddFolder", "Add Folder"),
          sigc::mem_fun(*this, &cGtkmmMainWindow::OnActionBrowseFolder));
  m_refActionGroup->add(Gtk::Action::create("FileAddFilesFromMusicFolder", "Add Files From Music Folder"),
          sigc::mem_fun(*this, &cGtkmmMainWindow::OnActionAddFilesFromMusicFolder));
  #ifdef BUILD_MEDUSA_IMPORT_BANSHEE_PLAYLIST
  Glib::RefPtr<Gtk::Action> pActionImportFromBanshee = Gtk::Action::create("FileImportFromBanshee", "Import from Banshee");
  m_refActionGroup->add(pActionImportFromBanshee,
          sigc::mem_fun(*this, &cGtkmmMainWindow::OnActionImportFromBanshee));
  #endif
  Glib::RefPtr<Gtk::Action> pActionImportFromRhythmBox = Gtk::Action::create("FileImportFromRhythmbox", "Import from Rhythmbox");
  m_refActionGroup->add(pActionImportFromRhythmBox,
          sigc::mem_fun(*this, &cGtkmmMainWindow::OnActionImportFromRhythmbox));
  m_refActionGroup->add(Gtk::Action::create("FileRemove", "Remove"),
          sigc::mem_fun(*this, &cGtkmmMainWindow::OnActionRemoveTrack));
  m_refActionGroup->add(Gtk::Action::create("FileMoveToFolderMenu", "Move to Folder"));
  m_refActionGroup->add(Gtk::Action::create("FileMoveToFolderBrowse", "Browse..."),
          sigc::mem_fun(*this, &cGtkmmMainWindow::OnActionTrackMoveToFolderBrowse));
  m_refActionGroup->add(Gtk::Action::create("FileMoveToRubbishBin", "Move to the Rubbish Bin"),
          sigc::mem_fun(*this, &cGtkmmMainWindow::OnActionTrackMoveToRubbishBin));
  m_refActionGroup->add(Gtk::Action::create("FileShowInFileManager", "Show in the File Manager"),
          sigc::mem_fun(*this, &cGtkmmMainWindow::OnActionTrackShowInFileManager));
  m_refActionGroup->add(Gtk::Action::create("FileProperties", "Properties"),
          sigc::mem_fun(*this, &cGtkmmMainWindow::OnActionTrackProperties));
  m_refActionGroup->add(Gtk::Action::create("FileQuit", Gtk::Stock::QUIT),
          sigc::mem_fun(*this, &cGtkmmMainWindow::OnMenuFileQuit));

  #ifdef BUILD_MEDUSA_IMPORT_BANSHEE_PLAYLIST
  pActionImportFromBanshee->set_sensitive(medusa::util::IsBansheePlaylistFilePresent());
  #endif
  pActionImportFromRhythmBox->set_sensitive(medusa::util::IsRhythmBoxPlaylistFilePresent());
  // Edit menu
  m_refActionGroup->add(Gtk::Action::create("EditMenu", "Edit"));
  m_refActionGroup->add(Gtk::Action::create("EditPreferences", Gtk::Stock::PREFERENCES),
          sigc::mem_fun(*this, &cGtkmmMainWindow::OnMenuEditPreferences));

  // Playback menu
  m_refActionGroup->add(Gtk::Action::create("PlaybackMenu", "Playback"));
  m_refActionGroup->add(Gtk::Action::create("PlaybackPrevious", Gtk::Stock::MEDIA_PREVIOUS),
          sigc::mem_fun(*this, &cGtkmmMainWindow::OnPlaybackPreviousClicked));
  pPlayPauseAction = Gtk::ToggleAction::create("PlaybackPlayPause", Gtk::Stock::MEDIA_PLAY, "Play/Pause");
  m_refActionGroup->add(pPlayPauseAction, sigc::mem_fun(*this, &cGtkmmMainWindow::OnPlaybackPlayPauseMenuToggled));
  m_refActionGroup->add(Gtk::Action::create("PlaybackNext", Gtk::Stock::MEDIA_NEXT),
          sigc::mem_fun(*this, &cGtkmmMainWindow::OnPlaybackNextClicked));
  m_refActionGroup->add(Gtk::Action::create("JumpToPlaying", "Jump to Playing"),
          Gtk::AccelKey("<control>J"),
          sigc::mem_fun(*this, &cGtkmmMainWindow::OnActionJumpToPlaying));
  pRepeatAction = Gtk::ToggleAction::create("PlaybackRepeatToggle", Gtk::Stock::GOTO_TOP, "Repeat");
  m_refActionGroup->add(pRepeatAction, sigc::mem_fun(*this, &cGtkmmMainWindow::OnPlaybackRepeatMenuToggled));

  // Help menu
  m_refActionGroup->add( Gtk::Action::create("HelpMenu", "Help") );
  m_refActionGroup->add( Gtk::Action::create("HelpAbout", Gtk::Stock::ABOUT),
          sigc::mem_fun(*this, &cGtkmmMainWindow::OnMenuHelpAbout) );

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
      "      <menuitem action='FileAddFilesFromMusicFolder'/>"
    #ifdef BUILD_MEDUSA_IMPORT_BANSHEE_PLAYLIST
      "      <menuitem action='FileImportFromBanshee'/>"
    #endif
      "      <menuitem action='FileImportFromRhythmbox'/>"
      "      <menuitem action='FileRemove'/>"
      "      <menu action='FileMoveToFolderMenu'>"
      "        <menuitem action='FileMoveToFolderBrowse'/>"
      "      </menu>"
      "      <menuitem action='FileMoveToRubbishBin'/>"
      "      <menuitem action='FileShowInFileManager'/>"
      "      <menuitem action='FileProperties'/>"
      "      <separator/>"
      "      <menuitem action='FileQuit'/>"
      "    </menu>"
      "    <menu action='EditMenu'>"
      "      <menuitem action='EditPreferences'/>"
      "    </menu>"
      "    <menu action='PlaybackMenu'>"
      "      <menuitem action='PlaybackPrevious'/>"
      "      <menuitem action='PlaybackPlayPause'/>"
      "      <menuitem action='PlaybackNext'/>"
      "      <menuitem action='JumpToPlaying'/>"
      "      <separator/>"
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

  boxToolbarAndVolume.pack_start(*Gtk::manage(new Gtk::Separator()), Gtk::PACK_SHRINK);

  buttonPrevious.signal_clicked().connect(sigc::mem_fun(*this, &cGtkmmMainWindow::OnPlaybackPreviousClicked));
  buttonPlayPause.signal_clicked().connect(sigc::mem_fun(*this, &cGtkmmMainWindow::OnPlaybackPlayPauseButtonToggled));
  buttonNext.signal_clicked().connect(sigc::mem_fun(*this, &cGtkmmMainWindow::OnPlaybackNextClicked));

  boxToolbarAndVolume.pack_start(buttonPrevious, Gtk::PACK_SHRINK);
  boxToolbarAndVolume.pack_start(buttonPlayPause, Gtk::PACK_SHRINK);
  boxToolbarAndVolume.pack_start(buttonNext, Gtk::PACK_SHRINK);

  boxToolbarAndVolume.pack_start(*Gtk::manage(new Gtk::Separator()), Gtk::PACK_SHRINK);

  pVolumeSlider = new cGtkmmSlider(*this, true);
  pVolumeSlider->SetRange(0, 100);
  pVolumeSlider->SetValue(100);
  pVolumeSlider->SetValue(settings.GetVolume0To100());

  pVolumeSlider->set_size_request(-1, 100);

  buttonRepeat.signal_clicked().connect(sigc::mem_fun(*this, &cGtkmmMainWindow::OnPlaybackRepeatButtonToggled));
  buttonRepeat.set_active(settings.IsRepeat());

  boxToolbarAndVolume.pack_start(textVolumePlus, Gtk::PACK_SHRINK);
  boxToolbarAndVolume.pack_start(*pVolumeSlider, Gtk::PACK_SHRINK);
  boxToolbarAndVolume.pack_start(textVolumeMinus, Gtk::PACK_SHRINK);

  boxToolbarAndVolume.pack_start(*Gtk::manage(new Gtk::Separator()), Gtk::PACK_SHRINK);

  boxToolbarAndVolume.pack_start(buttonRepeat, Gtk::PACK_SHRINK);


  iconTheme.RegisterThemeChangedListener(*this);

  SetPlaybackButtonIcons();


  // Right click menu
  popupActionGroupRef = Gtk::ActionGroup::create();

  // File|New sub menu:
  // These menu actions would normally already exist for a main menu, because a context menu should
  // not normally contain menu items that are only available via a context menu.
  popupActionGroupRef->add(Gtk::Action::create("ContextMenu", "Context Menu"));

  popupActionGroupRef->add(Gtk::Action::create("ContextAddFiles", "Add Files"),
          sigc::mem_fun(*this, &cGtkmmMainWindow::OnActionBrowseFiles));

  popupActionGroupRef->add(Gtk::Action::create("ContextAddFolder", "Add Folder"),
          Gtk::AccelKey("<control>P"),
          sigc::mem_fun(*this, &cGtkmmMainWindow::OnActionBrowseFolder));

  popupActionGroupRef->add(Gtk::Action::create("ContextRemove", "Remove"),
          sigc::mem_fun(*this, &cGtkmmMainWindow::OnActionRemoveTrack));

  popupActionGroupRef->add(Gtk::Action::create("ContextMoveToFolderMenu", "Move to Folder"));

  popupActionGroupRef->add(Gtk::Action::create("ContextTrackMoveToFolderBrowse", "Browse..."),
          sigc::mem_fun(*this, &cGtkmmMainWindow::OnActionTrackMoveToFolderBrowse));

  popupActionGroupRef->add(Gtk::Action::create("ContextTrackMoveToRubbishBin", "Move to the Rubbish Bin"),
          sigc::mem_fun(*this, &cGtkmmMainWindow::OnActionTrackMoveToRubbishBin));

  popupActionGroupRef->add(Gtk::Action::create("ContextShowInFileManager", "Show in the File Manager"),
          sigc::mem_fun(*this, &cGtkmmMainWindow::OnActionTrackShowInFileManager));

  popupActionGroupRef->add(Gtk::Action::create("ContextProperties", "Properties"),
          sigc::mem_fun(*this, &cGtkmmMainWindow::OnActionTrackProperties));

  popupUIManagerRef = Gtk::UIManager::create();
  popupUIManagerRef->insert_action_group(popupActionGroupRef);

  add_accel_group(popupUIManagerRef->get_accel_group());

  // Layout the actions in our popup menu
  {
    Glib::ustring ui_info =
      "<ui>"
      "  <popup name='PopupMenu'>"
      "    <menuitem action='ContextAddFiles'/>"
      "    <menuitem action='ContextAddFolder'/>"
      "    <menuitem action='ContextRemove'/>"
      "    <menu action='ContextMoveToFolderMenu'>"
      "      <menuitem action='ContextTrackMoveToFolderBrowse'/>"
      "    </menu>"
      "    <menuitem action='ContextTrackMoveToRubbishBin'/>"
      "    <menuitem action='ContextShowInFileManager'/>"
      "    <menuitem action='ContextProperties'/>"
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

  // Get the menu
  pMenuPopup = dynamic_cast<Gtk::Menu*>(popupUIManagerRef->get_widget("/PopupMenu"));
  if (pMenuPopup == nullptr) g_warning("Popup menu not found");
  assert(pMenuPopup != nullptr);

  // Get the move to folder menu
  Gtk::MenuItem* pMenuPopupMoveToFolder = dynamic_cast<Gtk::MenuItem*>(popupUIManagerRef->get_widget("/PopupMenu/ContextMoveToFolderMenu"));
  if (pMenuPopupMoveToFolder == nullptr) g_warning("Move to menu not found");
  assert(pMenuPopupMoveToFolder != nullptr);

  // Get the move to folder sub menu
  pMenuPopupRecentMoveToFolder = pMenuPopupMoveToFolder->get_submenu();
  if (pMenuPopupRecentMoveToFolder == nullptr) g_warning("Recent move to menu not found");
  assert(pMenuPopupRecentMoveToFolder != nullptr);


  // Status icon right click menu
  statusIconPopupActionGroupRef = Gtk::ActionGroup::create();

  // File|New sub menu:
  // These menu actions would normally already exist for a main menu, because a context menu should
  // not normally contain menu items that are only available via a context menu.
  statusIconPopupActionGroupRef->add(Gtk::Action::create("StatusIconMenu", "StatusIcon Menu"));

  statusIconPopupActionGroupRef->add(Gtk::Action::create("StatusIconAddFiles", "Add Files"),
          sigc::mem_fun(*this, &cGtkmmMainWindow::OnActionBrowseFiles));

  statusIconPopupActionGroupRef->add(Gtk::Action::create("StatusIconAddFolder", "Add Folder"),
          Gtk::AccelKey("<control>P"),
          sigc::mem_fun(*this, &cGtkmmMainWindow::OnActionBrowseFolder));

  statusIconPopupActionGroupRef->add(Gtk::Action::create("StatusIconPrevious", "Previous"),
          sigc::mem_fun(*this, &cGtkmmMainWindow::OnActionPlayPreviousTrack));

  statusIconPopupActionGroupRef->add(Gtk::Action::create("StatusIconPlayPause", "Play/Pause"),
          sigc::mem_fun(*this, &cGtkmmMainWindow::OnPlaybackPlayPauseMenuToggled));

  statusIconPopupActionGroupRef->add(Gtk::Action::create("StatusIconNext", "Next"),
          sigc::mem_fun(*this, &cGtkmmMainWindow::OnActionPlayNextTrack));

  statusIconPopupActionGroupRef->add(Gtk::Action::create("StatusIconQuit", "Quit"),
          sigc::mem_fun(*this, &cGtkmmMainWindow::OnMenuFileQuit));

  statusIconPopupUIManagerRef = Gtk::UIManager::create();
  statusIconPopupUIManagerRef->insert_action_group(statusIconPopupActionGroupRef);

  add_accel_group(statusIconPopupUIManagerRef->get_accel_group());

  // Layout the actions in our popup menu
  {
    Glib::ustring ui_info =
      "<ui>"
      "  <popup name='StatusIconPopupMenu'>"
      "    <menuitem action='StatusIconPrevious'/>"
      "    <menuitem action='StatusIconPlayPause'/>"
      "    <menuitem action='StatusIconNext'/>"
      "    <separator/>"
      "    <menuitem action='StatusIconAddFiles'/>"
      "    <menuitem action='StatusIconAddFolder'/>"
      "    <separator/>"
      "    <menuitem action='StatusIconQuit'/>"
      "  </popup>"
      "</ui>";

    try
    {
      statusIconPopupUIManagerRef->add_ui_from_string(ui_info);
    }
    catch(const Glib::Error& ex)
    {
      std::cerr<<"building menus failed: "<<ex.what();
    }
  }

  // Get the menu
  pStatusIconPopupMenu = dynamic_cast<Gtk::Menu*>(statusIconPopupUIManagerRef->get_widget("/StatusIconPopupMenu"));
  if (pStatusIconPopupMenu == nullptr) g_warning("menu not found");



  // Controls

  // Set the currently playing song information
  textCurrentlyPlaying.set_use_markup(true);
  textCurrentlyPlaying.set_markup("");


  pPositionSlider = new cGtkmmSlider(*this, false);
  pPositionSlider->SetRange(0, 0);

  boxPlaybackButtons.pack_start(textCurrentlyPlaying, Gtk::PACK_SHRINK);


  dummyCategories.set_size_request(150, -1);


  pTrackList = new cGtkmmTrackList(*this);


  buttonStopLoading.signal_clicked().connect(sigc::mem_fun(*this, &cGtkmmMainWindow::OnActionStopLoading));


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

  boxStatusBar.pack_start(statusBar, Gtk::PACK_SHRINK);
  boxStatusBar.pack_start(buttonStopLoading, Gtk::PACK_SHRINK);

  boxMainWindow.pack_start(boxControlsAndToolbar, Gtk::PACK_EXPAND_WIDGET);
  boxMainWindow.pack_start(boxStatusBar, Gtk::PACK_SHRINK);

  // Add the box layout to the main window
  add(boxMainWindow);

  show_all_children();

  // Hide the stop button until we start loading some files
  buttonStopLoading.hide();

  ApplySettings();

  // Start the update checker
  updateChecker.Run();
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
const char* sICON_STOP = "gtk-stop";

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

  Gtk::Image* pImageStop = new Gtk::Image;
  iconTheme.LoadStockIconWithSizePixels(sICON_STOP, 16, *pImageStop);
  buttonStopLoading.set_image(*pImageStop);
}

void cGtkmmMainWindow::SetStatusIconText(const string_t& sText)
{
  pStatusIcon->set_tooltip_text(spitfire::string::ToUTF8(sText).c_str());
}

void cGtkmmMainWindow::ShowWindow()
{
  bIsIconified = false;
  deiconify();
  present();
  raise();
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
  assert(pStatusIconPopupMenu != nullptr);
  pStatusIcon->popup_menu_at_position(*pStatusIconPopupMenu, button, activate_time);
}

void cGtkmmMainWindow::OnNotificationClicked(size_t notificationID)
{
  if (notificationID == NOTIFICATION_PLAYBACK) {
    // If the window is closed then we should open it
    if (bIsIconified) ShowWindow();
  } else LOG<<"cGtkmmMainWindow::OnNotificationClicked Unknown notificationID "<<notificationID<<std::endl;
}

void cGtkmmMainWindow::OnNotificationAction(size_t actionID)
{
  LOG<<"cGtkmmMainWindow::OnNotificationAction"<<std::endl;

  switch (actionID) {
    case NOTIFICATION_PREVIOUS: {
      OnActionPlayPreviousTrack();
      break;
    }
    case NOTIFICATION_PAUSE: {
      OnPlaybackPlayPauseButtonToggled();
      break;
    }
    case NOTIFICATION_NEXT: {
      OnActionPlayNextTrack();
      break;
    }
    default: {
      LOG<<"cGtkmmMainWindow::OnNotificationAction Unknown actionID "<<actionID<<std::endl;
      break;
    }
  };
}

  void cGtkmmMainWindow::OnNewVersionFound(int iNewMajorVersion, int iNewMinorVersion, const string_t& sDownloadPage)
  {
    LOG<<"cGtkmmMainWindow::OnNewVersionFound"<<std::endl;

    ASSERT(spitfire::util::IsMainThread());

    // Get our current version
    const string_t sVersion = BUILD_APPLICATION_VERSION_STRING;
    spitfire::string::cStringParser sp(sVersion);
    string_t sMajorVersion;
    ASSERT(sp.GetToStringAndSkip(".", sMajorVersion));
    const string_t sMinorVersion = sp.GetToEnd();
    const int iMajorVersion = spitfire::string::ToUnsignedInt(sMajorVersion);
    const int iMinorVersion = spitfire::string::ToUnsignedInt(sMinorVersion);

    LOG<<"cGtkmmMainWindow::OnNewVersionFound current="<<iMajorVersion<<"."<<iMinorVersion<<", new="<<iNewMajorVersion<<"."<<iNewMinorVersion<<", url="<<sDownloadPage<<std::endl;

    const int iVersion = (10 * iMajorVersion) + iMinorVersion;
    const int iNewVersion = (10 * iNewMajorVersion) + iNewMinorVersion;
    if (iNewVersion > iVersion) {
      const string_t sNewVersion = spitfire::string::ToString(iNewMajorVersion) + TEXT(".") + spitfire::string::ToString(iNewMinorVersion);

      // Check if we should ignore this version
      const string_t sIgnoreVersion = settings.GetIgnoreUpdateVersion();
      const bool bIgnoreThisVersion = (sIgnoreVersion == sNewVersion);

      if (!bIgnoreThisVersion) {
        cGtkmmAlertDialog dialog(*this);
        dialog.SetTitle(TEXT("There is a newer version available, ") + sNewVersion + TEXT("."));
        dialog.SetDescription(TEXT("Would you like to visit the Medusa website?"));
        dialog.SetOk(TEXT("Open Web Page"));
        dialog.SetOther(TEXT("Skip This Version"));
        dialog.SetCancel();
        ALERT_RESULT result = dialog.Run();
        if (result == ALERT_RESULT::OK) {
          spitfire::operatingsystem::OpenURL(sDownloadPage);
        } else if (result == ALERT_RESULT::NO) {
          settings.SetIgnoreUpdateVersion(sNewVersion);
        }
      }
    }
  }

void cGtkmmMainWindow::OnMenuHelpAbout()
{
  std::cout<<"cGtkmmMainWindow::OnMenuHelpAbout"<<std::endl;
  cGtkmmAboutDialog about;
  about.Run(*this);
}

void cGtkmmMainWindow::OnMenuFileQuit()
{
  std::cout<<"cGtkmmMainWindow::OnMenuFileQuit"<<std::endl;

  // Tell the update checker thread to stop soon
  if (updateChecker.IsRunning()) updateChecker.StopThreadSoon();

  // Tell the lastfm thread to stop soon
  if (lastfm.IsRunning()) lastfm.StopSoon();

  view.OnActionMainWindowQuitSoon();

  // Save the window settings
  settings.SetShowMainWindow(!bIsIconified);

  // Save volume settings
  settings.SetVolume0To100(pVolumeSlider->GetValue());

  // Save playback settings
  settings.SetPlaying(view.IsPlaying());

  // Tell the update checker thread to stop now
  if (updateChecker.IsRunning()) updateChecker.StopThreadSoon();

  // Tell the lastfm thread to stop now
  if (lastfm.IsRunning()) lastfm.Stop();

  view.OnActionMainWindowQuitNow();

  // Stop handling popup notifications
  spitfire::operatingsystem::NotificationDestroy();

  //hide(); //Closes the main window to stop the Gtk::Main::run().
  Gtk::Main::quit();
}

void cGtkmmMainWindow::OnMenuEditPreferences()
{
  std::cout<<"cGtkmmMainWindow::OnMenuEditPreferences"<<std::endl;
  cGtkmmPreferencesDialog dialog(settings, *this);
  if (dialog.Run()) {
    // Update our state from the settings
    ApplySettings();
  }
}

void cGtkmmMainWindow::OnFileDroppedFromNautilus(const Glib::RefPtr<Gdk::DragContext>& context, int x, int y, const Gtk::SelectionData& selection_data, guint info, guint time)
{
  std::cout<<"cGtkmmMainWindow::OnFileDroppedFromNautilus "<<selection_data.get_length()<<", "<<selection_data.get_format()<<std::endl;

  bool bIsHandled = false;

  if ((selection_data.get_length() >= 0) && (selection_data.get_format() == 8)) {
    std::cout<<"cGtkmmMainWindow::OnFileDroppedFromNautilus 1"<<std::endl;
    std::vector<Glib::ustring> file_list = selection_data.get_uris();
    if (!file_list.empty()) {
      std::cout<<"cGtkmmMainWindow::OnFileDroppedFromNautilus 2"<<std::endl;
      std::list<string_t> files;

      const size_t n = file_list.size();
      for (size_t i = 0; i < n; i++) {
        std::cout<<"cGtkmmMainWindow::OnFileDroppedFromNautilus 3"<<std::endl;
        const string_t sPath = spitfire::string::ToString_t(Glib::filename_from_uri(file_list[i]));
        std::cout<<"cGtkmmMainWindow::OnFileDroppedFromNautilus \""<<sPath<<"\""<<std::endl;

        if (spitfire::filesystem::IsFolder(sPath)) view.OnActionAddTracksFromFolder(sPath);
        else {
          // Group files for adding all at once later on
          files.push_back(sPath);
        }
      }

      // Now add all the files that were added all at once
      view.OnActionAddTracks(files);

      bIsHandled = true;
    }
  }

  context->drag_finish(bIsHandled, false, time);
}

void cGtkmmMainWindow::OnActionBrowseFiles()
{
  cGtkmmFileDialog dialog;
  dialog.SetType(cGtkmmFileDialog::TYPE::OPEN);
  dialog.SetSelectMultipleFiles(true);
  dialog.SetCaption(TEXT("Add audio files"));
  dialog.SetDefaultFolder(settings.GetLastAddLocation());

  // http://filext.com/file-extension/MP3
  cFilter filterMp3;
  filterMp3.sName = TEXT("MP3 files");
  filterMp3.mimeTypes.push_back("audio/mpeg");
  filterMp3.mimeTypes.push_back("audio/x-mpeg");
  filterMp3.mimeTypes.push_back("audio/mp3");
  filterMp3.mimeTypes.push_back("audio/x-mp3");
  filterMp3.mimeTypes.push_back("audio/mpeg3");
  filterMp3.mimeTypes.push_back("audio/x-mpeg3");
  filterMp3.mimeTypes.push_back("audio/mpg");
  filterMp3.mimeTypes.push_back("audio/x-mpg");
  filterMp3.mimeTypes.push_back("audio/x-mpegaudio");

  // http://filext.com/file-extension/WAV
  cFilter filterWav;
  filterWav.sName = TEXT("Wav files");
  filterWav.mimeTypes.push_back("audio/wav");
  filterWav.mimeTypes.push_back("audio/x-wav");
  filterWav.mimeTypes.push_back("audio/wave");
  filterWav.mimeTypes.push_back("audio/x-pn-wav");

  cFilterList filterList;
  filterList.AddFilter(filterMp3);
  filterList.AddFilter(filterWav);
  filterList.AddFilterAllFiles();

  dialog.SetFilterList(filterList);

  if (dialog.Run(*this)) {
    std::cout<<"cGtkmmMainWindow::OnActionBrowseFiles Selected files"<<std::endl;
    settings.SetLastAddLocation(dialog.GetSelectedFolder());
    settings.Save();
    view.OnActionAddTracks(dialog.GetSelectedFiles());
  }
}

void cGtkmmMainWindow::OnActionBrowseFolder()
{
  cGtkmmFolderDialog dialog;
  dialog.SetType(cGtkmmFolderDialog::TYPE::SELECT);
  dialog.SetCaption(TEXT("Add audio folder"));
  dialog.SetDefaultFolder(settings.GetLastAddLocation());
  if (dialog.Run(*this)) {
    std::cout<<"cGtkmmMainWindow::OnActionBrowseFolder Selected folder"<<std::endl;
    settings.SetLastAddLocation(dialog.GetSelectedFolder());
    settings.Save();
    const string_t sSelectedFolder = dialog.GetSelectedFolder();
    view.OnActionAddTracksFromFolder(sSelectedFolder);
  }
}

  void cGtkmmMainWindow::OnActionAddFilesFromMusicFolder()
  {
    cGtkmmAlertDialog dialog(*this);
    dialog.SetTitle(TEXT("Add all files from your Music folder?"));
    dialog.SetDescription(TEXT("This will add all files from your Music folder."));
    dialog.SetOk(TEXT("Add All Files"));
    dialog.SetCancel();
    ALERT_RESULT result = dialog.Run();
    if (result == ALERT_RESULT::OK) {
      const string_t sMusicFolder = spitfire::filesystem::GetHomeMusicDirectory();
      view.OnActionAddTracksFromFolder(sMusicFolder);
    }
  }

  #ifdef BUILD_MEDUSA_IMPORT_BANSHEE_PLAYLIST
  void cGtkmmMainWindow::OnActionImportFromBanshee()
  {
    cGtkmmAlertDialog dialog(*this);
    dialog.SetTitle(TEXT("Add all files from your Banshee playlist?"));
    dialog.SetDescription(TEXT("This will add all files from your Banshee playlist."));
    dialog.SetOk(TEXT("Add All Files"));
    dialog.SetCancel();
    ALERT_RESULT result = dialog.Run();
    if (result == ALERT_RESULT::OK) view.OnActionImportFromBanshee();
  }
  #endif

  void cGtkmmMainWindow::OnActionImportFromRhythmbox()
  {
    cGtkmmAlertDialog dialog(*this);
    dialog.SetTitle(TEXT("Add all files from your Rhythmbox playlist?"));
    dialog.SetDescription(TEXT("This will add all files from your Rhythmbox playlist."));
    dialog.SetOk(TEXT("Add All Files"));
    dialog.SetCancel();
    ALERT_RESULT result = dialog.Run();
    if (result == ALERT_RESULT::OK) view.OnActionImportFromRhythmbox();
  }

  void cGtkmmMainWindow::OnActionStopLoading()
  {
    view.OnActionStopLoading();
  }

void cGtkmmMainWindow::OnActionRemoveTrack()
{
  std::cout<<"cGtkmmMainWindow::OnActionRemoveTrack A popup menu item was selected"<<std::endl;

  // Collect the tracks to remove
  std::list<trackid_t> tracks;
  cGtkmmTrackListSelectedIterator iter(*pTrackList);
  while (iter.IsValid()) {
    const Gtk::TreeModel::Row& row = iter.GetRow();
    tracks.push_back(pTrackList->GetTrackIDForRow(row));

    iter.Next();
  }

  // If we are removing one track then find out if it is the currently playing track and if we should skip to the next one
  if ((tracks.size() == 1) && (pTrackList->GetTrackCount() != 1)) {
    trackid_t track = *(tracks.begin());
    if (view.GetCurrentTrackID() == track) OnPlaybackNextClicked();
  }

  // Tell the view that we are removing these tracks
  view.OnActionRemoveTracks(tracks);

  // Remove the selected tracks
  pTrackList->DeleteAllSelected();
}

void cGtkmmMainWindow::OnActionTrackMoveToFolder(const string_t& sDestinationFolder)
{
  std::cout<<"cGtkmmMainWindow::OnActionTrackMoveToFolder Destination \""<<sDestinationFolder<<"\""<<std::endl;

  bool bOverwriteAll = false;
  bool bDontOverwriteAll = false;

  // Tell the view that we are removing these tracks
  for (cGtkmmTrackListSelectedIterator iter(*pTrackList); iter.IsValid(); iter.Next()) {
    std::cout<<"cGtkmmMainWindow::OnActionTrackMoveToFolder Item was selected"<<std::endl;
    const Gtk::TreeModel::Row& row = iter.GetRow();
    trackid_t id = pTrackList->GetTrackIDForRow(row);

    // Move the file to the specified folder
    string_t sFilePath;
    spitfire::audio::cMetaData metaData;
    TRACK_STATUS status = TRACK_STATUS::OK;
    pTrackList->GetPropertiesForRow(id, sFilePath, metaData, status);
    if (spitfire::filesystem::FileExists(sFilePath)) { // Only try to move tracks that exist
      // Build the destination path
      const string_t sFileName = spitfire::filesystem::GetFile(sFilePath);
      const string_t sDestinationPath = spitfire::filesystem::MakeFilePath(sDestinationFolder, sFileName);
      if (sDestinationPath != sFilePath) {
        // If we aren't overwriting all files then we need to check if the file already exists
        if (!bOverwriteAll && spitfire::filesystem::FileExists(sDestinationPath)) {
          if (bDontOverwriteAll) continue;

          cGtkmmAlertDialog dialog(*this);
          dialog.SetTitle(TEXT("File already exists. Do you want to replace it?"));
          dialog.SetDescription(TEXT("The file \"") + sDestinationPath + TEXT("\" already exists. Replacing it will overwrite its contents."));
          dialog.SetOk(TEXT("Replace"));
          dialog.SetOther(TEXT("No"));
          dialog.SetCancel();
          dialog.SetCheckBox(TEXT("Replace All"), false);
          ALERT_RESULT result = dialog.Run();
          if (result == ALERT_RESULT::CANCEL) {
            break;
          } else if (result == ALERT_RESULT::NO) {
            bDontOverwriteAll = dialog.IsCheckBoxTicked();
            continue;
          }
          bOverwriteAll = dialog.IsCheckBoxTicked();
        }
        if (spitfire::filesystem::MoveFile(sFilePath, sDestinationPath)) {
          // Update our playlist
          pTrackList->SetPropertiesForRow(row, sFilePath, metaData, status);

          // Notify the model that the file has been moved
          view.OnActionTrackMoveToFolder(id, sDestinationPath);
        }
      }
    }
  }
}

void cGtkmmMainWindow::OnActionTrackMoveToFolderIndex(int i)
{
  std::cout<<"cGtkmmMainWindow::OnActionTrackMoveToFolderIndex "<<i<<std::endl;

  const std::vector<string_t> paths = recentMovedToFolders.GetFullPaths();
  const size_t n = paths.size();
  if ((i >= 0) && (i < int(n))) {
    const string_t sFullPath = paths[static_cast<size_t>(i)];
    std::cout<<"cGtkmmMainWindow::OnActionTrackMoveToFolderIndex Moving to folder \""<<sFullPath<<"\""<<std::endl;
    OnActionTrackMoveToFolder(sFullPath);
  } else std::cerr<<"cGtkmmMainWindow::OnActionTrackMoveToFolderIndex Invalid move to folder "<<i<<std::endl;
}

void cGtkmmMainWindow::OnActionTrackMoveToFolderBrowse()
{
  std::cout<<"cGtkmmMainWindow::OnActionTrackMoveToFolderBrowse A popup menu item was selected"<<std::endl;

  // Browse for the folder to move to
  cGtkmmFolderDialog dialog;
  dialog.SetType(cGtkmmFolderDialog::TYPE::SELECT);
  dialog.SetCaption(TEXT("Move tracks to folder"));
  dialog.SetDefaultFolder(settings.GetLastMoveToFolderLocation());
  if (dialog.Run(*this)) {
    std::cout<<"cGtkmmMainWindow::OnActionBrowseFolder Selected folder"<<std::endl;
    settings.SetLastMoveToFolderLocation(dialog.GetSelectedFolder());
    settings.Save();
    const string_t sSelectedFolder = dialog.GetSelectedFolder();

    // Add this folder to the list of recent move to folders
    recentMovedToFolders.AddPath(sSelectedFolder);

    // Save the recent moved to folder paths to the settings
    const std::vector<string_t> paths = recentMovedToFolders.GetFullPaths();
    settings.SetRecentMoveToFolders(paths);
    settings.Save();

    // Move the selected tracks to the destination folder
    OnActionTrackMoveToFolder(sSelectedFolder);
  }
}

void cGtkmmMainWindow::OnActionTrackMoveToRubbishBin()
{
  std::cout<<"cGtkmmMainWindow::OnActionTrackMoveToRubbishBin A popup menu item was selected"<<std::endl;

  // Ask if the user is sure about this
  cGtkmmAlertDialog dialog(*this);
  dialog.SetTitle(TEXT("Move files to rubbish bin. Are you sure you want to do this?"));
  dialog.SetOk(TEXT("Move to rubbish bin"));
  dialog.SetCancel();
  ALERT_RESULT result = dialog.Run();
  if (result == ALERT_RESULT::OK) {
    // Collect the tracks to remove and move them to the rubbish bin
    std::list<trackid_t> tracks;
    cGtkmmTrackListSelectedIterator iter(*pTrackList);
    while (iter.IsValid()) {
      std::cout<<"cGtkmmMainWindow::OnActionTrackMoveToRubbishBin Item was selected"<<std::endl;
      const Gtk::TreeModel::Row& row = iter.GetRow();
      trackid_t id = pTrackList->GetTrackIDForRow(row);
      tracks.push_back(pTrackList->GetTrackIDForRow(row));

      // Move the file to the rubbish bin
      string_t sFilePath;
      spitfire::audio::cMetaData metaData;
      TRACK_STATUS status = TRACK_STATUS::OK;
      pTrackList->GetPropertiesForRow(id, sFilePath, metaData, status);
      spitfire::filesystem::MoveFileToTrash(sFilePath);

      iter.Next();
    }

    // If we are removing one track then find out if it is the currently playing track and if we should skip to the next one
    if ((tracks.size() == 1) && (pTrackList->GetTrackCount() != 1)) {
      trackid_t track = *(tracks.begin());
      if (view.GetCurrentTrackID() == track) OnPlaybackNextClicked();
    }

    // Tell the view that we are removing these tracks
    view.OnActionRemoveTracks(tracks);

    // Remove the selected tracks
    pTrackList->DeleteAllSelected();
  }
}

void cGtkmmMainWindow::OnActionTrackShowInFileManager()
{
  std::cout<<"cGtkmmMainWindow::OnActionTrackShowInFileManager A popup menu item was selected"<<std::endl;
  cGtkmmTrackListSelectedIterator iter(*pTrackList);
  while (iter.IsValid()) {
    std::cout<<"cGtkmmMainWindow::OnActionTrackShowInFileManager Item was selected"<<std::endl;
    const Gtk::TreeModel::Row& row = iter.GetRow();
    trackid_t id = pTrackList->GetTrackIDForRow(row);

    // Show the file in the file manager
    string_t sFilePath;
    spitfire::audio::cMetaData metaData;
    TRACK_STATUS status = TRACK_STATUS::OK;
    pTrackList->GetPropertiesForRow(id, sFilePath, metaData, status);
    spitfire::filesystem::ShowFile(sFilePath);

    break;
  }
}

void cGtkmmMainWindow::OnActionTrackProperties()
{
  std::cout<<"cGtkmmMainWindow::OnActionTrackProperties A popup menu item was selected"<<std::endl;
  cGtkmmTrackListSelectedIterator iter(*pTrackList);
  while (iter.IsValid()) {
    std::cout<<"cGtkmmMainWindow::OnActionTrackProperties Item was selected"<<std::endl;
    const Gtk::TreeModel::Row& row = iter.GetRow();
    string_t sFilePath;
    spitfire::audio::cMetaData metaData;
    TRACK_STATUS status = TRACK_STATUS::OK;
    pTrackList->GetPropertiesForRow(row, sFilePath, metaData, status);
    std::cout<<"cGtkmmMainWindow::OnActionTrackProperties Properties selected for track "<<metaData.sArtist<<" - "<<metaData.sTitle<<std::endl;

    iter.Next();
  }
}

void cGtkmmMainWindow::OnActionJumpToPlaying()
{
  trackid_t id = view.GetCurrentTrackID();
  if (id != INVALID_TRACK) pTrackList->EnsureRowIsVisible(id);
}

void cGtkmmMainWindow::OnActionPlaylistRightClick(GdkEventButton* event)
{
  assert(pMenuPopupRecentMoveToFolder != nullptr);

  // Update our recent move to folder paths
  recentMovedToFolders.UpdatePaths();

  const std::vector<cFolder>& paths = recentMovedToFolders.GetPaths();
  const size_t n = paths.size();
  if (n != 0) {
    // Add a separator if we haven't already
    if (!bIsRecentMoveToFolderSeparatorAdded) {
      Gtk::SeparatorMenuItem* pMenuItem = Gtk::manage(new Gtk::SeparatorMenuItem());
      pMenuPopupRecentMoveToFolder->insert(*pMenuItem, 0);
      bIsRecentMoveToFolderSeparatorAdded = true;
    }

    for (size_t i = recentMoveToFoldersMenuItems.size(); i < n; i++) {
      Gtk::MenuItem* pMenuItem = Gtk::manage(new Gtk::MenuItem(spitfire::string::ToUTF8(paths[i].sShortPath).c_str()));
      pMenuPopupRecentMoveToFolder->insert(*pMenuItem, i);
      pMenuItem->show_all_children();
      pMenuItem->show_all();
      pMenuItem->signal_activate().connect(sigc::bind<int>(sigc::mem_fun(*this, &cGtkmmMainWindow::OnActionTrackMoveToFolderIndex), int(i)));
      recentMoveToFoldersMenuItems.push_back(pMenuItem);
    }
    for (size_t i = 0; i < n; i++) {
      recentMoveToFoldersMenuItems[i]->set_label(spitfire::string::ToUTF8(paths[i].sShortPath).c_str());
    }
  }

  assert(pMenuPopup != nullptr);
  pMenuPopup->show_all_children();
  pMenuPopup->show_all();
  pMenuPopup->popup(event->button, event->time);
}

void cGtkmmMainWindow::OnActionPlaylistDoubleClick(trackid_t id)
{
  string_t sFilePath;
  spitfire::audio::cMetaData metaData;
  TRACK_STATUS status = TRACK_STATUS::OK;
  if (pTrackList->GetPropertiesForRow(id, sFilePath, metaData, status)) {
    std::cout<<"cGtkmmTrackList::OnActionPlaylistDoubleClick Track: "<<metaData.sArtist<<" - "<<metaData.sTitle<<std::endl;
    OnActionPlayTrack(id, sFilePath, metaData);
  }
}

void cGtkmmMainWindow::OnActionPlaylistSelectionChanged()
{
  UpdateStatusBar();
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

void cGtkmmMainWindow::OnActionPlayTrack(trackid_t id, const string_t& sFilePath, const spitfire::audio::cMetaData& metaData)
{
  view.OnActionPlayTrack(id, sFilePath, metaData);

  lastfm.StartPlayingTrack(metaData);

  // Show a notification message bubble
  spitfire::operatingsystem::cNotification notification(NOTIFICATION_PLAYBACK);

  std::ostringstream oTitle;
  if (!metaData.sArtist.empty()) oTitle<<spitfire::string::ToUTF8(metaData.sArtist);
  if (!metaData.sArtist.empty() && !metaData.sTitle.empty()) oTitle<<" - ";
  if (!metaData.sTitle.empty()) oTitle<<spitfire::string::ToUTF8(metaData.sTitle);

  std::ostringstream oDescription;
  if (metaData.uiTracknum != 0) oDescription<<"Track "<<metaData.uiTracknum;
  if (!metaData.sAlbum.empty()) {
    if (metaData.uiTracknum != 0) oDescription<<" on ";
    else oDescription<<"On ";
    oDescription<<spitfire::string::ToUTF8(metaData.sAlbum);
  }

  notification.SetTitle(oTitle.str());
  notification.SetDescription(oDescription.str());
  notification.SetActionsMusicPlayer(NOTIFICATION_PREVIOUS, NOTIFICATION_PAUSE, NOTIFICATION_NEXT);
  spitfire::operatingsystem::NotificationShow(notification, 8000);
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
    const bool bIsRepeat = !buttonRepeat.get_active();
    buttonRepeat.set_active(bIsRepeat);
    bIsTogglingRepeat = false;

    // Perform the action
    //view.OnActionRepeatToggle();

    // Update the settings
    settings.SetRepeat(bIsRepeat);
    settings.Save();
  }
}

void cGtkmmMainWindow::OnPlaybackRepeatButtonToggled()
{
  if (!bIsTogglingRepeat) {
    // Toggle the repeat button
    bIsTogglingRepeat = true;
    const bool bIsRepeat = !pRepeatAction->get_active();
    pRepeatAction->set_active(bIsRepeat);
    bIsTogglingRepeat = false;

    // Perform the action
    //view.OnActionRepeatToggle();

    // Update the settings
    settings.SetRepeat(bIsRepeat);
    settings.Save();
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

trackid_t cGtkmmMainWindow::GetPreviousTrack() const
{
  trackid_t id = view.GetCurrentTrackID();
  if (id == INVALID_TRACK) {
    // There was no current track so just pick the last one
    trackid_t idPrevious = INVALID_TRACK;
    cGtkmmTrackListIterator iter(*pTrackList);
    while (iter.IsValid()) {
      const Gtk::TreeModel::Row& row = iter.GetRow();
      idPrevious = pTrackList->GetTrackIDForRow(row);

      iter.Next();
    }

    return idPrevious;
  } else {
    trackid_t idPrevious = INVALID_TRACK;
    cGtkmmTrackListIterator iter(*pTrackList);
    while (iter.IsValid()) {
      const Gtk::TreeModel::Row& row = iter.GetRow();
      trackid_t idThisTrack = pTrackList->GetTrackIDForRow(row);
      if (idThisTrack == id) {
        // Found our current track so return the previous one
        return idPrevious;
      }

      idPrevious = idThisTrack;

      iter.Next();
    }
  }

  return nullptr;
}

trackid_t cGtkmmMainWindow::GetNextTrack() const
{
  trackid_t id = view.GetCurrentTrackID();
  if (id == INVALID_TRACK) {
    // There was no current track so just pick the first one
    cGtkmmTrackListIterator iter(*pTrackList);
    if (iter.IsValid()) {
      const Gtk::TreeModel::Row& row = iter.GetRow();
      return pTrackList->GetTrackIDForRow(row);
    }
  } else {
    cGtkmmTrackListIterator iter(*pTrackList);
    while (iter.IsValid()) {
      const Gtk::TreeModel::Row& row = iter.GetRow();
      trackid_t idNext = pTrackList->GetTrackIDForRow(row);
      if (idNext == id) {
        // Found our current track so return the next one
        iter.Next();

        if (iter.IsValid()) {
          const Gtk::TreeModel::Row& row = iter.GetRow();
          return pTrackList->GetTrackIDForRow(row);
        }

        break;
      }

      iter.Next();
    }
  }

  if (settings.IsRepeat()) {
    // We are at the end of the playlist so go back to the start of the playlist
    cGtkmmTrackListIterator iter(*pTrackList);
    if (iter.IsValid()) {
      const Gtk::TreeModel::Row& row = iter.GetRow();
      return pTrackList->GetTrackIDForRow(row);
    }
  }

  return nullptr;
}

void cGtkmmMainWindow::OnActionPlayPreviousTrack()
{
  trackid_t id = GetPreviousTrack();
  if (id != nullptr) {
    string_t sFilePath;
    spitfire::audio::cMetaData metaData;
    TRACK_STATUS status = TRACK_STATUS::OK;
    pTrackList->GetPropertiesForRow(id, sFilePath, metaData, status);
    OnActionPlayTrack(id, sFilePath, metaData);
  }
}

void cGtkmmMainWindow::OnActionPlayNextTrack()
{
  trackid_t id = GetNextTrack();
  if (id != nullptr) {
    string_t sFilePath;
    spitfire::audio::cMetaData metaData;
    TRACK_STATUS status = TRACK_STATUS::OK;
    pTrackList->GetPropertiesForRow(id, sFilePath, metaData, status);
    OnActionPlayTrack(id, sFilePath, metaData);
  }
}

void cGtkmmMainWindow::SetPlaybackPositionMS(uint64_t milliseconds)
{
  //std::cout<<"cGtkmmMainWindow::SetPlaybackPositionMS "<<milliseconds<<"\n";
  pPositionSlider->SetValue(double(milliseconds) / 1000.0f);

  textPosition.set_text(spitfire::string::ToUTF8(util::FormatTime(milliseconds)).c_str());
}

void cGtkmmMainWindow::SetPlaybackLengthMS(uint64_t milliseconds)
{
  std::cout<<"cGtkmmMainWindow::SetPlaybackLengthMS "<<milliseconds<<"\n";
  pPositionSlider->SetRange(0, double(milliseconds) / 1000.0f);

  textLength.set_text(spitfire::string::ToUTF8(util::FormatTime(milliseconds)).c_str());
}

void cGtkmmMainWindow::SetStatePlaying(trackid_t id)
{
  string_t sFilePath;
  spitfire::audio::cMetaData metaData;
  TRACK_STATUS status = TRACK_STATUS::OK;
  pTrackList->GetPropertiesForRow(id, sFilePath, metaData, status);

  std::ostringstream o;
  if (!metaData.sArtist.empty()) o<<"<b><big>"<<spitfire::string::HTMLEncode(spitfire::string::ToUTF8(metaData.sArtist))<<"</big></b>";
  if (!metaData.sArtist.empty() && !metaData.sTitle.empty()) o<<" - ";
  if (!metaData.sTitle.empty()) o<<"<b><big>"<<spitfire::string::HTMLEncode(spitfire::string::ToUTF8(metaData.sTitle))<<"</big></b>";
  if ((metaData.uiTracknum != 0) && !metaData.sAlbum.empty()) o<<",";
  if (metaData.uiTracknum != 0) o<<" track "<<metaData.uiTracknum;
  if (!metaData.sAlbum.empty()) o<<" on "<<spitfire::string::HTMLEncode(spitfire::string::ToUTF8(metaData.sAlbum));

  textCurrentlyPlaying.set_markup(o.str().c_str());

  // Update position slider
  SetPlaybackLengthMS(metaData.uiDurationMilliSeconds);

  pTrackList->SetStatePlaying(id);

  bIsTogglingPlayPause = true;
  buttonPlayPause.set_active(true);
  pPlayPauseAction->set_active(true);
  bIsTogglingPlayPause = false;

  if (lastfm.IsRunning()) lastfm.StartPlayingTrack(metaData);
}

void cGtkmmMainWindow::SetStatePaused()
{
  pTrackList->SetStatePaused(view.GetCurrentTrackID());

  bIsTogglingPlayPause = true;
  buttonPlayPause.set_active(false);
  pPlayPauseAction->set_active(false);
  bIsTogglingPlayPause = false;

  if (lastfm.IsRunning()) lastfm.StopPlayingTrack();
}

void cGtkmmMainWindow::ApplySettings()
{
  buttonRepeat.set_active(settings.IsRepeat());

  // Stop Last.fm controller
  if (lastfm.IsRunning()) {
    std::cout<<"cGtkmmMainWindow::ApplySettings Stopping lastfm"<<std::endl;
    lastfm.Stop();
  }

  // Start Last.fm controller
  if (settings.IsLastFMEnabled()) {
    std::cout<<"cGtkmmMainWindow::ApplySettings Starting lastfm"<<std::endl;
    lastfm.Start(discombobulator::GetSecretLastfmKeyUTF8(), discombobulator::GetSecretLastfmSecretUTF8(), settings.GetLastFMUserName(), settings.GetLastFMPassword());
  }

  // Load the recent moved folders settings
  std::vector<string_t> paths;
  settings.GetRecentMoveToFolders(paths);
  const size_t n = paths.size();
  for (size_t i = 0; i < n; i++) recentMovedToFolders.AddPath(paths[i]);

  std::cout<<"cGtkmmMainWindow::ApplySettings returning"<<std::endl;
}

  void cGtkmmMainWindow::UpdateStatusBar()
  {
    std::ostringstream o;
    const size_t nSelectedCount = pTrackList->GetSelectedTrackCount();
    if (nSelectedCount != 0) {
      o<<nSelectedCount;
      o<<" selected of ";
    }
    o<<pTrackList->GetTrackCount();
    o<<" tracks";
    if (nTracksLoading != 0) {
      o<<", loading ";
      o<<nTracksLoading;
      o<<" files";
    }
    statusBar.set_text(o.str());

    // Show the stop button if we are currently loading tracks
    if (nTracksLoading != 0) buttonStopLoading.show();
    else buttonStopLoading.hide();
  }

void cGtkmmMainWindow::OnTracksAdded(const std::list<trackid_t>& ids, const std::list<cTrack*>& tracks)
{
  std::cout<<"cGtkmmMainWindow::OnTracksAdded"<<std::endl;
  ASSERT(ids.size() == tracks.size());
  size_t i = 0;
  std::list<trackid_t>::const_iterator iterID = ids.begin();
  const std::list<trackid_t>::const_iterator iterIDEnd = ids.end();
  std::list<cTrack*>::const_iterator iterTrack = tracks.begin();
  const std::list<cTrack*>::const_iterator iterTrackEnd = tracks.end();
  while ((iterID != iterIDEnd) && (iterTrack != iterTrackEnd)) {
    pTrackList->AddTrack(*iterID, *(*iterTrack));

    i++;

    iterID++;
    iterTrack++;
  }

  ASSERT(i == ids.size());
  ASSERT(i == tracks.size());

  // Now that the tracks have been added we can decrement our loading counter
  OnLoadingFilesToLoadDecrement(ids.size());
}

  void cGtkmmMainWindow::OnLoadingFilesToLoadIncrement(size_t nFiles)
  {
    std::cout<<"cGtkmmMainWindow::OnLoadingFilesToLoadIncrement "<<nTracksLoading<<" adding "<<nFiles<<std::endl;
    nTracksLoading += nFiles;
    UpdateStatusBar();
  }

  void cGtkmmMainWindow::OnLoadingFilesToLoadDecrement(size_t nFiles)
  {
    // Check that there are enough files to subtract
    std::cout<<"cGtkmmMainWindow::OnLoadingFilesToLoadDecrement "<<nTracksLoading<<" subtracting "<<nFiles<<std::endl;
    ASSERT(nTracksLoading + nFiles >= nFiles);
    nTracksLoading -= nFiles;
    UpdateStatusBar();
  }

  void cGtkmmMainWindow::OnPlaylistLoading()
  {
    std::cout<<"cGtkmmMainWindow::OnPlaylistLoaded"<<std::endl;
    if (!settings.IsShowMainWindow()) HideWindow();

    const unsigned int uiVolume0To100 = settings.GetVolume0To100();
    pVolumeSlider->SetValue(uiVolume0To100);
    view.OnActionVolumeChanged(uiVolume0To100);

    // TODO: disable controls
  }

  void cGtkmmMainWindow::OnPlaylistLoaded(trackid_t idLastPlayed)
  {
    // TODO: enable controls

    // Get the index of last played file to settings
    if (idLastPlayed != INVALID_TRACK) {
      pTrackList->EnsureRowIsVisible(idLastPlayed);

      // Start playing the track if we were playing when we quit last
      if (settings.IsPlaying()) {
        string_t sFilePath;
        spitfire::audio::cMetaData metaData;
        TRACK_STATUS status = TRACK_STATUS::OK;
        if (pTrackList->GetPropertiesForRow(idLastPlayed, sFilePath, metaData, status)) {
          std::cout<<"cGtkmmTrackList::OnPlaylistLoaded Track: "<<metaData.sArtist<<" - "<<metaData.sTitle<<std::endl;
          OnActionPlayTrack(idLastPlayed, sFilePath, metaData);
        }
      }
    }
  }

  void cGtkmmMainWindow::OnWebServerPreviousTrack()
  {
    OnPlaybackPreviousClicked();
  }

  void cGtkmmMainWindow::OnWebServerPlayPause()
  {
    OnPlaybackPlayPauseButtonToggled();
  }

  void cGtkmmMainWindow::OnWebServerNextTrack()
  {
    OnPlaybackNextClicked();
  }

  void cGtkmmMainWindow::OnWebServerSetVolumeMute()
  {
    OnActionVolumeValueChanged(0);
  }

  void cGtkmmMainWindow::OnWebServerSetVolumeFull()
  {
    OnActionVolumeValueChanged(100);
  }

  void cGtkmmMainWindow::OnWebServerTrackMoveToRubbishBin(trackid_t id)
  {
    std::cout<<"cGtkmmMainWindow::OnWebServerTrackMoveToRubbishBin"<<std::endl;

    // Get the data for the track
    string_t sFilePath;
    spitfire::audio::cMetaData metaData;
    TRACK_STATUS status = TRACK_STATUS::OK;
    if (!pTrackList->GetPropertiesForRow(id, sFilePath, metaData, status)) {
      std::cout<<"cGtkmmMainWindow::OnWebServerTrackMoveToRubbishBin Track has already been removed, returning"<<std::endl;
      return;
    }

    // Move the file to the rubbish bin
    spitfire::filesystem::MoveFileToTrash(sFilePath);

    // We are removing one track, so find out if it is the currently playing track and if we should skip to the next one
    if (pTrackList->GetTrackCount() != 1) {
      if (view.GetCurrentTrackID() == id) OnPlaybackNextClicked();
    }

    // Tell the view that we are removing these tracks
    std::list<trackid_t> tracks;
    tracks.push_back(id);
    view.OnActionRemoveTracks(tracks);

    // Remove the selected tracks
    pTrackList->DeleteTrack(id);
  }
}
