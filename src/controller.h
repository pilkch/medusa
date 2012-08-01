#ifndef MEDUSA_CONTROLLER_H
#define MEDUSA_CONTROLLER_H

// Standard headers
#include <list>

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

    void AddTracks(const std::list<string_t>& files);
    void AddTracksFromFolder(const string_t& sFolderPath);
    #ifdef BUILD_MEDUSA_IMPORT_BANSHEE_PLAYLIST
    void ImportFromBanshee();
    #endif
    void ImportFromRhythmbox();
    void StopLoading();
    void RemoveTracks(const std::list<trackid_t>& tracks);
    void UpdateTrackFilePath(trackid_t id, const string_t& sFilePath);

    void OnLoadingFilesToLoadIncrement(size_t nFiles);
    void OnLoadingFilesToLoadDecrement(size_t nFiles);
    void OnPlaylistLoading();
    void OnPlaylistLoaded(trackid_t idLastPlayed);
    void OnTracksAdded(const std::list<trackid_t>& ids, const std::list<cTrack*>& tracks);

  private:
    cModel& model;
    cView& view;
  };
}

#endif // MEDUSA_CONTROLLER_H
