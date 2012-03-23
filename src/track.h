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
    string_t sFilePath;

    spitfire::audio::cMetaData metaData;

    spitfire::util::cDateTime dateAdded;
    bool bIsPropertiesLoaded;
  };
}

#endif // MEDUSA_TRACK_H
