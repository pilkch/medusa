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
  const size_t INVALID_TRACK_INDEX = std::numeric_limits<size_t>::max();

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

  cModelEventPlayingTrack::cModelEventPlayingTrack(trackid_t _id) :
    id(_id)
  {
  }

  void cModelEventPlayingTrack::EventFunction(cModel& model)
  {
    model.SetPlayingTrack(id);
  }


  // ** cModel

  cModel::cModel() :
    spitfire::util::cThread(soAction, "cModel"),
    pController(nullptr),
    soAction("cModel_soAction"),
    eventQueue(soAction)
  {
  }

  cModel::~cModel()
  {
    const size_t n = tracks.size();
    for (size_t i = 0; i < n; i++) spitfire::SAFE_DELETE(tracks[i]);
    tracks.clear();
  }

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

  void cModel::SetPlayingTrack(trackid_t id)
  {
    if (spitfire::util::IsMainThread()) {
      cModelEventPlayingTrack* pEvent = new cModelEventPlayingTrack(id);
      eventQueue.AddItemToBack(pEvent);
    } else {
      SaveLastPlayed(id);
    }
  }

  void cModel::LoadPlaylist()
  {
    std::cout<<"cModel::LoadPlaylist"<<std::endl;

    // Load the playlist
    util::LoadPlaylistFromCSV(util::GetPlayListFilePath(), tracks);

    pController->OnTracksAdded(tracks);
  }

  void cModel::SavePlaylist() const
  {
    std::cout<<"cModel::SavePlaylist"<<std::endl;

    // Save the playlist
    util::SavePlaylistToCSV(util::GetPlayListFilePath(), tracks);
  }

  trackid_t cModel::LoadLastPlayed()
  {
    std::cout<<"cModel::LoadLastPlayed"<<std::endl;

    trackid_t idLastPlayed = INVALID_TRACK;

    // Load the last played track index
    size_t index = INVALID_TRACK_INDEX;

    spitfire::storage::cReadTextFile file(util::GetLastPlayedFilePath());
    if (file.IsOpen()) {
      // Read the index from the file
      string_t sLine;
      if (file.ReadLine(sLine)) {
        index = spitfire::string::ToUnsignedInt(sLine);
      }
    }

    // Get the last played track
    if (index < tracks.size()) idLastPlayed = tracks[index];

    return idLastPlayed;
  }

  void cModel::SaveLastPlayed(trackid_t idLastPlayed) const
  {
    std::cout<<"cModel::SaveLastPlayed"<<std::endl;

    // Get the last played track index
    size_t index = INVALID_TRACK_INDEX;

    const size_t n = tracks.size();
    for (size_t i = 0; i < n; i++) {
      if (tracks[i] == idLastPlayed) {
        index = i;
        break;
      }
    }

    // Save the last played track index
    if (index != INVALID_TRACK_INDEX) {
      spitfire::storage::cWriteTextFile file(util::GetLastPlayedFilePath());
      if (file.IsOpen()) {
        // Write the index to the file
        file.WriteLine(spitfire::string::ToString(index));
      }
    } else {
      // Otherwise delete the file
      spitfire::filesystem::DeleteFile(util::GetLastPlayedFilePath());
    }
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

    // Save the playlist on the background thread so that we can still process events on the main thread
    SavePlaylist();

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

    // Load the last played setting
    trackid_t idLastPlayed = LoadLastPlayed();

    pController->OnPlaylistLoaded(idLastPlayed);

    // Start
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
