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
#include "gtkmmdispatcher.h"
#include "gtkmmmainwindow.h"

namespace medusa
{
  class cGtkmmView;

  class cGtkmmViewEvent
  {
  public:
    virtual ~cGtkmmViewEvent() {}

    virtual void EventFunction(cGtkmmView& view) = 0;
  };

  class cGtkmmViewEventPlayerAboutToFinish : public cGtkmmViewEvent
  {
  public:
    virtual void EventFunction(cGtkmmView& view) override;
  };

  class cGtkmmViewEventTrackAdded : public cGtkmmViewEvent
  {
  public:
    cGtkmmViewEventTrackAdded(trackid_t id, const string_t& sFilePath, const spitfire::audio::cMetaData& metaData);

    virtual void EventFunction(cGtkmmView& view) override;

    trackid_t id;
    string_t sFilePath;
    spitfire::audio::cMetaData metaData;
  };

class cGtkmmView : public cView
{
public:
  friend class cGtkmmMainWindow;
  friend class cGStreamermmPlayer;
  friend class cGtkmmViewEventPlayerAboutToFinish;
  friend class cGtkmmViewEventTrackAdded;

  cGtkmmView(int argc, char** argv);
  ~cGtkmmView();

  const cTrack* GetTrack() const { return pCurrentTrack; }

protected:
  void OnActionMainWindowCreated();
  void OnActionMainWindowQuitSoon();
  void OnActionMainWindowQuitNow();

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
  void OnNotify();

  void _Run();

  Gtk::Main kit;

  cGtkmmMainWindow* pMainWindow;

  cGStreamermmPlayer player;

  const cTrack* pCurrentTrack;

  cGtkmmNotifyMainThread notifyMainThread;
  spitfire::util::cSignalObject soAction;
  spitfire::util::cThreadSafeQueue<cGtkmmViewEvent> eventQueue;

  // TODO: Move this to a separate class
  spitfire::util::cMutex mutexSettings;
  cSettings settings;
};
}

#endif // gtkmmview_h
