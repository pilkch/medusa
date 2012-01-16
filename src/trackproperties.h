#ifndef trackproperties_h
#define trackproperties_h

#include <spitfire/util/cString.h>

// ** cTrackProperties

class cTrackProperties
{
public:
  cTrackProperties();

  void Clear();

  spitfire::string_t sArtist;
  spitfire::string_t sCompilationArtist;
  spitfire::string_t sAlbum;
  spitfire::string_t sTitle;
  spitfire::string_t sGenre;
  spitfire::string_t sComment;
  uint64_t uiYear;
  uint64_t uiTracknum;
  uint64_t uiDurationMilliSeconds;
};


// ** cTrackPropertiesReader

class cTrackPropertiesReader
{
public:
  bool ReadTrackProperties(cTrackProperties& properties, const spitfire::string_t& sFilePath) const;

private:
  bool ReadTrackTags(cTrackProperties& properties, const spitfire::string_t& sFilePath) const;
  bool ReadTrackLength(cTrackProperties& properties, const spitfire::string_t& sFilePath) const;
};

#endif // trackproperties_h
