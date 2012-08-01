#ifndef MEDUSA_TRACK_H
#define MEDUSA_TRACK_H

// Spitfire headers
#include <spitfire/audio/metadata.h>
#include <spitfire/util/datetime.h>

// Medusa headers
#include "medusa.h"

namespace medusa
{
  enum class TRACK_STATUS {
    OK,
    FILE_DOES_NOT_EXIST,
    FILE_EMPTY
  };

  class cTrack
  {
  public:
    cTrack();

    string_t sFilePath;

    spitfire::audio::cMetaData metaData;

    spitfire::util::cDateTime dateAdded;
    spitfire::util::cDateTime dateLastPlayed;
    bool bIsPropertiesLoaded;
    TRACK_STATUS status;
  };

  inline cTrack::cTrack():
    bIsPropertiesLoaded(false),
    status(TRACK_STATUS::OK)
  {
    dateLastPlayed.SetFromUnixEpoch();
  }
}

#endif // MEDUSA_TRACK_H
