#ifndef model_h
#define model_h

// Standard headers
#include <list>
#include <set>
#include <unordered_set>

// Spitfire headers
#include <spitfire/spitfire.h>
#include <spitfire/audio/metadata.h>
#include <spitfire/util/queue.h>
#include <spitfire/util/thread.h>

// Medusa headers
#include "track.h"

namespace medusa
{
  class cModel;

  class cModelEvent
  {
  public:
    virtual ~cModelEvent() {}

    virtual void EventFunction(cModel& model) = 0;
  };

  class cController;

  class cModel : public spitfire::util::cThread
  {
  public:
    cModel();
    ~cModel();

    void SetController(cController* pController);

    void Start();
    void StopSoon();
    void StopNow();

    #ifdef BUILD_MEDUSA_IMPORT_BANSHEE_PLAYLIST
    void LoadBansheePlaylist();
    #endif
    void LoadRhythmBoxPlaylist();

    void AddTracks(const std::list<string_t>& files);
    void AddTracksFromFolder(const string_t& sFolderPath);
    void StopLoading();
    void RemoveTracks(const std::list<trackid_t>& tracks);
    void RemoveTracks(const std::unordered_set<trackid_t>& tracks);
    void MoveTracksToTrash(const std::list<trackid_t>& tracks);
    void UpdateTrackFilePath(trackid_t id, const string_t& sFilePath);

    void SetPlayingTrack(trackid_t id);

  private:
    virtual void ThreadFunction();

    void ClearEventQueue();

    void LoadPlaylist();
    void SavePlaylist() const;

    trackid_t LoadLastPlayed();
    void SaveLastPlayed(trackid_t idLastPlayed) const;

    void CollectFilesInFolder(const string_t& sFolderPath, std::list<string_t>& files) const;

    cController* pController;

    spitfire::util::cSignalObject soAction;
    spitfire::util::cThreadSafeQueue<cModelEvent> eventQueue;

    std::set<cTrack*> tracks;

    class cLoadingProcessInterface : public spitfire::util::cProcessInterface
    {
    public:
      cLoadingProcessInterface();

      override virtual bool IsToStop() const { return soStopLoading.IsSignalled(); }

      void SetStop() { soStopLoading.Signal(); }
      void Reset() { soStopLoading.Reset(); }

    private:
      spitfire::util::cSignalObject soStopLoading;
    };

    cLoadingProcessInterface loadingProcessInterface; // Signalled by the controller when the the model thread should stop loading files
  };

  // ** cModel

  inline void cModel::SetController(cController* _pController)
  {
    pController = _pController;
  }

  inline cModel::cLoadingProcessInterface::cLoadingProcessInterface() :
    soStopLoading("cModel::cLoadingProcessInterface_soStopLoading")
  {
  }
}

#endif // model_h
