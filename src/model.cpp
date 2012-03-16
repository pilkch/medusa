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
  void cModel::AddTrack(const string_t& sFilePath)
  {
    cTrackPropertiesReader propertiesReader;

    cTrack* pTrack = new cTrack;
    pTrack->sFilePath = sFilePath;
    std::wcout<<"cModel::AddTrack Selected file \""<<pTrack->sFilePath<<"\""<<std::endl;
    propertiesReader.ReadTrackProperties(pTrack->metaData, pTrack->sFilePath);

    tracks.push_back(pTrack);

    pController->OnTrackAdded(pTrack, sFilePath, pTrack->metaData);
  }

  void cModel::AddTracks(const std::vector<string_t>& files)
  {
    cTrackPropertiesReader propertiesReader;

    const size_t n = files.size();
    for (size_t i = 0; i < n; i++) {
      cTrack* pTrack = new cTrack;
      pTrack->sFilePath = files[i];
      std::wcout<<"cModel::AddTracks Selected file \""<<pTrack->sFilePath<<"\""<<std::endl;
      propertiesReader.ReadTrackProperties(pTrack->metaData, pTrack->sFilePath);

      tracks.push_back(pTrack);

      pController->OnTrackAdded(pTrack, pTrack->sFilePath, pTrack->metaData);
    }
  }

  void cModel::AddTracksFromFolder(const string_t& sFolderPath)
  {
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

  void cModel::Start()
  {
    LoadPlaylist();
  }

  void cModel::StopSoon()
  {
    // TODO: Stop the thread soon

    // TODO: Move this to the thread
    SavePlaylist();
  }

  void cModel::StopNow()
  {
    // TODO: Stop the thread
  }
}
