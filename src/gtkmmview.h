#ifndef gtkmmview_h
#define gtkmmview_h

// gtkmm headers
#include <gtkmm.h>

// Spitfire headers
#include <spitfire/util/thread.h>

// Medusa headers
#include "settings.h"
#include "view.h"
#include "gstreamermmplayer.h"
#include "gtkmmmainwindow.h"

class cGtkmmView : public cView
{
public:
  friend class cGtkmmMainWindow;
  friend class cGStreamermmPlayer;

  cGtkmmView(int argc, char** argv);
  ~cGtkmmView();

  const cTrack* GetTrack() const { return player.GetTrack(); }

protected:
  void OnActionPlayTrack(const cTrack* pTrack);
  void OnActionPlaybackPositionChanged(uint64_t seconds);
  void OnActionVolumeChanged(unsigned int uiVolume0To100);
  void OnActionPlay();
  void OnActionPlayPause();
  void OnPlayerUpdatePlaybackPosition();
  void OnPlayerAboutToFinish();

private:
  void _Run();

  Gtk::Main kit;

  cGtkmmMainWindow* pMainWindow;

  cGStreamermmPlayer player;

  // TODO: Move this to a separate class
  spitfire::util::cMutex mutexSettings;
  cSettings settings;
};

#endif // gtkmmview_h
