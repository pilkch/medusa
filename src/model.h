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

  private:
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
