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

  void cController::AddTrack(const string_t& sFilePath)
  {
    model.AddTrack(sFilePath);
  }

  void cController::AddTracks(const std::vector<string_t>& files)
  {
    model.AddTracks(files);
  }

  void cController::AddTracksFromFolder(const string_t& sFolderPath)
  {
    model.AddTracksFromFolder(sFolderPath);
  }

  void cController::RemoveTrack(trackid_t id)
  {
    model.RemoveTrack(id);
  }

  void cController::UpdateTrackFilePath(trackid_t id, const string_t& sFilePath)
  {
    model.UpdateTrackFilePath(id, sFilePath);
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

  void cController::OnTrackAdded(trackid_t id, const cTrack& track)
  {
    std::cout<<"cController::OnTrackAdded \""<<track.sFilePath<<"\""<<std::endl;
    view.OnTrackAdded(id, track);
  }

  void cController::OnTracksAdded(const std::vector<cTrack*>& tracks)
  {
    std::cout<<"cController::OnTracksAdded"<<std::endl;
    view.OnTracksAdded(tracks);
  }
}
