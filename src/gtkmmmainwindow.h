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

protected:
  void OnPlayPauseClicked();

  cGtkmmView& view;

  Gtk::Button button;
};

#endif // gtkmmmainwindow_h
