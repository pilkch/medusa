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
    const string_t sFileExtensionLower = spitfire::string::ToLower(sFileExtension);
    return ((sFileExtensionLower == TEXT(".mp3")) || (sFileExtensionLower == TEXT(".wav")));
  }

  cModelEventAddFiles::cModelEventAddFiles(const std::vector<string_t>& _files) :
    files(_files)
  {
  }

  void cModelEventAddFiles::EventFunction(cModel& model)
  {
    model.AddTracks(files);
  }

  cModelEventAddFolder::cModelEventAddFolder(const string_t& _sFolderPath) :
    sFolderPath(_sFolderPath)
  {
  }

  void cModelEventAddFolder::EventFunction(cModel& model)
  {
    model.AddTracksFromFolder(sFolderPath);
  }


  cModelEventTrackUpdatedFilePath::cModelEventTrackUpdatedFilePath(trackid_t _id, const string_t& _sFilePath) :
    id(_id),
    sFilePath(_sFilePath)
  {
  }

  void cModelEventTrackUpdatedFilePath::EventFunction(cModel& model)
  {
    model.UpdateTrackFilePath(id, sFilePath);
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

  void cModel::AddTracks(const std::vector<string_t>& files)
  {
    std::cout<<"cModel::AddTracks"<<std::endl;

    std::cout<<"cModel::AddTracks 0"<<std::endl;
    if (spitfire::util::IsMainThread()) {
      std::cout<<"cModel::AddTracks 1"<<std::endl;
      cModelEventAddFiles* pEvent = new cModelEventAddFiles(files);
      eventQueue.AddItemToBack(pEvent);
    } else {
      std::cout<<"cModel::AddTracks 2"<<std::endl;

      std::vector<string_t> supportedFiles;

      {
        const size_t n = files.size();
        for (size_t i = 0; i < n; i++) {
          if (IsFileTypeSupported(spitfire::filesystem::GetExtension(files[i]))) supportedFiles.push_back(files[i]);
        }
      }

      if (!supportedFiles.empty()) {
        // Tell the controller that we are about to load this many files
        pController->OnLoadingFilesToLoadIncrement(supportedFiles.size());

        cTrackPropertiesReader propertiesReader;

        const size_t n = supportedFiles.size();
        for (size_t i = 0; i < n; i++) {
          cTrack* pTrack = new cTrack;
          pTrack->sFilePath = supportedFiles[i];
          std::cout<<"cModel::AddTracks Selected file \""<<pTrack->sFilePath<<"\""<<std::endl;
          propertiesReader.ReadTrackProperties(pTrack->metaData, pTrack->sFilePath);

          tracks.push_back(pTrack);

          pController->OnTrackAdded(pTrack, *pTrack);
        }
      }
    }
  }

  void cModel::CollectFilesInFolder(const string_t& sFolderPath, std::vector<string_t>& files) const
  {
    spitfire::filesystem::cFolderIterator iter(sFolderPath);
    iter.SetIgnoreHiddenFilesAndFolders();
    while (iter.IsValid()) {
      if (iter.IsFolder()) CollectFilesInFolder(iter.GetFullPath(), files);
      else files.push_back(iter.GetFullPath());

      iter.Next();
    }
  }

  void cModel::AddTracksFromFolder(const string_t& sFolderPath)
  {
    if (spitfire::util::IsMainThread()) {
      cModelEventAddFolder* pEvent = new cModelEventAddFolder(sFolderPath);
      eventQueue.AddItemToBack(pEvent);
    } else {
      std::cout<<"cModel::AddTracksFromFolder 0"<<std::endl;

      // Collect all files this folder and sub directories
      std::vector<string_t> files;
      CollectFilesInFolder(sFolderPath, files);

      // Now add any found files
      AddTracks(files);
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
          std::cout<<"cModel::RemoveTrack Removing track \""<<(*iter)->sFilePath<<"\""<<std::endl;
          tracks.erase(iter);
          break;
        }

        iter++;
      }
    }
  }

  void cModel::UpdateTrackFilePath(trackid_t id, const string_t& sFilePath)
  {
    if (spitfire::util::IsMainThread()) {
      cModelEventTrackUpdatedFilePath* pEvent = new cModelEventTrackUpdatedFilePath(id, sFilePath);
      eventQueue.AddItemToBack(pEvent);
    } else {
      std::vector<cTrack*>::iterator iter = tracks.begin();
      const std::vector<cTrack*>::iterator iterEnd = tracks.end();
      while (iter != iterEnd) {
        if (*iter == id) {
          std::cout<<"cModel::UpdateTrackFilePath Found track \""<<(*iter)->sFilePath<<"\""<<std::endl;

          // NOTE: The view has already moved the file, we just have to update our model
          cTrack* pTrack = *iter;
          pTrack->sFilePath = sFilePath;

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

    // Tell the main thread that we are about to load the playlist
    pController->OnPlaylistLoading();

    // Load the playlist
    util::LoadPlaylistFromCSV(util::GetPlayListFilePath(), tracks);

    // Tell the controller that we are about to load this many files
    pController->OnLoadingFilesToLoadIncrement(tracks.size());

    // Tell the controller that we added these tracks
    pController->OnTracksAdded(tracks);

    // Load the last played setting
    trackid_t idLastPlayed = LoadLastPlayed();

    // Tell the main thread that we have finished loading the playlist
    pController->OnPlaylistLoaded(idLastPlayed);
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

    // Load the playlist
    LoadPlaylist();

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
