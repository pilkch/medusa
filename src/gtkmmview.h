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

  const cTrack* GetTrack() const { return player.GetTrack(); }

protected:
  void OnActionPlayTrack(const cTrack* pTrack);
  void OnActionPlaybackPositionChanged(uint64_t seconds);
  void OnActionPlay();
  void OnActionPlayPause();
  void OnActionTimerUpdatePlaybackPosition();

private:
  void _Run();

  Gtk::Main kit;

  cGtkmmMainWindow mainWindow;

  cGStreamermmPlayer player;
};

#endif // gtkmmview_h
