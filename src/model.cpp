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

  template <typename T>
  inline void UpdateStatusOnTracks(T& tracks)
  {
    typename T::iterator iter = tracks.begin();
    const typename T::const_iterator iterEnd = tracks.end();
    while (iter != iterEnd) {
      const string_t& sFilePath = (*iter)->sFilePath;
      (*iter)->status = TRACK_STATUS::OK;
      if (!spitfire::filesystem::FileExists(sFilePath)) (*iter)->status = TRACK_STATUS::FILE_DOES_NOT_EXIST;
      else if (spitfire::filesystem::GetFileSizeBytes(sFilePath) == 0) (*iter)->status = TRACK_STATUS::FILE_EMPTY;

      iter++;
    }
  }


  class cModelEventAddFiles : public cModelEvent
  {
  public:
    explicit cModelEventAddFiles(const std::list<string_t>& files);

    virtual void EventFunction(cModel& model) override;

    std::list<string_t> files;
  };

  class cModelEventAddFolder : public cModelEvent
  {
  public:
    explicit cModelEventAddFolder(const string_t& sFolderPath);

    virtual void EventFunction(cModel& model) override;

    string_t sFolderPath;
  };

  #ifdef BUILD_MEDUSA_IMPORT_BANSHEE_PLAYLIST
  class cModelEventLoadFromBanshee : public cModelEvent
  {
  public:
    virtual void EventFunction(cModel& model) override;
  };
  #endif

  class cModelEventLoadFromRhythmBox : public cModelEvent
  {
  public:
    virtual void EventFunction(cModel& model) override;
  };

  class cModelEventTrackUpdatedFilePath : public cModelEvent
  {
  public:
    explicit cModelEventTrackUpdatedFilePath(trackid_t id, const string_t& sFilePath);

    virtual void EventFunction(cModel& model) override;

    trackid_t id;
    string_t sFilePath;
  };

  class cModelEventRemoveTracks : public cModelEvent
  {
  public:
    explicit cModelEventRemoveTracks(const std::list<trackid_t>& tracks);

    virtual void EventFunction(cModel& model) override;

    std::list<trackid_t> tracks;
  };

  class cModelEventRemoveTracksUnorderedSet : public cModelEvent
  {
  public:
    explicit cModelEventRemoveTracksUnorderedSet(const std::unordered_set<trackid_t>& tracks);

    virtual void EventFunction(cModel& model) override;

    std::unordered_set<trackid_t> tracks;
  };

  class cModelEventMoveTracksToTrash : public cModelEvent
  {
  public:
    explicit cModelEventMoveTracksToTrash(const std::list<trackid_t>& tracks);

    virtual void EventFunction(cModel& model) override;

    std::list<trackid_t> tracks;
  };

  class cModelEventPlayingTrack : public cModelEvent
  {
  public:
    explicit cModelEventPlayingTrack(trackid_t id);

    virtual void EventFunction(cModel& model) override;

    trackid_t id;
  };

  cModelEventAddFiles::cModelEventAddFiles(const std::list<string_t>& _files) :
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


  #ifdef BUILD_MEDUSA_IMPORT_BANSHEE_PLAYLIST
  void cModelEventLoadFromBanshee::EventFunction(cModel& model)
  {
    model.LoadBansheePlaylist();
  }
  #endif

  void cModelEventLoadFromRhythmBox::EventFunction(cModel& model)
  {
    model.LoadRhythmBoxPlaylist();
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


  cModelEventRemoveTracks::cModelEventRemoveTracks(const std::list<trackid_t>& _tracks) :
    tracks(_tracks)
  {
  }

  void cModelEventRemoveTracks::EventFunction(cModel& model)
  {
    model.RemoveTracks(tracks);
  }

  cModelEventRemoveTracksUnorderedSet::cModelEventRemoveTracksUnorderedSet(const std::unordered_set<trackid_t>& _tracks) :
    tracks(_tracks)
  {
  }

  void cModelEventRemoveTracksUnorderedSet::EventFunction(cModel& model)
  {
    model.RemoveTracks(tracks);
  }


  cModelEventMoveTracksToTrash::cModelEventMoveTracksToTrash(const std::list<trackid_t>& _tracks) :
    tracks(_tracks)
  {
  }

  void cModelEventMoveTracksToTrash::EventFunction(cModel& model)
  {
    model.MoveTracksToTrash(tracks);
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
    std::set<cTrack*>::iterator iter = tracks.begin();
    const std::set<cTrack*>::iterator iterEnd = tracks.end();
    while (iter != iterEnd) {
      delete *iter;

      iter++;
    }

    tracks.clear();
  }

  void cModel::AddTracks(const std::list<string_t>& files)
  {
    std::cout<<"cModel::AddTracks"<<std::endl;

    std::cout<<"cModel::AddTracks 0"<<std::endl;
    if (spitfire::util::IsMainThread()) {
      std::cout<<"cModel::AddTracks 1"<<std::endl;
      cModelEventAddFiles* pEvent = new cModelEventAddFiles(files);
      eventQueue.AddItemToBack(pEvent);
    } else {
      std::cout<<"cModel::AddTracks 2"<<std::endl;

      std::list<string_t> supportedFiles;

      {
        std::list<string_t>::const_iterator iter = files.begin();
        const std::list<string_t>::const_iterator iterEnd = files.end();
        while (iter != iterEnd) {
          if (IsFileTypeSupported(spitfire::filesystem::GetExtension(*iter))) supportedFiles.push_back(*iter);

          iter++;
        }
      }

      if (!supportedFiles.empty()) {
        // Tell the controller that we are about to load this many files
        pController->OnLoadingFilesToLoadIncrement(supportedFiles.size());

        cTrackPropertiesReader propertiesReader;

        const size_t n = supportedFiles.size();
        size_t i = 0;

        std::list<string_t>::const_iterator iter = supportedFiles.begin();
        const std::list<string_t>::const_iterator iterEnd = supportedFiles.end();
        while (iter != iterEnd) {
          // Check if we have to stop loading
          if (IsToStop() || loadingProcessInterface.IsToStop()) {
            pController->OnLoadingFilesToLoadDecrement(n - i);
            break;
          }

          cTrack* pTrack = new cTrack;
          pTrack->sFilePath = *iter;
          //std::cout<<"cModel::AddTracks Selected file \""<<pTrack->sFilePath<<"\""<<std::endl;
          propertiesReader.ReadTrackProperties(pTrack->metaData, pTrack->sFilePath);
          //std::cout<<"cModel::AddTracks After ReadTrackProperties"<<std::endl;

          tracks.insert(pTrack);

          {
            // TODO: Group these and add them say, 10 at a time
            std::list<trackid_t> ids;
            ids.push_back(pTrack);
            std::list<cTrack*> tracks;
            tracks.push_back(pTrack);

            UpdateStatusOnTracks(tracks);

            pController->OnTracksAdded(ids, tracks);
          }

          i++;

          iter++;
        }

        // Keep the saved playlist up to date in case we crash
        // TODO: This seems to corrupt memory which is used later on for some reason?
        SavePlaylist();

        std::cout<<"cModel::AddTracks cTrackPropertiesReader is falling out of scope"<<std::endl;
      }

      std::cout<<"cModel::AddTracks cTrackPropertiesReader has fallen out of scope"<<std::endl;
    }
  }

  void cModel::CollectFilesInFolder(const string_t& sFolderPath, std::list<string_t>& files) const
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
      std::list<string_t> files;
      CollectFilesInFolder(sFolderPath, files);

      // Now add any found files
      AddTracks(files);
    }
  }

  void cModel::RemoveTracks(const std::list<trackid_t>& tracksToRemove)
  {
    if (spitfire::util::IsMainThread()) {
      cModelEventRemoveTracks* pEvent = new cModelEventRemoveTracks(tracksToRemove);
      eventQueue.AddItemToBack(pEvent);
    } else {
      // For each track to remove
      std::list<const cTrack*>::const_iterator iterRemove = tracksToRemove.begin();
      const std::list<const cTrack*>::const_iterator iterRemoveEnd = tracksToRemove.end();
      while (iterRemove != iterRemoveEnd) {
        // Find the track in the list and remove it
        cTrack* pTrack = const_cast<cTrack*>(*iterRemove);
        std::set<cTrack*>::iterator iter = tracks.find(pTrack);
        if (iter != tracks.end()) tracks.erase(iter);

        iterRemove++;
      }

      // Keep the saved playlist up to date in case we crash
      // TODO: This seems to corrupt memory which is used later on for some reason?
      SavePlaylist();
    }
  }

  void cModel::RemoveTracks(const std::unordered_set<trackid_t>& tracksToRemove)
  {
    if (spitfire::util::IsMainThread()) {
      cModelEventRemoveTracksUnorderedSet* pEvent = new cModelEventRemoveTracksUnorderedSet(tracksToRemove);
      eventQueue.AddItemToBack(pEvent);
    } else {
      // For each track to remove
      std::unordered_set<const cTrack*>::const_iterator iterRemove = tracksToRemove.begin();
      const std::unordered_set<const cTrack*>::const_iterator iterRemoveEnd = tracksToRemove.end();
      while (iterRemove != iterRemoveEnd) {
        // Find the track in the list and remove it
        cTrack* pTrack = const_cast<cTrack*>(*iterRemove);
        std::set<cTrack*>::iterator iter = tracks.find(pTrack);
        if (iter != tracks.end()) tracks.erase(iter);

        iterRemove++;
      }

      // Keep the saved playlist up to date in case we crash
      // TODO: This seems to corrupt memory which is used later on for some reason?
      SavePlaylist();
    }
  }

  void cModel::MoveTracksToTrash(const std::list<trackid_t>& tracksToRemove)
  {
    if (spitfire::util::IsMainThread()) {
      cModelEventMoveTracksToTrash* pEvent = new cModelEventMoveTracksToTrash(tracksToRemove);
      eventQueue.AddItemToBack(pEvent);
    } else {
      // For each track to remove
      std::list<const cTrack*>::const_iterator iterRemove = tracksToRemove.begin();
      const std::list<const cTrack*>::const_iterator iterRemoveEnd = tracksToRemove.end();
      while (iterRemove != iterRemoveEnd) {
        // Find the track in the list and remove it
        cTrack* pTrack = const_cast<cTrack*>(*iterRemove);
        std::set<cTrack*>::iterator iter = tracks.find(pTrack);
        if (iter != tracks.end()) {
          tracks.erase(iter);

          // Move the file to the trash
          if (spitfire::filesystem::FileExists(pTrack->sFilePath)) spitfire::filesystem::MoveFileToTrash(pTrack->sFilePath);
        }

        iterRemove++;
      }

      // Keep the saved playlist up to date in case we crash
      // TODO: This seems to corrupt memory which is used later on for some reason?
      SavePlaylist();
    }
  }

  void cModel::UpdateTrackFilePath(trackid_t id, const string_t& sFilePath)
  {
    if (spitfire::util::IsMainThread()) {
      cModelEventTrackUpdatedFilePath* pEvent = new cModelEventTrackUpdatedFilePath(id, sFilePath);
      eventQueue.AddItemToBack(pEvent);
    } else {
      std::set<cTrack*>::iterator iter = tracks.begin();
      const std::set<cTrack*>::iterator iterEnd = tracks.end();
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

    UpdateStatusOnTracks(tracks);

    // Tell the controller that we added these tracks
    {
      std::list<trackid_t> ids;
      std::list<cTrack*> tempTracks;
      std::set<cTrack*>::iterator iter = tracks.begin();
      const std::set<cTrack*>::const_iterator iterEnd = tracks.end();
      while (iter != iterEnd) {
        ids.push_back(*iter);
        tempTracks.push_back(*iter);

        iter++;
      }
      pController->OnTracksAdded(ids, tempTracks);
    }

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

  #ifdef BUILD_MEDUSA_IMPORT_BANSHEE_PLAYLIST
  void cModel::LoadBansheePlaylist()
  {
    if (spitfire::util::IsMainThread()) {
      cModelEventLoadFromBanshee* pEvent = new cModelEventLoadFromBanshee;
      eventQueue.AddItemToBack(pEvent);
    } else {
      std::cout<<"cModel::LoadBansheePlaylist"<<std::endl;
      std::list<string_t> files;
      util::LoadBansheePlaylistFile(files);
      AddTracks(files);
    }
  }
  #endif

  void cModel::LoadRhythmBoxPlaylist()
  {
    if (spitfire::util::IsMainThread()) {
      cModelEventLoadFromRhythmBox* pEvent = new cModelEventLoadFromRhythmBox;
      eventQueue.AddItemToBack(pEvent);
    } else {
      std::cout<<"cModel::LoadRhythmBoxPlaylist"<<std::endl;

      // Tell the controller that we are about to load this this playlist file
      pController->OnLoadingFilesToLoadIncrement(1);

      // Parse the playlist file and collect files from it
      std::list<string_t> files;
      util::LoadRhythmBoxPlaylistFile(loadingProcessInterface, files);

      // Tell the controller that we have finished loading this playlist file
      pController->OnLoadingFilesToLoadDecrement(1);

      // Add the tracks to the model
      AddTracks(files);
    }
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
    if (index < tracks.size()) {
      size_t i = 0;
      std::set<cTrack*>::iterator iter = tracks.begin();
      const std::set<cTrack*>::const_iterator iterEnd = tracks.end();
      while (iter != iterEnd) {
        if (i == index) {
          idLastPlayed = *iter;
          break;
        }

        i++;
        iter++;
      }
    }

    return idLastPlayed;
  }

  void cModel::SaveLastPlayed(trackid_t idLastPlayed) const
  {
    std::cout<<"cModel::SaveLastPlayed"<<std::endl;

    // Get the last played track index
    size_t index = INVALID_TRACK_INDEX;

    size_t i = 0;
    std::set<cTrack*>::const_iterator iter = tracks.begin();
    const std::set<cTrack*>::const_iterator iterEnd = tracks.end();
    while (iter != iterEnd) {
      if (*iter == idLastPlayed) {
        index = i;
        break;
      }

      i++;
      iter++;
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

  void cModel::StopLoading()
  {
    loadingProcessInterface.SetStop();

    // Remove all the remaining events
    ClearEventQueue();
  }

  void cModel::ClearEventQueue()
  {
    // Remove and delete all events on the queue
    while (true) {
      cModelEvent* pEvent = eventQueue.RemoveItemFromFront();
      if (pEvent == nullptr) break;

      spitfire::SAFE_DELETE(pEvent);
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

      //std::cout<<"cModel::ThreadFunction Loop getting event"<<std::endl;
      cModelEvent* pEvent = eventQueue.RemoveItemFromFront();
      if (pEvent != nullptr) {
        //std::cout<<"cModel::ThreadFunction Loop calling event function"<<std::endl;
        pEvent->EventFunction(*this);
        //std::cout<<"cModel::ThreadFunction Loop deleting event"<<std::endl;
        spitfire::SAFE_DELETE(pEvent);
      } else {
        // If the queue is empty then we know that there are no more actions and it is safe to reset our stop loading signal object
        loadingProcessInterface.Reset();
      }

      // Try to avoid hogging the CPU/
      spitfire::util::SleepThisThreadMS(1);
    }

    // Save the playlist on the background thread so that we can still process events on the main thread
    SavePlaylist();

    // Remove any further events because we don't care any more
    ClearEventQueue();
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
