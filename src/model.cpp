// Standard headers
#include <iostream>

// Spitfire headers
#include <spitfire/audio/playlist.h>
#include <spitfire/storage/filesystem.h>

// Medusa headers
#include "controller.h"
#include "model.h"
#include "trackmetadata.h"
#include "util.h"

namespace medusa
{
  bool IsFileTypeSupported(const string_t& sFileExtension)
  {
    return ((sFileExtension == TEXT("mp3")) || (sFileExtension == TEXT(".wav")));
  }

  cModelEventAddFile::cModelEventAddFile(const string_t& _sFilePath) :
    sFilePath(_sFilePath)
  {
  }

  void cModelEventAddFile::EventFunction(cModel& model)
  {
    model.AddTrack(sFilePath);
  }

  cModelEventAddFolder::cModelEventAddFolder(const string_t& _sFolderPath) :
    sFolderPath(_sFolderPath)
  {
  }

  void cModelEventAddFolder::EventFunction(cModel& model)
  {
    model.AddTracksFromFolder(sFolderPath);
  }

  cModelEventRemoveTrack::cModelEventRemoveTrack(trackid_t _id) :
    id(_id)
  {
  }

  void cModelEventRemoveTrack::EventFunction(cModel& model)
  {
    model.RemoveTrack(id);
  }

  // ** cModel

  void cModel::AddTrack(const string_t& sFilePath)
  {
    if (!IsFileTypeSupported(spitfire::filesystem::GetExtension(sFilePath))) return;

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

      pController->OnTrackAdded(pTrack, *pTrack);
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
      cModelEventAddFolder* pEvent = new cModelEventAddFolder(sFolderPath);
      eventQueue.AddItemToBack(pEvent);
    } else {
      spitfire::filesystem::cFolderIterator iter(sFolderPath);
      iter.SetIgnoreHiddenFilesAndFolders();
      while (iter.IsValid()) {
        if (iter.IsFolder()) {
          // Add an event to iterate into this folder later
          cModelEventAddFolder* pEvent = new cModelEventAddFolder(iter.GetFullPath());
          eventQueue.AddItemToBack(pEvent);
        } else AddTrack(iter.GetFullPath());

        iter.Next();
      }
    }
  }

  void cModel::RemoveTrack(trackid_t id)
  {
    if (spitfire::util::IsMainThread()) {
      cModelEventRemoveTrack* pEvent = new cModelEventRemoveTrack(id);
      eventQueue.AddItemToBack(pEvent);
    } else {
      std::vector<cTrack*>::iterator iter = tracks.begin();
      const std::vector<cTrack*>::iterator iterEnd = tracks.end();
      while (iter != iterEnd) {
        if (*iter == id) {
          std::wcout<<"cModel::RemoveTrack Removing track \""<<(*iter)->sFilePath<<"\""<<std::endl;
          tracks.erase(iter);
          break;
        }

        iter++;
      }
    }
  }

  void cModel::LoadPlaylist()
  {
    std::cout<<"cModel::LoadPlaylist"<<std::endl;

    // Save the playlist
    std::vector<cTrack*> playlist;
    util::LoadPlaylistFromCSV(util::GetPlayListFilePath(), playlist);

    const size_t n = playlist.size();
    for (size_t i = 0; i < n; i++) {
      const cTrack* pPlaylistTrack = playlist[i];

      cTrack* pTrack = new cTrack;
      *pTrack = *pPlaylistTrack;

      tracks.push_back(pTrack);
    }

    pController->OnTracksAdded(tracks);
  }

  void cModel::SavePlaylist() const
  {
    // Save the playlist
    std::vector<cTrack*> playlist;

    const size_t n = tracks.size();
    for (size_t i = 0; i < n; i++) {
      const cTrack* pTrack = tracks[i];

      cTrack* pPlaylistTrack = new cTrack;
      *pPlaylistTrack = *pTrack;

      playlist.push_back(pPlaylistTrack);
    }

    util::SavePlaylistToCSV(util::GetPlayListFilePath(), playlist);
  }

  void cModel::ThreadFunction()
  {
    std::cout<<"cModel::ThreadFunction"<<std::endl;

    while (true) {
      //std::cout<<"cModel::ThreadFunction Loop"<<std::endl;
      soAction.WaitTimeoutMS(1000);

      if (IsToStop()) break;

      cModelEvent* pEvent = eventQueue.RemoveItemFromFront();
      if (pEvent != nullptr) {
        pEvent->EventFunction(*this);
        spitfire::SAFE_DELETE(pEvent);
      }

      Yield();
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
    // Load the playlist on the main thread so that we can populate the track list and start playing as soon as possible
    LoadPlaylist();

    pController->OnPlaylistLoaded();

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
