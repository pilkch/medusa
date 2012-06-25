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

  class cModelEventAddFile : public cModelEvent
  {
  public:
    explicit cModelEventAddFile(const string_t& sFilePath);

    virtual void EventFunction(cModel& model) override;

    string_t sFilePath;
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

  class cModelEventRemoveTrack : public cModelEvent
  {
  public:
    explicit cModelEventRemoveTrack(trackid_t id);

    virtual void EventFunction(cModel& model) override;

    trackid_t id;
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

    void AddTrack(const string_t& sFilePath);
    void AddTracks(const std::vector<string_t>& files);
    void AddTracksFromFolder(const string_t& sFolderPath);
    void RemoveTrack(trackid_t id);
    void UpdateTrackFilePath(trackid_t id, const string_t& sFilePath);

    void SetPlayingTrack(trackid_t id);

  private:
    virtual void ThreadFunction();

    void LoadPlaylist();
    void SavePlaylist() const;

    trackid_t LoadLastPlayed();
    void SaveLastPlayed(trackid_t idLastPlayed) const;

    cController* pController;

    spitfire::util::cSignalObject soAction;
    spitfire::util::cThreadSafeQueue<cModelEvent> eventQueue;

    std::vector<cTrack*> tracks;
  };

  // ** cModel

  inline void cModel::SetController(cController* _pController)
  {
    pController = _pController;
  }
}

#endif // model_h
