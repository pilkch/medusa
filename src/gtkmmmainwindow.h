#ifndef gtkmmmainwindow_h
#define gtkmmmainwindow_h

// Standard headers
#include <set>

// gtkmm headers
#include <gtkmm.h>

// libgtkmm headers
#include <libgtkmm/icontheme.h>
#include <libgtkmm/slider.h>

// Spitfire headers
#include <spitfire/audio/lastfm.h>
#include <spitfire/platform/notifications.h>
#include <spitfire/util/updatechecker.h>

// Medusa headers
#include "settings.h"
#include "track.h"

namespace medusa
{
  class cFolder
  {
  public:
    string_t sFullPath;
    string_t sShortPath;
  };

  class cFolderList
  {
  public:
    void AddPath(const string_t& sFullPath);
    void UpdatePaths();

    const std::vector<cFolder>& GetPaths() const;
    std::vector<string_t> GetFullPaths() const;

  private:
    bool AreFullPathsUnique() const;
    bool AreShortPathsUnique() const;
    std::set<size_t> GetNonUniqueShortPaths() const;

    std::vector<cFolder> folders;
  };

  class cGtkmmView;
  class cGtkmmTrackList;

class cGtkmmMainWindow : public Gtk::Window, public spitfire::operatingsystem::cNotificationHandler, public gtkmm::cGtkmmSliderHandler {
public:
  cGtkmmMainWindow(cGtkmmView& view, cSettings& settings);
  virtual ~cGtkmmMainWindow() {}

  void SetStatePlaying(trackid_t id);
  void SetStatePaused();
  void SetPlaybackPositionMS(uint64_t milliseconds);
  void SetPlaybackLengthMS(uint64_t milliseconds);

  void OnFileDroppedFromNautilus(const Glib::RefPtr<Gdk::DragContext>& context, int x, int y, const Gtk::SelectionData& selection_data, guint info, guint time);

  void OnActionBrowseFiles();
  void OnActionBrowseFolder();
  void OnActionAddFilesFromMusicFolder();
  #ifdef BUILD_MEDUSA_IMPORT_BANSHEE_PLAYLIST
  void OnActionImportFromBanshee();
  #endif
  void OnActionImportFromRhythmbox();
  void OnActionStopLoading();
  void OnActionRemoveTrack();
  void OnActionTrackMoveToFolder(const string_t& sDestinationFolder);
  void OnActionTrackMoveToFolderIndex(int i);
  void OnActionTrackMoveToFolderBrowse();
  void OnActionTrackMoveToRubbishBin();
  void OnActionTrackShowInFileManager();
  void OnActionTrackProperties();
  void OnActionPlayTrack(trackid_t id, const string_t& sFilePath, const spitfire::audio::cMetaData& metaData);
  void OnActionPlayPreviousTrack();
  void OnActionPlayNextTrack();
  void OnActionJumpToPlaying();
  void OnActionPlaylistRightClick(GdkEventButton* event);
  void OnActionPlaylistDoubleClick(trackid_t id);
  void OnActionPlaylistSelectionChanged();
  void OnActionPlaybackPositionValueChanged(uint64_t uiValue);
  void OnActionVolumeValueChanged(unsigned int uiVolume0To100);

  virtual void OnActionSliderValueChanged(const gtkmm::cGtkmmSlider& slider, uint64_t uiValue) override;

  void OnThemeChanged();

  void OnLoadingFilesToLoadIncrement(size_t nFiles);
  void OnLoadingFilesToLoadDecrement(size_t nFiles);
  void OnPlaylistLoading();
  void OnPlaylistLoaded(trackid_t idLastPlayed);
  void OnTracksAdded(const std::list<trackid_t>& ids, const std::list<cTrack*>& tracks);

  void OnWebServerPreviousTrack();
  void OnWebServerPlayPause();
  void OnWebServerPlayTrack(trackid_t id);
  void OnWebServerNextTrack();
  void OnWebServerSetVolumeMute();
  void OnWebServerSetVolumeFull();
  void OnWebServerTrackMoveToRubbishBin(trackid_t id);

  // For handling lastfm errors
  void OnLastFMErrorUserNameOrPasswordIncorrect();

  // For handling new version notifications
  void OnNewVersionFound(int iMajorVersion, int iMinorVersion, const string_t& sDownloadPage);

private:
  trackid_t GetPreviousTrack() const;
  trackid_t GetNextTrack() const;

  std::string TimeToString(uint64_t milliseconds) const;

  void UpdateStatusBar();

  void SetPlaybackButtonIcons();

  void ShowWindow();
  void HideWindow();

  void SetStatusIconText(const string_t& sText);

  void ApplySettings();

  void TrackMoveToRubbishBin(trackid_t id);

  // Overridden
  bool on_delete_event(GdkEventAny* event);

  void OnStatusIconActivate();
  void OnStatusIconPopupMenu(guint button, guint32 activate_time);

  void OnMenuFileRemoveMissingAndDuplicates();
  void OnMenuFileQuit();
  void OnMenuEditPreferences();
  void OnMenuHelpAbout();

  void OnPlaybackPreviousClicked();
  void OnPlaybackNextClicked();

  void OnPlaybackRepeatMenuToggled();
  void OnPlaybackPlayPauseMenuToggled();
  void OnPlaybackRepeatButtonToggled();
  void OnPlaybackPlayPauseButtonToggled();

  void OnInfoBarResponse(int response);

  // For handling popup notifications
  virtual void OnNotificationClicked(size_t notificationID) override;
  virtual void OnNotificationAction(size_t actionID) override;

  cGtkmmView& view;
  cSettings& settings;

  spitfire::util::cUpdateChecker updateChecker;

  spitfire::audio::lastfm::cLastFM lastfm;

  spitfire::operatingsystem::cNotification notification;

  cFolderList recentMovedToFolders;

  gtkmm::cIconTheme iconTheme;

  Gtk::InfoBar infoBar;
  Gtk::Label infoBarMessageLabel;

  // Status icon
  bool bIsIconified;
  Glib::RefPtr<Gtk::StatusIcon> pStatusIcon;

  // Menu and toolbar
  Glib::RefPtr<Gtk::UIManager> m_refUIManager;
  Glib::RefPtr<Gtk::ActionGroup> m_refActionGroup;

  // Playlist right click menu
  Glib::RefPtr<Gtk::UIManager> popupUIManagerRef;
  Glib::RefPtr<Gtk::ActionGroup> popupActionGroupRef;
  Gtk::Menu* pMenuPopup;
  Gtk::Menu* pMenuPopupRecentMoveToFolder;

  std::vector<Gtk::MenuItem*> recentMoveToFoldersMenuItems;
  bool bIsRecentMoveToFolderSeparatorAdded;

  // Status icon right click menu
  Glib::RefPtr<Gtk::UIManager> statusIconPopupUIManagerRef;
  Glib::RefPtr<Gtk::ActionGroup> statusIconPopupActionGroupRef;
  Gtk::Menu* pStatusIconPopupMenu;

  // Layouts
  Gtk::VBox boxMainWindow;
  Gtk::Box boxToolbarAndVolume;
  Gtk::HBox boxPlaybackButtons;
  Gtk::HBox boxPositionSlider;
  Gtk::HBox boxCategoriesAndPlaylist;
  Gtk::HBox boxControlsAndToolbar;
  Gtk::VBox boxControls;
  Gtk::HBox boxStatusBar;

  // Controls
  Gtk::Label textCurrentlyPlaying;
  Gtk::Label textPosition;
  gtkmm::cGtkmmSlider* pPositionSlider;
  Gtk::Label textLength;

  Gtk::Button buttonAddFiles;
  Gtk::Button buttonAddFolder;
  Gtk::Button buttonPrevious;
  Gtk::ToggleButton buttonPlayPause;
  Gtk::Button buttonNext;
  Gtk::Label textVolumePlus;
  gtkmm::cGtkmmSlider* pVolumeSlider;
  Gtk::Label textVolumeMinus;
  Gtk::ToggleButton buttonRepeat;

  Gtk::Button dummyCategories;

  Gtk::Label statusBar;
  Gtk::Button buttonStopLoading;

  size_t nTracksLoading;
  cGtkmmTrackList* pTrackList;

  Glib::RefPtr<Gtk::ToggleAction> pPlayPauseAction;
  Glib::RefPtr<Gtk::ToggleAction> pRepeatAction;
  bool bIsTogglingPlayPause;
  bool bIsTogglingRepeat;
};
}

#endif // gtkmmmainwindow_h
