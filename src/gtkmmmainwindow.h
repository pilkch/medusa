#ifndef gtkmmmainwindow_h
#define gtkmmmainwindow_h

// gtkmm headers
#include <gtkmm.h>

// Medusa headers
#include "settings.h"
#include "track.h"

// http://git.gnome.org/browse/rhythmbox/tree/widgets/rb-header.c
// HACK: we want the behaviour you get with the middle button, so we
// mangle the event.  clicking with other buttons moves the slider in
// step increments, clicking with the middle button moves the slider to
// the location of the click.
#define BUILD_SUPPORT_SLIDER_DONT_JUMP_HACK

class cGtkmmView;
class cGtkmmTrackList;
class cGtkmmHorizontalSlider;

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

  void OnActionSliderValueChanged(const cGtkmmHorizontalSlider& slider, uint64_t uiValue);

private:
  const cTrack* GetPreviousTrack() const;
  const cTrack* GetNextTrack() const;

  std::string TimeToString(uint64_t milliseconds) const;

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

  // Menu and toolbar
  Glib::RefPtr<Gtk::UIManager> m_refUIManager;
  Glib::RefPtr<Gtk::ActionGroup> m_refActionGroup;

  // Popup menu
  Glib::RefPtr<Gtk::UIManager> popupUIManagerRef;
  Glib::RefPtr<Gtk::ActionGroup> popupActionGroupRef;
  Gtk::Menu* pMenuPopup;

  // Layouts
  Gtk::VBox boxMainWindow;
  Gtk::VBox boxToolbar;
  Gtk::HBox boxPlaybackButtons;
  Gtk::HBox boxPositionSlider;
  Gtk::HBox boxCategoriesAndPlaylist;
  Gtk::HBox boxControlsAndToolbar;
  Gtk::VBox boxControls;

  // Controls
  Gtk::Label textCurrentlyPlaying;
  Gtk::Label textVolumeMinus;
  cGtkmmHorizontalSlider* pVolumeSlider;
  Gtk::Label textVolumePlus;
  Gtk::Label textPosition;
  cGtkmmHorizontalSlider* pPositionSlider;
  Gtk::Label textLength;

  Gtk::Button dummyCategories;

  Gtk::Label dummyStatusBar;

  cGtkmmTrackList* pTrackList;
};

#endif // gtkmmmainwindow_h
