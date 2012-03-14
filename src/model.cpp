// Standard headers
#include <iostream>

// Medusa headers
#include "controller.h"
#include "model.h"
#include "trackmetadata.h"

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
}
