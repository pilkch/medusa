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

#include <spitfire/storage/filesystem.h>
#include <spitfire/storage/xml.h>

// Medusa headers
#include "settings.h"

void cSettings::Load()
{
  // Read the xml document
  spitfire::xml::reader reader;

  const spitfire::string_t sFilename = spitfire::filesystem::GetThisApplicationSettingsDirectory() + TEXT("config.xml");
  if (!reader.ReadFromFile(document, sFilename)) {
    std::cout<<"cSettings::Load \""<<spitfire::string::ToUTF8(sFilename)<<"\" not found"<<std::endl;
    return;
  }
}

void cSettings::Save()
{
  // Create the directory
  const spitfire::string_t sFolder = spitfire::filesystem::GetThisApplicationSettingsDirectory();
  spitfire::filesystem::CreateDirectory(sFolder);

  // Write the xml document
  spitfire::xml::writer writer;

  const spitfire::string_t sFilename = sFolder + TEXT("config.xml");
  if (!writer.WriteToFile(document, sFilename)) {
    std::cout<<"cSettings::Save Error saving to file \""<<spitfire::string::ToUTF8(sFilename)<<"\""<<std::endl;
    return;
  }
}

template <class T>
T cSettings::GetXMLValue(const spitfire::string_t& sSection, const spitfire::string_t& sItem, const spitfire::string_t& sAttribute, const T& valueDefault) const
{
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
void cSettings::SetXMLValue(const spitfire::string_t& sSection, const spitfire::string_t& sItem, const spitfire::string_t& sAttribute, const T& value)
{
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

uint32_t cSettings::GetVolume0To100() const
{
  return GetXMLValue(TEXT("settings"), TEXT("playback"), TEXT("volume"), 100);
}

void cSettings::SetVolume0To100(uint32_t uiVolume)
{
  SetXMLValue(TEXT("settings"), TEXT("playback"), TEXT("volume"), uiVolume);
}
