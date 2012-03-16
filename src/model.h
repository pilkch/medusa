#ifndef model_h
#define model_h

// Spitfire headers
#include <spitfire/spitfire.h>
#include <spitfire/audio/metadata.h>

// Medusa headers
#include "track.h"

namespace medusa
{
  class cController;

  class cModel
  {
  public:
    cModel();

    void SetController(cController* pController);

    void Start();
    void StopSoon();
    void StopNow();

    void AddTrack(const string_t& sFilePath);
    void AddTracks(const std::vector<string_t>& files);
    void AddTracksFromFolder(const string_t& sFolderPath);

  private:
    void LoadPlaylist();
    void SavePlaylist() const;

    cController* pController;

    std::vector<cTrack*> tracks;
  };

  // ** cModel

  inline cModel::cModel() :
    pController(nullptr)
  {
  }

  inline void cModel::SetController(cController* _pController)
  {
    pController = _pController;
  }
}

#endif // model_h
