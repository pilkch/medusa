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
#include "webserver.h"

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

  class cGtkmmViewEventWebServerPreviousTrack : public cGtkmmViewEvent
  {
  public:
    virtual void EventFunction(cGtkmmView& view) override;
  };

  class cGtkmmViewEventWebServerPlayPause : public cGtkmmViewEvent
  {
  public:
    virtual void EventFunction(cGtkmmView& view) override;
  };

  class cGtkmmViewEventWebServerNextTrack : public cGtkmmViewEvent
  {
  public:
    virtual void EventFunction(cGtkmmView& view) override;
  };

  class cGtkmmViewEventWebServerSetVolumeMute : public cGtkmmViewEvent
  {
  public:
    virtual void EventFunction(cGtkmmView& view) override;
  };

  class cGtkmmViewEventWebServerSetVolumeFull : public cGtkmmViewEvent
  {
  public:
    virtual void EventFunction(cGtkmmView& view) override;
  };

  class cGtkmmViewEventWebServerTrackMoveToRubbishBin : public cGtkmmViewEvent
  {
  public:
    explicit cGtkmmViewEventWebServerTrackMoveToRubbishBin(trackid_t id);

    virtual void EventFunction(cGtkmmView& view) override;

    trackid_t id;
  };

  class cGtkmmViewEventUpdateCheckerNewVersionFound : public cGtkmmViewEvent
  {
  public:
    cGtkmmViewEventUpdateCheckerNewVersionFound(int iMajorVersion, int iMinorVersion, const string_t& sDownloadPage);

    virtual void EventFunction(cGtkmmView& view) override;

    int iMajorVersion;
    int iMinorVersion;
    string_t sDownloadPage;
  };

class cGtkmmView : public cView, public spitfire::util::cUpdateCheckerHandler
{
public:
  friend class cGtkmmMainWindow;
  friend class cGStreamermmPlayer;
  friend class cWebServer;
  friend class cGtkmmViewEventLoadingFilesToLoadIncrement;
  friend class cGtkmmViewEventLoadingFilesToLoadDecrement;
  friend class cGtkmmViewEventPlayerAboutToFinish;
  friend class cGtkmmViewEventPlayerError;
  friend class cGtkmmViewEventPlaylistLoading;
  friend class cGtkmmViewEventPlaylistLoaded;
  friend class cGtkmmViewEventTracksAdded;
  friend class cGtkmmViewEventWebServerPreviousTrack;
  friend class cGtkmmViewEventWebServerPlayPause;
  friend class cGtkmmViewEventWebServerNextTrack;
  friend class cGtkmmViewEventWebServerSetVolumeMute;
  friend class cGtkmmViewEventWebServerSetVolumeFull;
  friend class cGtkmmViewEventWebServerTrackMoveToRubbishBin;
  friend class cGtkmmViewEventUpdateCheckerNewVersionFound;

  cGtkmmView(int argc, char** argv);
  ~cGtkmmView();

  trackid_t GetCurrentTrackID() const { return pCurrentTrack; }
  bool IsPlaying() const;

protected:
  void OnActionMainWindowCreated();
  void OnActionMainWindowApplySettings();
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

  void OnWebServerPreviousTrack();
  void OnWebServerPlayPause();
  void OnWebServerNextTrack();
  void OnWebServerSetVolumeMute();
  void OnWebServerSetVolumeFull();
  void OnWebServerTrackMoveToRubbishBin(trackid_t id);

private:
  void InstallDesktopFile();

  void OnNotify();

  void _Run();

  // Called by the update checker
  virtual void OnNewVersionFound(int iMajorVersion, int iMinorVersion, const string_t& sDownloadPage) override;

  Gtk::Main kit;

  string_t sExecutableFolder;

  cGtkmmMainWindow* pMainWindow;

  cGStreamermmPlayer player;

  cWebServer webServer;

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
