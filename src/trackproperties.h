#ifndef trackproperties_h
#define trackproperties_h

#include <spitfire/util/cString.h>

// ** cTrackProperties

class cTrackProperties
{
public:
  cTrackProperties();

  uint64_t uiDurationMilliSeconds;
};


// ** cTrackPropertiesReader

class cTrackPropertiesReader
{
public:
  bool ReadTrackProperties(cTrackProperties& properties, const spitfire::string_t& sFilePath) const;

private:
  bool ReadTrackLength(cTrackProperties& properties, const spitfire::string_t& sFilePath) const;
};

#endif // trackproperties_h
