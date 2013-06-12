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
#include <spitfire/spitfire.h>

#include <spitfire/util/thread.h>
#include <spitfire/storage/filesystem.h>
#include <spitfire/storage/xml.h>

// Medusa headers
#include "settings.h"

namespace medusa
{
void cSettings::Load()
{
  assert(spitfire::util::IsMainThread());

  // Read the xml document
  spitfire::xml::reader reader;

  spitfire::util::cProcessInterfaceVoid interface;
  const string_t sFilename = spitfire::filesystem::GetThisApplicationSettingsDirectory() + TEXT("config.xml");
  spitfire::util::PROCESS_RESULT result = reader.ReadFromFile(interface, document, sFilename);
  if (result != spitfire::util::PROCESS_RESULT::COMPLETE) std::cout<<"cSettings::Load \""<<spitfire::string::ToUTF8(sFilename)<<"\" not found"<<std::endl;
}

void cSettings::Save()
{
  assert(spitfire::util::IsMainThread());

  // Create the directory
  const string_t sFolder = spitfire::filesystem::GetThisApplicationSettingsDirectory();
  spitfire::filesystem::CreateDirectory(sFolder);

  // Write the xml document
  spitfire::xml::writer writer;

  const string_t sFilename = sFolder + TEXT("config.xml");
  if (!writer.WriteToFile(document, sFilename)) {
    std::cout<<"cSettings::Save Error saving to file \""<<spitfire::string::ToUTF8(sFilename)<<"\""<<std::endl;
    return;
  }
}

template <class T>
T cSettings::GetXMLValue(const string_t& sSection, const string_t& sItem, const string_t& sAttribute, const T& valueDefault) const
{
  assert(spitfire::util::IsMainThread());

  T value = valueDefault;

  spitfire::document::cNode::const_iterator iterConfig(document);
  if (!iterConfig.IsValid()) return value;

  iterConfig.FindChild("config");
  if (!iterConfig.IsValid()) return value;

  {
    spitfire::document::cNode::const_iterator iter(iterConfig);

    iter.FindChild(spitfire::string::ToUTF8(sSection));
    if (iter.IsValid()) {
      iter.FindChild(spitfire::string::ToUTF8(sItem));
      if (iter.IsValid()) {
        iter.GetAttribute(spitfire::string::ToUTF8(sAttribute), value);
        //std::cout<<"cSettings::GetXMLValue Item \""<<sItem<<"\" found "<<spitfire::string::ToString(value)<<std::endl;
      }
    }
  }

  return value;
}

template <class T>
void cSettings::SetXMLValue(const string_t& sSection, const string_t& sItem, const string_t& sAttribute, const T& value)
{
  assert(spitfire::util::IsMainThread());

  // Get or create the config element
  spitfire::document::cNode::iterator iterConfig(document);
  if (!iterConfig.IsValid()) {
    spitfire::document::element* configElement = document.CreateElement("config");
    document.AppendChild(configElement);
    iterConfig = document;
    assert(iterConfig.IsValid());
  }

  // Get or create the config element
  iterConfig.FindChild("config");
  if (!iterConfig.IsValid()) {
    spitfire::document::element* configElement = document.CreateElement("config");
    document.AppendChild(configElement);
    iterConfig = document;
    assert(iterConfig.IsValid());
    iterConfig.FindChild("config");
    assert(iterConfig.IsValid());
  }

  // Get or create the section element
  spitfire::document::cNode::iterator iterSection(iterConfig);
  iterSection.FindChild(spitfire::string::ToUTF8(sSection));
  if (!iterSection.IsValid()) {
    spitfire::document::element* sectionElement = document.CreateElement(spitfire::string::ToUTF8(sSection));
    spitfire::document::element* configElement = iterConfig.Get();
    configElement->AppendChild(sectionElement);
    iterSection = iterConfig;
    assert(iterSection.IsValid());
    iterSection.FindChild(spitfire::string::ToUTF8(sSection));
    assert(iterSection.IsValid());
  }

  // Get or create the item element
  spitfire::document::cNode::iterator iterItem(iterSection);
  iterItem.FindChild(spitfire::string::ToUTF8(sItem));
  if (!iterItem.IsValid()) {
    spitfire::document::element* itemElement = document.CreateElement(spitfire::string::ToUTF8(sItem));
    spitfire::document::element* sectionElement = iterSection.Get();
    sectionElement->AppendChild(itemElement);
    iterItem = iterSection;
    assert(iterItem.IsValid());
    iterItem.FindChild(spitfire::string::ToUTF8(sItem));
    assert(iterItem.IsValid());
  }

  spitfire::document::element* itemElement = iterItem.Get();

  // Create and append the item element
  itemElement->SetAttribute(spitfire::string::ToUTF8(sAttribute), value);
}

bool cSettings::IsShowMainWindow() const
{
  return GetXMLValue(TEXT("settings"), TEXT("window"), TEXT("showMainWindow"), true);
}

void cSettings::SetShowMainWindow(bool bShowMainWindow)
{
  SetXMLValue(TEXT("settings"), TEXT("window"), TEXT("showMainWindow"), bShowMainWindow);
}

string_t cSettings::GetIgnoreUpdateVersion() const
{
  return GetXMLValue<string_t>(TEXT("settings"), TEXT("update"), TEXT("ignoredVersion"), "");
}

void cSettings::SetIgnoreUpdateVersion(const string_t& sVersion)
{
  SetXMLValue(TEXT("settings"), TEXT("update"), TEXT("ignoredVersion"), sVersion);
}

bool cSettings::IsPlaying() const
{
  return GetXMLValue(TEXT("settings"), TEXT("playback"), TEXT("playing"), true);
}

void cSettings::SetPlaying(bool bPlaying)
{
  SetXMLValue(TEXT("settings"), TEXT("playback"), TEXT("playing"), bPlaying);
}

uint32_t cSettings::GetVolume0To100() const
{
  return GetXMLValue(TEXT("settings"), TEXT("playback"), TEXT("volume"), 100);
}

void cSettings::SetVolume0To100(uint32_t uiVolume)
{
  SetXMLValue(TEXT("settings"), TEXT("playback"), TEXT("volume"), uiVolume);
}

bool cSettings::IsRepeat() const
{
  return GetXMLValue(TEXT("settings"), TEXT("window"), TEXT("repeat"), true);
}

void cSettings::SetRepeat(bool bRepeat)
{
  SetXMLValue(TEXT("settings"), TEXT("window"), TEXT("repeat"), bRepeat);
}

bool cSettings::IsNotifyOnSongChange() const
{
  return GetXMLValue(TEXT("settings"), TEXT("window"), TEXT("notifyOnSongChange"), true);
}

void cSettings::SetNotifyOnSongChange(bool bNotifyOnSongChange)
{
  SetXMLValue(TEXT("settings"), TEXT("window"), TEXT("notifyOnSongChange"), bNotifyOnSongChange);
}

bool cSettings::IsNextSongOnMoveToTrash() const
{
  return GetXMLValue(TEXT("settings"), TEXT("playback"), TEXT("nextSongOnMoveToTrash"), true);
}

void cSettings::SetNextSongOnMoveToTrash(bool bNextSongOnMoveToTrash)
{
  SetXMLValue(TEXT("settings"), TEXT("window"), TEXT("nextSongOnMoveToTrash"), bNextSongOnMoveToTrash);
}

bool cSettings::IsLastFMEnabled() const
{
  return GetXMLValue(TEXT("settings"), TEXT("lastfm"), TEXT("enabled"), false);
}

void cSettings::SetLastFMEnabled(bool bEnabled)
{
  SetXMLValue(TEXT("settings"), TEXT("lastfm"), TEXT("enabled"), bEnabled);
}

string_t cSettings::GetLastFMUserName() const
{
  return GetXMLValue<string_t>(TEXT("settings"), TEXT("lastfm"), TEXT("username"), TEXT(""));
}

void cSettings::SetLastFMUserName(const string_t& sUserName)
{
  SetXMLValue(TEXT("settings"), TEXT("lastfm"), TEXT("username"), sUserName);
}

string_t cSettings::GetLastFMPassword() const
{
  return GetXMLValue<string_t>(TEXT("settings"), TEXT("lastfm"), TEXT("password"), TEXT(""));
}

void cSettings::SetLastFMPassword(const string_t& sPassword)
{
  SetXMLValue(TEXT("settings"), TEXT("lastfm"), TEXT("password"), sPassword);
}

  string_t cSettings::GetLastAddLocation() const
  {
    return GetXMLValue<string_t>(TEXT("settings"), TEXT("path"), TEXT("lastAddLocation"), spitfire::filesystem::GetHomeMusicDirectory());
  }

  void cSettings::SetLastAddLocation(const string_t& sLastAddLocation)
  {
    SetXMLValue(TEXT("settings"), TEXT("path"), TEXT("lastAddLocation"), sLastAddLocation);
  }

  string_t cSettings::GetLastMoveToFolderLocation() const
  {
    return GetXMLValue<string_t>(TEXT("settings"), TEXT("path"), TEXT("lastMoveToFolderLocation"), spitfire::filesystem::GetHomeMusicDirectory());
  }

  void cSettings::SetLastMoveToFolderLocation(const string_t& sLastMoveToFolderLocation)
  {
    SetXMLValue(TEXT("settings"), TEXT("path"), TEXT("lastMoveToFolderLocation"), sLastMoveToFolderLocation);
  }

  void cSettings::GetRecentMoveToFolders(std::vector<string_t>& folders) const
  {
    folders.clear();

    // Get the count
    const size_t n = GetXMLValue(TEXT("settings"), TEXT("path"), TEXT("recentMoveToFolderLocationCount"), 0);

    // Get each path
    string_t sValue;
    for (size_t i = 0; i < n; i++) {
      sValue = GetXMLValue<string_t>(TEXT("settings"), TEXT("path"), TEXT("recentMoveToFolderLocation") + spitfire::string::ToString(i), TEXT(""));
      if (!sValue.empty()) folders.push_back(sValue);
    }
  }

  void cSettings::SetRecentMoveToFolders(const std::vector<string_t>& folders)
  {
    // Set the count
    const size_t n = folders.size();
    SetXMLValue(TEXT("settings"), TEXT("path"), TEXT("recentMoveToFolderLocationCount"), n);

    // Add each path
    for (size_t i = 0; i < n; i++) {
      SetXMLValue(TEXT("settings"), TEXT("path"), TEXT("recentMoveToFolderLocation") + spitfire::string::ToString(i), folders[i]);
    }
  }
}
