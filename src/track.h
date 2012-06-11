#ifndef MEDUSA_TRACK_H
#define MEDUSA_TRACK_H

// Spitfire headers
#include <spitfire/audio/metadata.h>
#include <spitfire/util/datetime.h>

// Medusa headers
#include "medusa.h"

namespace medusa
{
  class cTrack
  {
  public:
    cTrack();

    string_t sFilePath;

    spitfire::audio::cMetaData metaData;

    spitfire::util::cDateTime dateAdded;
    spitfire::util::cDateTime dateLastPlayed;
    bool bIsPropertiesLoaded;
  };

  inline cTrack::cTrack():
    bIsPropertiesLoaded(false)
  {
    dateLastPlayed.SetFromUnixEpoch();
  }
}

#endif // MEDUSA_TRACK_H
