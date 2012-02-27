#ifndef MEDUSA_TRACK_H
#define MEDUSA_TRACK_H

// Spitfire headers
#include <spitfire/spitfire.h>
#include <spitfire/audio/metadata.h>

namespace medusa
{
  class cTrack
  {
  public:
    spitfire::string_t sFilePath;

    spitfire::audio::cMetaData metaData;

    bool bIsPropertiesLoaded;
  };
}

#endif // MEDUSA_TRACK_H
