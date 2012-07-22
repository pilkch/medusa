#ifndef model_h
#define model_h

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

  class cModelEventAddFiles : public cModelEvent
  {
  public:
    explicit cModelEventAddFiles(const std::vector<string_t>& files);

    virtual void EventFunction(cModel& model) override;

    std::vector<string_t> files;
  };

  class cModelEventAddFolder : public cModelEvent
  {
  public:
    explicit cModelEventAddFolder(const string_t& sFolderPath);

    virtual void EventFunction(cModel& model) override;

    string_t sFolderPath;
  };

  class cModelEventTrackUpdatedFilePath : public cModelEvent
  {
  public:
    explicit cModelEventTrackUpdatedFilePath(trackid_t id, const string_t& sFilePath);

    virtual void EventFunction(cModel& model) override;

    trackid_t id;
    string_t sFilePath;
  };

  class cModelEventRemoveTracks : public cModelEvent
  {
  public:
    explicit cModelEventRemoveTracks(const std::vector<trackid_t>& tracks);

    virtual void EventFunction(cModel& model) override;

    std::vector<trackid_t> tracks;
  };

  class cModelEventPlayingTrack : public cModelEvent
  {
  public:
    explicit cModelEventPlayingTrack(trackid_t id);

    virtual void EventFunction(cModel& model) override;

    trackid_t id;
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

    void AddTracks(const std::vector<string_t>& files);
    void AddTracksFromFolder(const string_t& sFolderPath);
    void StopLoading();
    void RemoveTracks(const std::vector<trackid_t>& tracks);
    void UpdateTrackFilePath(trackid_t id, const string_t& sFilePath);

    void SetPlayingTrack(trackid_t id);

  private:
    virtual void ThreadFunction();

    void LoadPlaylist();
    void SavePlaylist() const;

    trackid_t LoadLastPlayed();
    void SaveLastPlayed(trackid_t idLastPlayed) const;

    void CollectFilesInFolder(const string_t& sFolderPath, std::vector<string_t>& files) const;

    cController* pController;

    spitfire::util::cSignalObject soAction;
    spitfire::util::cThreadSafeQueue<cModelEvent> eventQueue;

    std::vector<cTrack*> tracks;

    spitfire::util::cSignalObject soStopLoading; // Signalled by the controller when the the model thread should stop loading files
  };

  // ** cModel

  inline void cModel::SetController(cController* _pController)
  {
    pController = _pController;
  }
}

#endif // model_h
