// Standard headers
#include <iostream>

// Spitfire headers
#include <spitfire/audio/playlist.h>

// Medusa headers
#include "controller.h"
#include "model.h"
#include "trackmetadata.h"
#include "util.h"

namespace medusa
{
  void cModelEventAddFile::EventFunction(cModel& model)
  {
    model.AddTrack(sFilePath);
  }

  cModelEventAddFile::cModelEventAddFile(const string_t& _sFilePath) :
    sFilePath(_sFilePath)
  {
  }

  void cModelEventAddFolder::EventFunction(cModel& model)
  {
    model.AddTracksFromFolder(sFolderPath);
  }

  // ** cModel

  void cModel::AddTrack(const string_t& sFilePath)
  {
    if (spitfire::util::IsMainThread()) {
      cModelEventAddFile* pEvent = new cModelEventAddFile(sFilePath);
      eventQueue.AddItemToBack(pEvent);
    } else {
      cTrackPropertiesReader propertiesReader;

      cTrack* pTrack = new cTrack;
      pTrack->sFilePath = sFilePath;
      std::wcout<<"cModel::AddTrack Selected file \""<<pTrack->sFilePath<<"\""<<std::endl;
      propertiesReader.ReadTrackProperties(pTrack->metaData, pTrack->sFilePath);

      tracks.push_back(pTrack);

      pController->OnTrackAdded(pTrack, sFilePath, pTrack->metaData);
    }
  }

  void cModel::AddTracks(const std::vector<string_t>& files)
  {
    const size_t n = files.size();
    for (size_t i = 0; i < n; i++) AddTrack(files[i]);
  }

  void cModel::AddTracksFromFolder(const string_t& sFolderPath)
  {
    if (spitfire::util::IsMainThread()) {
      cModelEventAddFolder* pEvent = new cModelEventAddFolder;
      pEvent->sFolderPath = sFolderPath;
      eventQueue.AddItemToBack(pEvent);
    } else {
      // TODO: Add contents of folder
    }
  }

  void cModel::LoadPlaylist()
  {
    std::cout<<"cModel::LoadPlaylist"<<std::endl;

    // Save the playlist
    spitfire::audio::cPlaylist playlist;
    util::LoadPlaylistFromCSV(util::GetPlayListFilePath(), playlist);

    const size_t n = playlist.tracks.size();
    for (size_t i = 0; i < n; i++) {
      const spitfire::audio::cTrack* pPlaylistTrack = playlist.tracks[i];

      cTrack* pTrack = new cTrack;
      pTrack->sFilePath = pPlaylistTrack->sFullPath;

      pTrack->metaData.sArtist = pPlaylistTrack->sArtist;
      pTrack->metaData.sTitle = pPlaylistTrack->sTitle;
      pTrack->metaData.uiDurationMilliSeconds = pPlaylistTrack->uiTrackLengthMS;

      tracks.push_back(pTrack);

      pController->OnTrackAdded(pTrack, pTrack->sFilePath, pTrack->metaData);
    }
  }

  void cModel::SavePlaylist() const
  {
    // Save the playlist
    spitfire::audio::cPlaylist playlist;

    const size_t n = tracks.size();
    for (size_t i = 0; i < n; i++) {
      const cTrack* pTrack = tracks[i];

      util::AddTrackToPlaylist(playlist, pTrack);
    }

    util::SavePlaylistToCSV(util::GetPlayListFilePath(), playlist);
  }

  void cModel::ThreadFunction()
  {
    std::cout<<"cModel::ThreadFunction"<<std::endl;

    LoadPlaylist();

    while (true) {
      //std::cout<<"cModel::ThreadFunction Loop"<<std::endl;
      soAction.WaitTimeoutMS(100);

      if (IsToStop()) break;

      cModelEvent* pEvent = eventQueue.RemoveItemFromFront();
      if (pEvent != nullptr) {
        pEvent->EventFunction(*this);
        spitfire::SAFE_DELETE(pEvent);
      }
    }

    SavePlaylist();

    {
      const size_t n = tracks.size();
      for (size_t i = 0; i < n; i++) spitfire::SAFE_DELETE(tracks[i]);
      tracks.clear();
    }

    // Remove any further events because we don't care any more
    while (true) {
      cModelEvent* pEvent = eventQueue.RemoveItemFromFront();
      if (pEvent == nullptr) break;

      spitfire::SAFE_DELETE(pEvent);
    }
  }

  void cModel::Start()
  {
    Run();
  }

  void cModel::StopSoon()
  {
    StopThreadSoon();
  }

  void cModel::StopNow()
  {
    StopThreadNow();
  }
}
