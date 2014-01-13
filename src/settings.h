#ifndef MEDUSA_SETTINGS_H
#define MEDUSA_SETTINGS_H

// Spitfire headers
#include <spitfire/storage/settings.h>
#include <spitfire/util/datetime.h>

// Medusa headers
#include "medusa.h"

namespace medusa
{
  // ** cLastFmStatistics

  struct cLastfmStatistics
  {
    cLastfmStatistics() :
      nTracksQueued(0),
      nTracksSubmitted(0)
    {
    }

    size_t nTracksQueued;
    size_t nTracksSubmitted;
    string_t sLastTrackArtistAndTitle;
    spitfire::util::cDateTime lastSubmittedDateTime;
  };


// ** cSettings

class cSettings
{
public:
  void Load();
  void Save();

  void GetMainWindowSize(size_t& width, size_t& height) const;
  void SetMainWindowSize(size_t width, size_t height);

  bool IsMainWindowMaximised() const;
  void SetMainWindowMaximised(bool bMaximised);

  bool IsShowMainWindow() const;
  void SetShowMainWindow(bool bShowMainWindow);

  string_t GetIgnoreUpdateVersion() const;
  void SetIgnoreUpdateVersion(const string_t& sVersion);

  bool IsPlaying() const;
  void SetPlaying(bool bPlaying);

  uint32_t GetVolume0To100() const;
  void SetVolume0To100(uint32_t uiVolume);

  bool IsRepeat() const;
  void SetRepeat(bool bRepeat);

  bool IsNotifyOnSongChange() const;
  void SetNotifyOnSongChange(bool bNotifyOnSongChange);

  bool IsNextSongOnMoveToTrash() const;
  void SetNextSongOnMoveToTrash(bool bNextSongOnMoveToTrash);

  bool IsLastFMEnabled() const;
  void SetLastFMEnabled(bool bEnabled);

  string_t GetLastFMUserName() const;
  void SetLastFMUserName(const string_t& sUserName);

  string_t GetLastFMPassword() const;
  void SetLastFMPassword(const string_t& sPassword);

  cLastfmStatistics GetLastFmStatistics() const;
  void SetLastFmStatistics(const cLastfmStatistics& statistics);

  string_t GetLastAddLocation() const;
  void SetLastAddLocation(const string_t& sLastAddLocation);

  string_t GetLastMoveToFolderLocation() const;
  void SetLastMoveToFolderLocation(const string_t& sLastMoveToFolderLocation);

  void GetRecentMoveToFolders(std::vector<string_t>& folders) const;
  void SetRecentMoveToFolders(const std::vector<string_t>& folders);

  bool IsWebServerEnabled() const;
  void SetWebServerEnabled(bool bWebServerEnabled);

private:
  spitfire::storage::cSettingsDocument document;
};
}

#endif // MEDUSA_SETTINGS_H
