// Standard headers
#include <iostream>

// Medusa headers
#include "controller.h"
#include "model.h"
#include "view.h"

namespace medusa
{
  // ** cController

  cController::cController(cModel& _model, cView& _view) :
    model(_model),
    view(_view)
  {
    model.SetController(this);
    view.SetController(this);
  }

  void cController::OnActionCreated()
  {
    model.Start();
  }

  void cController::OnActionQuitSoon()
  {
    model.StopSoon();
  }

  void cController::OnActionQuitNow()
  {
    model.StopNow();
  }

  void cController::OnActionPlayTrack(trackid_t id)
  {
    model.SetPlayingTrack(id);
  }

  void cController::AddTracks(const std::list<string_t>& files)
  {
    model.AddTracks(files);
  }

  void cController::AddTracksFromFolder(const string_t& sFolderPath)
  {
    model.AddTracksFromFolder(sFolderPath);
  }

  #ifdef BUILD_MEDUSA_IMPORT_BANSHEE_PLAYLIST
  void cController::ImportFromBanshee()
  {
    model.LoadBansheePlaylist();
  }
  #endif

  void cController::ImportFromRhythmbox()
  {
    model.LoadRhythmBoxPlaylist();
  }

  void cController::RemoveTracks(const std::list<trackid_t>& tracks)
  {
    model.RemoveTracks(tracks);
  }

  void cController::RemoveTracks(const std::unordered_set<trackid_t>& tracks)
  {
    model.RemoveTracks(tracks);
  }

  void cController::UpdateTrackFilePath(trackid_t id, const string_t& sFilePath)
  {
    model.UpdateTrackFilePath(id, sFilePath);
  }

  void cController::StopLoading()
  {
    model.StopLoading();
  }

  void cController::OnLoadingFilesToLoadIncrement(size_t nFiles)
  {
    std::cout<<"cController::OnLoadingFilesToLoadIncrement"<<std::endl;
    view.OnLoadingFilesToLoadIncrement(nFiles);
  }

  void cController::OnLoadingFilesToLoadDecrement(size_t nFiles)
  {
    std::cout<<"cController::OnLoadingFilesToLoadDecrement"<<std::endl;
    view.OnLoadingFilesToLoadDecrement(nFiles);
  }

  void cController::OnPlaylistLoading()
  {
    std::cout<<"cController::OnPlaylistLoading"<<std::endl;
    view.OnPlaylistLoading();
  }

  void cController::OnPlaylistLoaded(trackid_t idLastPlayed)
  {
    std::cout<<"cController::OnPlaylistLoaded"<<std::endl;
    view.OnPlaylistLoaded(idLastPlayed);
  }

  void cController::OnTracksAdded(const std::list<trackid_t>& ids, const std::list<cTrack*>& tracks)
  {
    std::cout<<"cController::OnTracksAdded"<<std::endl;
    view.OnTracksAdded(ids, tracks);
  }
}
