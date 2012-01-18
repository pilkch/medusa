#ifndef gtkmmmainwindow_h
#define gtkmmmainwindow_h

// gtkmm headers
#include <gtkmm.h>

class cGtkmmView;

class cGtkmmMainWindow : public Gtk::Window {
public:
  explicit cGtkmmMainWindow(cGtkmmView& view);
  //virtual ~cGtkmmMainWindow() {}

  void SetStatePlaying();
  void SetStatePaused();
  void SetPlaybackPositionMS(uint64_t milliseconds);
  void SetPlaybackLengthMS(uint64_t milliseconds);

private:
  std::string TimeToString(uint64_t milliseconds) const;

  bool OnPlaybackPositionChanged(Gtk::ScrollType scrollType, double value);
  void OnPlayPauseClicked();
  bool OnTimerPlaybackPosition();

  cGtkmmView& view;

  Gtk::VBox boxMainWindow;
  Gtk::HBox boxPositionSlider;
  Gtk::HScale positionSlider;
  Gtk::Button button;
  Gtk::Label textPosition;
  Gtk::Label textLength;

  sigc::connection m_timeout_connection;
};

#endif // gtkmmmainwindow_h
