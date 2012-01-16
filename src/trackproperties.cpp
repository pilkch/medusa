// Standard headers
#include <cstdio>
#include <iostream>
#include <memory.h>

// Unix headers
#include <sys/stat.h>

// libid3tag headers
#include <id3tag.h>

// libmad headers
#include <mad.h>

// Meduasa headers
#include "trackproperties.h"

// ** cTrackProperties

cTrackProperties::cTrackProperties() :
  uiYear(0),
  uiTracknum(0),
  uiDurationMilliSeconds(0)
{
}

void cTrackProperties::Clear()
{
  sArtist.clear();
  sCompilationArtist.clear();
  sAlbum.clear();
  sTitle.clear();
  sGenre.clear();
  sComment.clear();
  uiYear = 0;
  uiTracknum = 0;
  uiDurationMilliSeconds = 0;
}


#define MYTH_ID3_FRAME_COMPILATIONARTIST "TPE4"
#define MYTH_ID3_FRAME_COMMENT "TXXX"
#define MYTH_ID3_FRAME_MUSICBRAINZ_ALBUMARTISTDESC "MusicBrainz Album Artist Id"
#define MYTH_MUSICBRAINZ_ALBUMARTIST_UUID "89ad4ac3-39f7-470e-963a-56509c546377"

// ** cLibID3Tag
//
// Use libid3tag to read the tag data
// http://mythaudio.googlecode.com/svn/trunk/mythaudio/metaioid3v2.h
// http://mythaudio.googlecode.com/svn/trunk/mythaudio/metaioid3v2.cpp
// http://mythaudio.googlecode.com/svn/trunk/mythaudio/metaio.h
// http://mythaudio.googlecode.com/svn/trunk/mythaudio/metaio.cpp
//
class cLibID3Tag
{
public:
  bool ReadTrackTags(cTrackProperties& properties, const spitfire::string_t& sFilePath) const;

private:
  bool IsNumbersOnly(const spitfire::string_t& sText) const;

  spitfire::string_t GetTag(id3_tag* pTag, const char* pLabel, const spitfire::string_t& desc = TEXT("")) const;
  spitfire::string_t GetRawID3String(union id3_field* pField) const;
};

bool cLibID3Tag::IsNumbersOnly(const spitfire::string_t& sText) const
{
  const size_t n = sText.length();
  for (size_t i = 0; i < n; i++) {
    if (!isdigit(sText[i])) return false;
  }

  return true;
}

bool cLibID3Tag::ReadTrackTags(cTrackProperties& properties, const spitfire::string_t& sFilePath) const
{
  properties.Clear();

  id3_file* p_input = id3_file_open(spitfire::string::ToUTF8(sFilePath).c_str(), ID3_FILE_MODE_READONLY);
  if (p_input == nullptr) p_input = id3_file_open(spitfire::string::ToASCII(sFilePath).c_str(), ID3_FILE_MODE_READONLY);

  if (p_input != nullptr) {
    id3_tag* tag = id3_file_tag(p_input);
    if (tag == nullptr) {
      id3_file_close(p_input);
      return false;
    }

    properties.sTitle = GetTag(tag, ID3_FRAME_TITLE);
    properties.sArtist = GetTag(tag, ID3_FRAME_ARTIST);
    properties.sCompilationArtist = GetTag(tag, MYTH_ID3_FRAME_COMPILATIONARTIST);
    properties.sAlbum = GetTag(tag, ID3_FRAME_ALBUM);

    // Get Track Num dealing with 1/16, 2/16 etc. format
    properties.uiTracknum = spitfire::string::ToUnsignedInt(GetTag(tag, ID3_FRAME_TRACK));

    // NB Year could be TDRC or TYER depending on version....
    // From: http://www.id3.org/id3v2.4.0-structure.txt
    // Time stamps can be: yyyy, yyyy-MM, yyyy-MM-dd, yyyy-MM-ddTHH,
    //                     yyyy-MM-ddTHH:mm and yyyy-MM-ddTHH:mm:ss
    // Basically all starting yyyy.

    // Depending on the version of libid3tag, it will reassign a #define,
    // but we want to look for both.
    properties.uiYear = spitfire::string::ToUnsignedInt(GetTag(tag, ID3_FRAME_YEAR));
    if (0 == properties.uiYear) properties.uiYear = spitfire::string::ToUnsignedInt(GetTag(tag, "TYER"));

    // Genre
    properties.sGenre = GetTag(tag, ID3_FRAME_GENRE);

    // Genre in ID3v2 = "genrenum|Genre Name"
    if (IsNumbersOnly(properties.sGenre)) {
      // This means the genre is 100% numeric
      // Try and decode genre number
      const std::string sGenreUTF8(spitfire::string::ToUTF8(properties.sGenre));
      id3_ucs4_t* p_tmp = id3_utf8_ucs4duplicate((const id3_utf8_t*)(const char*)sGenreUTF8.c_str());

      const id3_ucs4_t* p_ucs4 = id3_genre_name(p_tmp);
      free(p_tmp);

      id3_utf8_t* p_utf8 = id3_ucs4_utf8duplicate(p_ucs4);
      properties.sGenre = spitfire::string::ToString_t((const char*)p_utf8);

      free(p_utf8);
    }

    // Comment
    properties.sComment = GetTag(tag, ID3_FRAME_COMMENT);

    id3_file_close(p_input);
  }

  // If we don't have title and artist return false
  if (properties.sTitle.empty() && properties.sArtist.empty()) {
    std::wcerr<<"cLibID3Tag::ReadTrackTags FAILED to read metadata from \""<<sFilePath<<"\"\n";
    return false;
  }

  //std::wcout<<"artist: \""<<properties.sArtist<<"\"\n";
  //std::wcout<<"compilation_artist: \""<<properties.sCompilationArtist<<"\"\n";
  //std::wcout<<"album: \""<<properties.sAlbum<<"\"\n";
  //std::wcout<<"title: \""<<properties.sTitle<<"\"\n";
  //std::wcout<<"genre: \""<<properties.sGenre<<"\"\n";
  //std::wcout<<"comment: \""<<properties.sComment<<"\"\n";
  //std::wcout<<"year: \""<<properties.uiYear<<"\"\n";
  //std::wcout<<"tracknum: \""<<properties.uiTracknum<<"\"\n";

  return true;
}

spitfire::string_t cLibID3Tag::GetRawID3String(union id3_field* pField) const
{
  spitfire::string_t tmp = TEXT("");

  const id3_ucs4_t* p_ucs4 = (const id3_ucs4_t*) id3_field_getstring(pField);

  if (p_ucs4 != nullptr) {
    id3_utf8_t* p_utf8 = id3_ucs4_utf8duplicate(p_ucs4);

    if (p_utf8 == nullptr) return TEXT("");

    tmp = spitfire::string::ToString_t((const char*)p_utf8);

    free(p_utf8);
  } else {
    unsigned int nstrings = id3_field_getnstrings(pField);

    for (unsigned int j=0; j<nstrings; ++j) {
      p_ucs4 = id3_field_getstrings(pField, j);

      if (p_ucs4 == nullptr) break;

      id3_utf8_t* p_utf8 = id3_ucs4_utf8duplicate(p_ucs4);

      if (p_utf8 == nullptr) break;

      tmp += spitfire::string::ToString_t((const char*)p_utf8);

      free(p_utf8);
    }
  }

  return tmp;
}

spitfire::string_t cLibID3Tag::GetTag(id3_tag* pTag, const char* pLabel, const spitfire::string_t& desc) const
{
  if (pLabel == nullptr) return TEXT("");

  struct id3_frame* p_frame = nullptr;

  for (int i = 0; nullptr != (p_frame = id3_tag_findframe(pTag, pLabel, i)); ++i) {
    int field_num = 1;

    spitfire::string_t tmp = TEXT("");

    // Compare the first field with the description if supplied
    if (!desc.empty()) {
      tmp = GetRawID3String(&p_frame->fields[field_num++]);

      // Now compare tmp to desc
      if (tmp != desc) {
        // No match - move on.
        continue;
      }
    }

    // Get the value and return it.
    tmp = GetRawID3String(&p_frame->fields[field_num]);

    return tmp;
  }

  // Not found.
  return TEXT("");
}



// ** cTrackPropertiesReader

bool cTrackPropertiesReader::ReadTrackProperties(cTrackProperties& properties, const spitfire::string_t& sFilePath) const
{
  properties.Clear();

  bool bResult = true;

  if (!ReadTrackTags(properties, sFilePath)) bResult = false;
  if (!ReadTrackLength(properties, sFilePath)) bResult = false;

  return bResult;
}

bool cTrackPropertiesReader::ReadTrackTags(cTrackProperties& properties, const spitfire::string_t& sFilePath) const
{
  cLibID3Tag libID3Tag;
  return libID3Tag.ReadTrackTags(properties, sFilePath);
}

// Use libmad to read the track length
// http://mythaudio.googlecode.com/svn/trunk/mythaudio/metaioid3v2.cpp

bool cTrackPropertiesReader::ReadTrackLength(cTrackProperties& properties, const spitfire::string_t& sFilePath) const
{
  // TODO: CLEAN UP THIS FUNCTION

  properties.uiDurationMilliSeconds = 0;

  const std::string sFileNameUTF8 = spitfire::string::ToUTF8(sFilePath);
  FILE* file = fopen(sFileNameUTF8.c_str(), "r");
  if (file == nullptr) return false;

  struct stat s;
  fstat(fileno(file), &s);

  struct mad_stream stream;
  mad_stream_init(&stream);

  struct mad_header header;
  mad_header_init(&header);

  unsigned long old_bitrate = 0;
  bool bIsVBR = false;
  int amount_checked = 0;
  int alt_length = 0;

  unsigned char buffer[8192];
  unsigned int buflen = 0;

  mad_timer_t timer = mad_timer_zero;

  bool bContinue = true;
  while (bContinue) {
    if (buflen < sizeof(buffer)) {
      int bytes = fread(buffer + buflen, 1, sizeof(buffer) - buflen, file);
      if (bytes <= 0) break;
      buflen += bytes;
    }

    mad_stream_buffer(&stream, buffer, buflen);

    while (true) {
      if (mad_header_decode(&header, &stream) == -1) {
        if (!MAD_RECOVERABLE(stream.error)) break;

        if (stream.error == MAD_ERROR_LOSTSYNC) {
          int tagsize = id3_tag_query(stream.this_frame, stream.bufend - stream.this_frame);
          if (tagsize > 0) {
            mad_stream_skip(&stream, tagsize);
            s.st_size -= tagsize;
          }
        }
      } else {
        if (amount_checked == 0) old_bitrate = header.bitrate;
        else if (header.bitrate != old_bitrate) bIsVBR = true;

        if ((amount_checked == 32) && !bIsVBR) {
          alt_length = (s.st_size * 8) / (old_bitrate / 1000);
          bContinue = false;
          break;
        }
        amount_checked++;
        mad_timer_add(&timer, header.duration);
      }
    }

    if (stream.error != MAD_ERROR_BUFLEN) break;

    memmove(buffer, stream.next_frame, &buffer[buflen] - stream.next_frame);
    buflen -= stream.next_frame - &buffer[0];
  }

  mad_header_finish(&header);
  mad_stream_finish(&stream);

  fclose(file);

  if (bIsVBR) properties.uiDurationMilliSeconds = mad_timer_count(timer, MAD_UNITS_MILLISECONDS);
  else properties.uiDurationMilliSeconds = alt_length;

  return true;
}
