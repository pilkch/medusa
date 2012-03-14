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

namespace medusa
{
class cGtkmmView : public cView
{
public:
  friend class cGtkmmMainWindow;
  friend class cGStreamermmPlayer;

  cGtkmmView(int argc, char** argv);
  ~cGtkmmView();

  const cTrack* GetTrack() const { return pCurrentTrack; }

protected:
  void OnActionAddTrack(const string_t& sFilePath);
  void OnActionAddTracks(const std::vector<string_t>& files);
  void OnActionAddTracksFromFolder(const string_t& sFolderPath);
  void OnActionPlayTrack(const cTrack* pTrack);
  void OnActionPlaybackPositionChanged(uint64_t seconds);
  void OnActionVolumeChanged(unsigned int uiVolume0To100);
  void OnActionPlay();
  void OnActionPlayPause();
  void OnPlayerUpdatePlaybackPosition();
  void OnPlayerAboutToFinish();

  virtual void OnTrackAdded(trackid_t id, const string_t& sFilePath, const spitfire::audio::cMetaData& metaData) override;

private:
  void _Run();

  Gtk::Main kit;

  cGtkmmMainWindow* pMainWindow;

  cGStreamermmPlayer player;

  const cTrack* pCurrentTrack;

  // TODO: Move this to a separate class
  spitfire::util::cMutex mutexSettings;
  cSettings settings;
};
}

#endif // gtkmmview_h
