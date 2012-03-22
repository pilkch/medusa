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

  void cController::OnTrackAdded(trackid_t id, const string_t& sFilePath, const spitfire::audio::cMetaData& metaData)
  {
    std::wcout<<"cController::OnTrackAdded \""<<sFilePath<<"\""<<std::endl;
    view.OnTrackAdded(id, sFilePath, metaData);
  }

  void cController::OnTracksAdded(const std::vector<cTrack*>& tracks)
  {
    std::cout<<"cController::OnTracksAdded"<<std::endl;
    view.OnTracksAdded(tracks);
  }

  void cController::OnPlaylistLoaded()
  {
    std::cout<<"cController::OnPlaylistLoaded"<<std::endl;
    view.OnPlaylistLoaded();
  }
}
