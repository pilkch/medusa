#ifndef trackproperties_h
#define trackproperties_h

// Spitfire headers
#include <spitfire/audio/metadata.h>

// Medusa headers
#include "medusa.h"

namespace medusa
{
// ** cTrackPropertiesReader

class cTrackPropertiesReader
{
public:
  bool ReadTrackProperties(spitfire::audio::cMetaData& properties, const string_t& sFilePath) const;

private:
  bool ReadTrackTags(spitfire::audio::cMetaData& properties, const string_t& sFilePath) const;
  bool ReadTrackLength(spitfire::audio::cMetaData& properties, const string_t& sFilePath) const;
};

// ** cTrackPropertiesWriter

class cTrackPropertiesWriter
{
public:
  bool WriteTrackProperties(const spitfire::audio::cMetaData& properties, const string_t& sFilePath) const;
  void ReadTrackProperties(spitfire::audio::cMetaData arg1, const char* arg2);

private:
  bool WriteTrackTags(const spitfire::audio::cMetaData& properties, const string_t& sFilePath) const;
};
}

#endif // trackproperties_h
