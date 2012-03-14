#ifndef MEDUSA_CONTROLLER_H
#define MEDUSA_CONTROLLER_H

// Spitfire headers
#include <spitfire/util/string.h>
#include <spitfire/audio/metadata.h>

// Medusa headers
#include "medusa.h"

namespace medusa
{
  class cModel;
  class cView;

  class cController
  {
  public:
    cController(cModel& model, cView& view);

    void AddTrack(const string_t& sFilePath);
    void AddTracks(const std::vector<string_t>& files);
    void AddTracksFromFolder(const string_t& sFolderPath);

    void OnTrackAdded(trackid_t id, const string_t& sFilePath, const spitfire::audio::cMetaData& metaData);

  private:
    cModel& model;
    cView& view;
  };
}

#endif // MEDUSA_CONTROLLER_H
