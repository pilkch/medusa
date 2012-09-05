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

  class cGtkmmViewEventPlayerError : public cGtkmmViewEvent
  {
  public:
    virtual void EventFunction(cGtkmmView& view) override;
  };

  class cGtkmmViewEventLoadingFilesToLoadIncrement : public cGtkmmViewEvent
  {
  public:
    explicit cGtkmmViewEventLoadingFilesToLoadIncrement(size_t nFiles);

    virtual void EventFunction(cGtkmmView& view) override;

    size_t nFiles;
  };

  class cGtkmmViewEventLoadingFilesToLoadDecrement : public cGtkmmViewEvent
  {
  public:
    explicit cGtkmmViewEventLoadingFilesToLoadDecrement(size_t nFiles);

    virtual void EventFunction(cGtkmmView& view) override;

    size_t nFiles;
  };

  class cGtkmmViewEventPlaylistLoaded : public cGtkmmViewEvent
  {
  public:
    explicit cGtkmmViewEventPlaylistLoaded(trackid_t idLastPlayed);

    virtual void EventFunction(cGtkmmView& view) override;

    trackid_t idLastPlayed;
  };

  class cGtkmmViewEventPlaylistLoading : public cGtkmmViewEvent
  {
  public:
    virtual void EventFunction(cGtkmmView& view) override;
  };

  class cGtkmmViewEventTracksAdded : public cGtkmmViewEvent
  {
  public:
    explicit cGtkmmViewEventTracksAdded(const std::list<trackid_t>& ids, const std::list<cTrack*>& tracks);

    virtual void EventFunction(cGtkmmView& view) override;

    std::list<trackid_t> ids;
    std::list<cTrack*> tracks;
  };

class cGtkmmView : public cView
{
public:
  friend class cGtkmmMainWindow;
  friend class cGStreamermmPlayer;
  friend class cGtkmmViewEventLoadingFilesToLoadIncrement;
  friend class cGtkmmViewEventLoadingFilesToLoadDecrement;
  friend class cGtkmmViewEventPlayerAboutToFinish;
  friend class cGtkmmViewEventPlayerError;
  friend class cGtkmmViewEventPlaylistLoading;
  friend class cGtkmmViewEventPlaylistLoaded;
  friend class cGtkmmViewEventTracksAdded;

  cGtkmmView(int argc, char** argv);
  ~cGtkmmView();

  trackid_t GetCurrentTrackID() const { return pCurrentTrack; }
  bool IsPlaying() const;

protected:
  void OnActionMainWindowCreated();
  void OnActionMainWindowQuitSoon();
  void OnActionMainWindowQuitNow();

  void OnActionAddTracks(const std::list<string_t>& files);
  void OnActionAddTracksFromFolder(const string_t& sFolderPath);
  #ifdef BUILD_MEDUSA_IMPORT_BANSHEE_PLAYLIST
  void OnActionImportFromBanshee();
  #endif
  void OnActionImportFromRhythmbox();
  void OnActionStopLoading();
  void OnActionRemoveTracks(const std::list<trackid_t>& tracks);
  void OnActionTrackMoveToFolder(trackid_t id, const string_t& sFilePath);
  void OnActionPlayTrack(trackid_t id, const string_t& sFilePath, const spitfire::audio::cMetaData& metaData);
  void OnActionPlaybackPositionChanged(uint64_t seconds);
  void OnActionVolumeChanged(unsigned int uiVolume0To100);
  void OnActionPlay();
  void OnActionPlayPause();
  void OnPlayerUpdatePlaybackPosition();
  void OnPlayerAboutToFinish();
  void OnPlayerError();

  virtual void OnLoadingFilesToLoadIncrement(size_t nFiles) override;
  virtual void OnLoadingFilesToLoadDecrement(size_t nFiles) override;
  virtual void OnPlaylistLoading() override;
  virtual void OnPlaylistLoaded(trackid_t idLastPlayed) override;
  virtual void OnTracksAdded(const std::list<trackid_t>& ids, const std::list<cTrack*>& tracks) override;

private:
  void InstallDesktopFile();

  void OnNotify();

  void _Run();

  Gtk::Main kit;

  string_t sExecutableFolder;

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
