#ifndef MEDUSA_SETTINGS_H
#define MEDUSA_SETTINGS_H

// Spitfire headers
#include <spitfire/storage/document.h>

// Medusa headers
#include "medusa.h"

namespace medusa
{
// ** cSettings

class cSettings
{
public:
  void Load();
  void Save();

  bool IsShowMainWindow() const;
  void SetShowMainWindow(bool bShowMainWindow);

  bool IsPlaying() const;
  void SetPlaying(bool bPlaying);

  uint32_t GetVolume0To100() const;
  void SetVolume0To100(uint32_t uiVolume);

  bool IsRepeat() const;
  void SetRepeat(bool bRepeat);

  bool IsNotifyOnSongChange() const;
  void SetNotifyOnSongChange(bool bNotifyOnSongChange);

  bool IsLastFMEnabled() const;
  void SetLastFMEnabled(bool bEnabled);

  string_t GetLastFMUserName() const;
  void SetLastFMUserName(const string_t& sUserName);

  string_t GetLastFMPassword() const;
  void SetLastFMPassword(const string_t& sPassword);

  string_t GetLastAddLocation();
  void SetLastAddLocation(const string_t& sLastAddLocation);

  string_t GetLastMoveToFolderLocation() const;
  void SetLastMoveToFolderLocation(const string_t& sLastMoveToFolderLocation);

  void GetRecentMoveToFolders(std::vector<string_t>& folders) const;
  void SetRecentMoveToFolders(const std::vector<string_t>& folders);

private:
  template <class T>
  T GetXMLValue(const string_t& sSection, const string_t& sItem, const string_t& sAttribute, const T& valueDefault) const;
  template <class T>
  void SetXMLValue(const string_t& sSection, const string_t& sItem, const string_t& sAttribute, const T& value);

  spitfire::document::cDocument document;
};
}

#endif // MEDUSA_SETTINGS_H
