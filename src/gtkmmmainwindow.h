#ifndef gtkmmmainwindow_h
#define gtkmmmainwindow_h

// gtkmm headers
#include <gtkmm.h>

// Medusa headers
#include "settings.h"
#include "track.h"
#include "gtkmmicontheme.h"

class cGtkmmView;
class cGtkmmSlider;
class cGtkmmTrackList;

class cGtkmmMainWindow : public Gtk::Window {
public:
  cGtkmmMainWindow(cGtkmmView& view, cSettings& settings);
  virtual ~cGtkmmMainWindow() {}

  void SetStatePlaying(const cTrack* pTrack);
  void SetStatePaused();
  void SetPlaybackPositionMS(uint64_t milliseconds);
  void SetPlaybackLengthMS(uint64_t milliseconds);

  void OnActionBrowseFiles();
  void OnActionBrowseFolder();
  void OnActionPlayTrack(const cTrack* pTrack);
  void OnActionPlayPreviousTrack();
  void OnActionPlayNextTrack();
  void OnActionPlaylistRightClick(GdkEventButton* event);
  void OnActionPlaybackPositionValueChanged(uint64_t uiValue);
  void OnActionVolumeValueChanged(unsigned int uiVolume0To100);

  void OnActionSliderValueChanged(const cGtkmmSlider& slider, uint64_t uiValue);

  void OnThemeChanged();

private:
  const cTrack* GetPreviousTrack() const;
  const cTrack* GetNextTrack() const;

  std::string TimeToString(uint64_t milliseconds) const;

  void SetPlaybackButtonIcons();

  void ShowWindow();
  void HideWindow();

  void SetStatusIconText(const spitfire::string_t& sText);

  // Overridden
  bool on_delete_event(GdkEventAny* event);

  void OnStatusIconActivate();
  void OnStatusIconPopupMenu(guint button, guint32 activate_time);

  void on_menu_file_new_generic();
  void on_menu_file_quit();
  void on_menu_others();

  void on_menu_file_popup_generic();

  void OnPlaybackPreviousClicked();
  void OnPlaybackNextClicked();

  void OnPlaybackRepeatMenuToggled();
  void OnPlaybackPlayPauseMenuToggled();
  void OnPlaybackRepeatButtonToggled();
  void OnPlaybackPlayPauseButtonToggled();

  cGtkmmView& view;
  cSettings& settings;

  cGtkmmIconTheme iconTheme;

  // Status icon
  bool bIsIconified;
  Glib::RefPtr<Gtk::StatusIcon> pStatusIcon;

  // Menu and toolbar
  Glib::RefPtr<Gtk::UIManager> m_refUIManager;
  Glib::RefPtr<Gtk::ActionGroup> m_refActionGroup;

  // Popup menu
  Glib::RefPtr<Gtk::UIManager> popupUIManagerRef;
  Glib::RefPtr<Gtk::ActionGroup> popupActionGroupRef;
  Gtk::Menu* pMenuPopup;

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

#endif // gtkmmmainwindow_h
