// Standard headers
#include <cassert>
#include <cmath>
#include <cstring>

#include <string>
#include <iostream>
#include <sstream>

#include <algorithm>
#include <map>
#include <vector>
#include <list>

// Spitfire headers
#include <spitfire/storage/filesystem.h>

// Medusa headers
#include "settings.h"

namespace medusa
{
  void cSettings::Load()
  {
    document.Save();
  }

  void cSettings::Save()
  {
    document.Save();
  }

bool cSettings::IsShowMainWindow() const
{
  return document.GetValue(TEXT("settings"), TEXT("window"), TEXT("showMainWindow"), true);
}

void cSettings::SetShowMainWindow(bool bShowMainWindow)
{
  document.SetValue(TEXT("settings"), TEXT("window"), TEXT("showMainWindow"), bShowMainWindow);
}

string_t cSettings::GetIgnoreUpdateVersion() const
{
  return document.GetValue<string_t>(TEXT("settings"), TEXT("update"), TEXT("ignoredVersion"), "");
}

void cSettings::SetIgnoreUpdateVersion(const string_t& sVersion)
{
  document.SetValue(TEXT("settings"), TEXT("update"), TEXT("ignoredVersion"), sVersion);
}

bool cSettings::IsPlaying() const
{
  return document.GetValue(TEXT("settings"), TEXT("playback"), TEXT("playing"), true);
}

void cSettings::SetPlaying(bool bPlaying)
{
  document.SetValue(TEXT("settings"), TEXT("playback"), TEXT("playing"), bPlaying);
}

uint32_t cSettings::GetVolume0To100() const
{
  return document.GetValue(TEXT("settings"), TEXT("playback"), TEXT("volume"), 100);
}

void cSettings::SetVolume0To100(uint32_t uiVolume)
{
  document.SetValue(TEXT("settings"), TEXT("playback"), TEXT("volume"), uiVolume);
}

bool cSettings::IsRepeat() const
{
  return document.GetValue(TEXT("settings"), TEXT("window"), TEXT("repeat"), true);
}

void cSettings::SetRepeat(bool bRepeat)
{
  document.SetValue(TEXT("settings"), TEXT("window"), TEXT("repeat"), bRepeat);
}

bool cSettings::IsNotifyOnSongChange() const
{
  return document.GetValue(TEXT("settings"), TEXT("window"), TEXT("notifyOnSongChange"), true);
}

void cSettings::SetNotifyOnSongChange(bool bNotifyOnSongChange)
{
  document.SetValue(TEXT("settings"), TEXT("window"), TEXT("notifyOnSongChange"), bNotifyOnSongChange);
}

bool cSettings::IsNextSongOnMoveToTrash() const
{
  return document.GetValue(TEXT("settings"), TEXT("playback"), TEXT("nextSongOnMoveToTrash"), true);
}

void cSettings::SetNextSongOnMoveToTrash(bool bNextSongOnMoveToTrash)
{
  document.SetValue(TEXT("settings"), TEXT("window"), TEXT("nextSongOnMoveToTrash"), bNextSongOnMoveToTrash);
}

bool cSettings::IsLastFMEnabled() const
{
  return document.GetValue(TEXT("settings"), TEXT("lastfm"), TEXT("enabled"), false);
}

void cSettings::SetLastFMEnabled(bool bEnabled)
{
  document.SetValue(TEXT("settings"), TEXT("lastfm"), TEXT("enabled"), bEnabled);
}

string_t cSettings::GetLastFMUserName() const
{
  return document.GetValue<string_t>(TEXT("settings"), TEXT("lastfm"), TEXT("username"), TEXT(""));
}

void cSettings::SetLastFMUserName(const string_t& sUserName)
{
  document.SetValue(TEXT("settings"), TEXT("lastfm"), TEXT("username"), sUserName);
}

string_t cSettings::GetLastFMPassword() const
{
  return document.GetValue<string_t>(TEXT("settings"), TEXT("lastfm"), TEXT("password"), TEXT(""));
}

void cSettings::SetLastFMPassword(const string_t& sPassword)
{
  document.SetValue(TEXT("settings"), TEXT("lastfm"), TEXT("password"), sPassword);
}

  string_t cSettings::GetLastAddLocation() const
  {
    return document.GetValue<string_t>(TEXT("settings"), TEXT("path"), TEXT("lastAddLocation"), spitfire::filesystem::GetHomeMusicDirectory());
  }

  void cSettings::SetLastAddLocation(const string_t& sLastAddLocation)
  {
    document.SetValue(TEXT("settings"), TEXT("path"), TEXT("lastAddLocation"), sLastAddLocation);
  }

  string_t cSettings::GetLastMoveToFolderLocation() const
  {
    return document.GetValue<string_t>(TEXT("settings"), TEXT("path"), TEXT("lastMoveToFolderLocation"), spitfire::filesystem::GetHomeMusicDirectory());
  }

  void cSettings::SetLastMoveToFolderLocation(const string_t& sLastMoveToFolderLocation)
  {
    document.SetValue(TEXT("settings"), TEXT("path"), TEXT("lastMoveToFolderLocation"), sLastMoveToFolderLocation);
  }

  void cSettings::GetRecentMoveToFolders(std::vector<string_t>& folders) const
  {
    folders.clear();

    // Get the count
    const size_t n = document.GetValue(TEXT("settings"), TEXT("path"), TEXT("recentMoveToFolderLocationCount"), 0);

    // Get each path
    string_t sValue;
    for (size_t i = 0; i < n; i++) {
      sValue = document.GetValue<string_t>(TEXT("settings"), TEXT("path"), TEXT("recentMoveToFolderLocation") + spitfire::string::ToString(i), TEXT(""));
      if (!sValue.empty()) folders.push_back(sValue);
    }
  }

  void cSettings::SetRecentMoveToFolders(const std::vector<string_t>& folders)
  {
    // Set the count
    const size_t n = folders.size();
    document.SetValue(TEXT("settings"), TEXT("path"), TEXT("recentMoveToFolderLocationCount"), n);

    // Add each path
    for (size_t i = 0; i < n; i++) {
      document.SetValue(TEXT("settings"), TEXT("path"), TEXT("recentMoveToFolderLocation") + spitfire::string::ToString(i), folders[i]);
    }
  }

  bool cSettings::IsWebServerEnabled() const
  {
    return document.GetValue(TEXT("settings"), TEXT("webserver"), TEXT("enabled"), true);
  }

  void cSettings::SetWebServerEnabled(bool bWebServerEnabled)
  {
    document.SetValue(TEXT("settings"), TEXT("webserver"), TEXT("enabled"), bWebServerEnabled);
  }

}
