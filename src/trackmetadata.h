#ifndef trackproperties_h
#define trackproperties_h

// Spitfire headers
#include <spitfire/audio/metadata.h>

// ** cTrackPropertiesReader

class cTrackPropertiesReader
{
public:
  bool ReadTrackProperties(spitfire::audio::cMetaData& properties, const spitfire::string_t& sFilePath) const;

private:
  bool ReadTrackTags(spitfire::audio::cMetaData& properties, const spitfire::string_t& sFilePath) const;
  bool ReadTrackLength(spitfire::audio::cMetaData& properties, const spitfire::string_t& sFilePath) const;
};

#endif // trackproperties_h
