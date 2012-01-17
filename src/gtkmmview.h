#ifndef gtkmmview_h
#define gtkmmview_h

// gtkmm headers
#include <gtkmm.h>

#include "view.h"
#include "gstreamermmplayer.h"
#include "gtkmmmainwindow.h"

class cGtkmmView : public cView
{
public:
  friend class cGtkmmMainWindow;

  cGtkmmView(int argc, char** argv);
  ~cGtkmmView();

protected:
  void OnActionPlaybackPositionChanged(uint64_t seconds);
  void OnActionPlayPause();
  void OnActionTimerUpdatePlaybackPosition();

private:
  void _Run();

  Gtk::Main kit;

  cGtkmmMainWindow mainWindow;

  cGStreamermmPlayer player;
};

#endif // gtkmmview_h
