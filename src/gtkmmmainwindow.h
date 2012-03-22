#ifndef gtkmmmainwindow_h
#define gtkmmmainwindow_h

// gtkmm headers
#include <gtkmm.h>

// Spitfire headers
#include <spitfire/audio/lastfm.h>

// Medusa headers
#include "settings.h"
#include "track.h"
#include "gtkmmicontheme.h"

namespace medusa
{
  class cGtkmmView;
  class cGtkmmSlider;
  class cGtkmmTrackList;

class cGtkmmMainWindow : public Gtk::Window {
public:
  cGtkmmMainWindow(cGtkmmView& view, cSettings& settings);
  virtual ~cGtkmmMainWindow() {}

  void SetStatePlaying(trackid_t id);
  void SetStatePaused();
  void SetPlaybackPositionMS(uint64_t milliseconds);
  void SetPlaybackLengthMS(uint64_t milliseconds);

  void OnActionBrowseFiles();
  void OnActionBrowseFolder();
  void OnActionRemoveTrack();
  void OnActionTrackProperties();
  void OnActionPlayTrack(trackid_t id, const string_t& sFilePath, const spitfire::audio::cMetaData& metaData);
  void OnActionPlayPreviousTrack();
  void OnActionPlayNextTrack();
  void OnActionJumpToPlaying();
  void OnActionPlaylistRightClick(GdkEventButton* event);
  void OnActionPlaylistDoubleClick(trackid_t id);
  void OnActionPlaybackPositionValueChanged(uint64_t uiValue);
  void OnActionVolumeValueChanged(unsigned int uiVolume0To100);

  void OnActionSliderValueChanged(const cGtkmmSlider& slider, uint64_t uiValue);

  void OnThemeChanged();

  void OnTrackAdded(trackid_t id, const string_t& sFilePath, const spitfire::audio::cMetaData& metaData);
  void OnPlaylistLoaded();

private:
  trackid_t GetPreviousTrack() const;
  trackid_t GetNextTrack() const;

  std::string TimeToString(uint64_t milliseconds) const;

  void SetPlaybackButtonIcons();

  void ShowWindow();
  void HideWindow();

  void SetStatusIconText(const string_t& sText);

  void ApplySettings();

  // Overridden
  bool on_delete_event(GdkEventAny* event);

  void OnStatusIconActivate();
  void OnStatusIconPopupMenu(guint button, guint32 activate_time);

  void OnMenuFileQuit();
  void OnMenuEditPreferences();
  void OnMenuHelpAbout();

  void OnPlaybackPreviousClicked();
  void OnPlaybackNextClicked();

  void OnPlaybackRepeatMenuToggled();
  void OnPlaybackPlayPauseMenuToggled();
  void OnPlaybackRepeatButtonToggled();
  void OnPlaybackPlayPauseButtonToggled();

  cGtkmmView& view;
  cSettings& settings;

  spitfire::audio::lastfm::cLastFM lastfm;

  cGtkmmIconTheme iconTheme;

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

  // Controls
  Gtk::Label textCurrentlyPlaying;
  Gtk::Label textPosition;
  cGtkmmSlider* pPositionSlider;
  Gtk::Label textLength;

  Gtk::Button buttonAddFiles;
  Gtk::Button buttonAddFolder;
  Gtk::Button buttonPrevious;
  Gtk::ToggleButton buttonPlayPause;
  Gtk::Button buttonNext;
  Gtk::Label textVolumePlus;
  cGtkmmSlider* pVolumeSlider;
  Gtk::Label textVolumeMinus;
  Gtk::ToggleButton buttonRepeat;

  Gtk::Button dummyCategories;

  Gtk::Label dummyStatusBar;

  cGtkmmTrackList* pTrackList;

  Glib::RefPtr<Gtk::ToggleAction> pPlayPauseAction;
  Glib::RefPtr<Gtk::ToggleAction> pRepeatAction;
  bool bIsTogglingPlayPause;
  bool bIsTogglingRepeat;
};
}

#endif // gtkmmmainwindow_h
