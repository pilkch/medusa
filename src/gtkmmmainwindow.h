#ifndef gtkmmmainwindow_h
#define gtkmmmainwindow_h

// gtkmm headers
#include <gtkmm.h>

// Medusa headers
#include "track.h"

class cGtkmmView;
class cGtkmmTrackList;

class cGtkmmMainWindow : public Gtk::Window {
public:
  explicit cGtkmmMainWindow(cGtkmmView& view);
  virtual ~cGtkmmMainWindow() {}

  void SetStatePlaying(const cTrack* pTrack);
  void SetStatePaused();
  void SetPlaybackPositionMS(uint64_t milliseconds);
  void SetPlaybackLengthMS(uint64_t milliseconds);

  void OnActionPlayTrack(const cTrack* pTrack);
  void OnActionPlaylistRightClick(GdkEventButton* event);

private:
  std::string TimeToString(uint64_t milliseconds) const;

  void on_menu_file_new_generic();
  void on_menu_file_quit();
  void on_menu_others();

  void on_menu_file_popup_generic();

  bool OnPlaybackPositionChanged(Gtk::ScrollType scrollType, double value);
  void OnPlayPauseClicked();
  bool OnTimerPlaybackPosition();

  cGtkmmView& view;

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
  Gtk::Button buttonPrevious;
  Gtk::Button buttonPlay;
  Gtk::Button buttonNext;
  Gtk::HScale volumeSlider;
  Gtk::Label textPosition;
  Gtk::HScale positionSlider;
  Gtk::Label textLength;

  Gtk::Button dummyCategories;

  Gtk::Label dummyStatusBar;

  cGtkmmTrackList* pTrackList;

  sigc::connection timeoutConnection;
};

#endif // gtkmmmainwindow_h
