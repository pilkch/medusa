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

  void OnWindowClose();

  void on_menu_file_new_generic();
  void on_menu_file_quit();
  void on_menu_others();

  void on_menu_file_popup_generic();

  void OnPlaybackPreviousClicked();
  void OnPlaybackPlayPauseClicked();
  void OnPlaybackNextClicked();

  cGtkmmView& view;
  cSettings& settings;

  cGtkmmIconTheme iconTheme;

  // Menu and toolbar
  Glib::RefPtr<Gtk::UIManager> m_refUIManager;
  Glib::RefPtr<Gtk::ActionGroup> m_refActionGroup;

  // Popup menu
  Glib::RefPtr<Gtk::UIManager> popupUIManagerRef;
  Glib::RefPtr<Gtk::ActionGroup> popupActionGroupRef;
  Gtk::Menu* pMenuPopup;

  // Layouts
  Gtk::VBox boxMainWindow;
  Gtk::VBox boxToolbarAndVolume;
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

  Gtk::Button buttonPrevious;
  Gtk::Button buttonPlay;
  Gtk::Button buttonNext;
  Gtk::Label textVolumePlus;
  cGtkmmSlider* pVolumeSlider;
  Gtk::Label textVolumeMinus;

  Gtk::Button dummyCategories;

  Gtk::Label dummyStatusBar;

  cGtkmmTrackList* pTrackList;
};

#endif // gtkmmmainwindow_h
