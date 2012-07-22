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

    void OnActionCreated();
    void OnActionPlayTrack(trackid_t id);
    void OnActionQuitSoon();
    void OnActionQuitNow();

    void AddTracks(const std::vector<string_t>& files);
    void AddTracksFromFolder(const string_t& sFolderPath);
    void RemoveTrack(trackid_t id);
    void UpdateTrackFilePath(trackid_t id, const string_t& sFilePath);

    void OnPlaylistLoading();
    void OnPlaylistLoaded(trackid_t idLastPlayed);
    void OnTrackAdded(trackid_t id, const cTrack& track);
    void OnTracksAdded(const std::vector<cTrack*>& tracks);

  private:
    cModel& model;
    cView& view;
  };
}

#endif // MEDUSA_CONTROLLER_H
