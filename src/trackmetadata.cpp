// Standard headers
#include <cstdio>
#include <iostream>
#include <memory.h>
#include <unistd.h>

// Unix headers
#include <sys/stat.h>

// libid3tag headers
#include <id3tag.h>

// libmad headers
#include <mad.h>

// Medusa headers
#include "trackmetadata.h"

// Spitfire headers
#include <spitfire/storage/filesystem.h>

struct id3_file {
  FILE *iofile;
  enum id3_file_mode mode;
  char *path;

  int flags;

  struct id3_tag *primary;

  unsigned int ntags;
  struct filetag *tags;
};

struct filetag {
  struct id3_tag *tag;
  unsigned long location;
  id3_length_t length;
};

namespace medusa
{
// ** cLibID3Tag
//
// Use libid3tag to read the tag data
// http://wiki.hydrogenaudio.org/index.php?title=Foobar2000:ID3_Tag_Mapping
// http://mythaudio.googlecode.com/svn/trunk/mythaudio/metaioid3v2.h
// http://mythaudio.googlecode.com/svn/trunk/mythaudio/metaioid3v2.cpp
// http://mythaudio.googlecode.com/svn/trunk/mythaudio/metaio.h
// http://mythaudio.googlecode.com/svn/trunk/mythaudio/metaio.cpp
//

#define MYTH_ID3_FRAME_ALBUMARTIST "TPE2"
#define MYTH_ID3_FRAME_COMPILATIONARTIST "TPE4"
#define MYTH_ID3_FRAME_YEAR "TYER"
#define MYTH_ID3_FRAME_COMMENT "TXXX"


#define BUILD_ID3_FILE_UPDATE_HACK

#ifdef BUILD_ID3_FILE_UPDATE_HACK

// The id3_file_update function has a feature deficit
// http://mythaudio.googlecode.com/svn/trunk/mythaudio/metaio_libid3hack.h
// http://mythaudio.googlecode.com/svn/trunk/mythaudio/metaio_libid3hack.c

#define ID3_FILE_UPDATE myth_id3_file_update

enum {
  ID3_FILE_FLAG_ID3V1 = 0x0001
};


/*
 * NAME: v2_write()
 * DESCRIPTION:  write ID3v2 tag modifications to a file
 */
static
int myth_v2_write(struct id3_file *file,
                  id3_byte_t const *data, id3_length_t length)
{
  int result = 0;
  int file_size, overlap;
  char *buffer = NULL;
  int buffersize;

  int bytesread1, bytesread2, bytesread_tmp;
  char *ring1 = NULL, *ring2 = NULL, *ring_tmp = NULL;

  assert(!data || length > 0);

  if (!data
      || (!(file->ntags == 1 && !(file->flags & ID3_FILE_FLAG_ID3V1))
          && !(file->ntags == 2 &&  (file->flags & ID3_FILE_FLAG_ID3V1)))) {
    /* no v2 tag. we should create one */

    /* ... */

    goto done;
  }

  if (file->tags[0].length == length) {
    /* easy special case: rewrite existing tag in-place */

    if (fseek(file->iofile, file->tags[0].location, SEEK_SET) == -1 ||
        fwrite(data, length, 1, file->iofile) != 1 ||
        fflush(file->iofile) == EOF)
      return -1;

    goto done;

  }

  /* the new tag has a different size */

  /* calculate the difference in tag sizes.
   * we'll need at least double this difference to write the file again using
   * optimal memory allocation, but avoiding a temporary file.
   */
  overlap = length - file->tags[0].length;

  if (overlap > 0) {
    buffersize = overlap*2;
    buffer = (char*)malloc(buffersize);
    if (!buffer)
      return -1;

    ring1 = buffer;
    ring2 = &buffer[overlap];
  } else {
    /* let's use a 100kB buffer */
    buffersize = 100*1024;
    buffer = (char*)malloc(buffersize);
  }

  /* find out the filesize */
  fseek(file->iofile, 0, SEEK_END);
  file_size = ftell(file->iofile);

  /* Seek to start of data */
  if (-1 == fseek(file->iofile, file->tags[0].location + file->tags[0].length,
                  SEEK_SET))
    goto fail;

  /* fill our buffer, if needed */
  if (overlap > 0) {
    if (1 != fread(buffer, buffersize, 1, file->iofile))
      goto fail;
  }

  /* write the tag where the old one was */
  if (-1 == fseek(file->iofile, file->tags[0].location, SEEK_SET)
      || 1 != fwrite(data, length, 1, file->iofile))
    goto fail;

  /* loop through reading and writing the data */
  if (overlap > 0) {

    /* File is getting larger */
    bytesread1 = bytesread2 = overlap;
    while (0 != bytesread1 || 0 != bytesread2) {

      /* Write the contents of ring1 */
      if (1 != fwrite(ring1, bytesread1, 1, file->iofile))
        goto fail;

      /* Read the next "overlap" bytes into ring1 */
      bytesread1 = fread(ring1, 1, overlap, file->iofile);

      if (bytesread1 > 0) {
        /* Seek back that many bytes */
        if (-1 == fseek(file->iofile, -1*bytesread1, SEEK_CUR))
          goto fail;
      }

      /* swap rings */
      ring_tmp = ring1;
      ring1 = ring2;
      ring2 = ring_tmp;

      bytesread_tmp = bytesread1;
      bytesread1 = bytesread2;
      bytesread2 = bytesread_tmp;
    }
  } else {

    /* File is getting smaller */

    /* remember "overlap" is negative so let's absolute it */
    overlap *= -1;

    while (0 == feof(file->iofile)) {

      /* seek ahead "overlap" bytes */
      if (-1 == fseek(file->iofile, overlap, SEEK_CUR))
        goto fail;

      /* read buffer */
      bytesread1 = fread(buffer, 1, buffersize, file->iofile);

      if (bytesread1 > 0) {
        /* seek back that many bytes */
        if (-1 == fseek(file->iofile, -1*(bytesread1+overlap), SEEK_CUR))
          goto fail;

        /* write the buffer contents */
        if (1 != fwrite(buffer, bytesread1, 1, file->iofile))
          goto fail;
      } else {
        /* just seek back to the point we should be at for truncation */
        if (-1 == fseek(file->iofile, -1*overlap, SEEK_CUR))
          goto fail;
      }

      /* Check to see if we've reached the end of the file */
      if (bytesread1 != buffersize)
        break;
    }
  }

  if (buffer)
  {
    free(buffer);
    buffer = NULL;
  }

  /* flush the FILE */
  if (fflush(file->iofile) == EOF)
    goto fail;

  /* truncate if required */
  if (ftell(file->iofile) < file_size) {
    int iResult = ftruncate(fileno(file->iofile), ftell(file->iofile));
    if (iResult != 0) std::cerr<<"myth_v2_write ftruncate FAILED result="<<iResult<<std::endl;
  }

  if (0) {
  fail:
    if (buffer) free(buffer);
    result = -1;
  }

 done:
  return result;
}

/*
 * NAME: file->update()
 * DESCRIPTION:  rewrite tag(s) to a file
 */
int myth_id3_file_update(struct id3_file *file)
{
  int options, result = 0;
  id3_length_t v2size = 0;
  id3_byte_t *id3v2 = 0;

  assert(file);

  if (file->mode != ID3_FILE_MODE_READWRITE)
    return -1;

  options = id3_tag_options(file->primary, 0, 0);

  /* render ID3v2 */

  id3_tag_options(file->primary, ID3_TAG_OPTION_ID3V1, 0);

  v2size = id3_tag_render(file->primary, 0);
  if (v2size) {
    id3v2 = (id3_byte_t*)malloc(v2size);
    if (id3v2 == 0)
      goto fail;

    v2size = id3_tag_render(file->primary, id3v2);
    if (v2size == 0) {
      free(id3v2);
      id3v2 = 0;
    }
  }

  /* write tags */

  if (myth_v2_write(file, id3v2, v2size) == -1)
    goto fail;

  rewind(file->iofile);

  /* update file tags array? ... */

  if (0) {
  fail:
    result = -1;
  }

  /* clean up; restore tag options */

  if (id3v2)
    free(id3v2);

  id3_tag_options(file->primary, ~0, options);

  return result;
}

#endif

class cLibID3Tag
{
public:
  bool ReadTrackTags(spitfire::audio::cMetaData& metaData, const string_t& sFilePath) const;
  bool WriteTrackTags(const spitfire::audio::cMetaData& metaData, const string_t& sFilePath) const;

private:
  bool IsNumbersOnly(const string_t& sText) const;

  string_t GetTag(id3_tag* pTag, const char* szLabel, const string_t& desc = TEXT("")) const;
  string_t GetRawID3String(union id3_field* pField) const;

  bool SetTag(id3_tag *pTag, const char* szLabel, const string_t& value, const string_t& desc = TEXT("")) const;
  void RemoveTag(id3_tag* pTag, const char* szLabel) const;
};

bool cLibID3Tag::IsNumbersOnly(const string_t& sText) const
{
  const size_t n = sText.length();
  for (size_t i = 0; i < n; i++) {
    if (!isdigit(sText[i])) return false;
  }

  return true;
}

bool cLibID3Tag::ReadTrackTags(spitfire::audio::cMetaData& metaData, const string_t& sFilePath) const
{
  metaData.Clear();

  if (!spitfire::filesystem::FileExists(sFilePath)) {
    std::cerr<<"cLibID3Tag::ReadTrackTags File does not exist, returning false"<<std::endl;
    return false;
  }
  if (spitfire::filesystem::GetFileSizeBytes(sFilePath) == 0) {
    std::cerr<<"cLibID3Tag::ReadTrackTags File is empty, returning false"<<std::endl;
    return false;
  }

  id3_file* p_input = id3_file_open(spitfire::string::ToUTF8(sFilePath).c_str(), ID3_FILE_MODE_READONLY);
  if (p_input == nullptr) p_input = id3_file_open(spitfire::string::ToASCII(sFilePath).c_str(), ID3_FILE_MODE_READONLY);

  if (p_input == nullptr) {
    std::cout<<"cLibID3Tag::ReadTrackTags Could not open file, returning false"<<std::endl;
    return false;
  }

  {
    id3_tag* tag = id3_file_tag(p_input);
    if (tag == nullptr) {
      id3_file_close(p_input);
      return false;
    }

    metaData.sTitle = GetTag(tag, ID3_FRAME_TITLE);
    metaData.sArtist = GetTag(tag, ID3_FRAME_ARTIST);
    metaData.sAlbumArtist = GetTag(tag, MYTH_ID3_FRAME_COMPILATIONARTIST);
    if (metaData.sAlbumArtist.empty()) metaData.sAlbumArtist = GetTag(tag, MYTH_ID3_FRAME_ALBUMARTIST);
    metaData.sAlbum = GetTag(tag, ID3_FRAME_ALBUM);

    // Get Track Num dealing with 1/16, 2/16 etc. format
    metaData.uiTracknum = spitfire::string::ToUnsignedInt(GetTag(tag, ID3_FRAME_TRACK));

    // NB Year could be TDRC or TYER depending on version....
    // From: http://www.id3.org/id3v2.4.0-structure.txt
    // Time stamps can be: yyyy, yyyy-MM, yyyy-MM-dd, yyyy-MM-ddTHH,
    //                     yyyy-MM-ddTHH:mm and yyyy-MM-ddTHH:mm:ss
    // Basically all starting yyyy.

    // Depending on the version of libid3tag, it will reassign a #define,
    // but we want to look for both.
    metaData.uiYear = spitfire::string::ToUnsignedInt(GetTag(tag, ID3_FRAME_YEAR));
    if (0 == metaData.uiYear) metaData.uiYear = spitfire::string::ToUnsignedInt(GetTag(tag, MYTH_ID3_FRAME_YEAR));

    // Genre
    metaData.sGenre = GetTag(tag, ID3_FRAME_GENRE);

    // Genre in ID3v2 = "genrenum|Genre Name"
    if (IsNumbersOnly(metaData.sGenre)) {
      // This means the genre is 100% numeric
      // Try and decode genre number
      const std::string sGenreUTF8(spitfire::string::ToUTF8(metaData.sGenre));
      id3_ucs4_t* p_tmp = id3_utf8_ucs4duplicate((const id3_utf8_t*)(const char*)sGenreUTF8.c_str());

      const id3_ucs4_t* p_ucs4 = id3_genre_name(p_tmp);
      free(p_tmp);

      id3_utf8_t* p_utf8 = id3_ucs4_utf8duplicate(p_ucs4);
      metaData.sGenre = spitfire::string::ToString_t((const char*)p_utf8);

      free(p_utf8);
    }

    // Comment
    metaData.sComment = GetTag(tag, ID3_FRAME_COMMENT);

    id3_file_close(p_input);
  }

  // If we don't have title and artist return false
  if (metaData.sTitle.empty() && metaData.sArtist.empty()) {
    std::cerr<<"cLibID3Tag::ReadTrackTags FAILED to read metadata from \""<<sFilePath<<"\"\n";
    return false;
  }

  return true;
}

string_t cLibID3Tag::GetRawID3String(union id3_field* pField) const
{
  string_t tmp = TEXT("");

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

string_t cLibID3Tag::GetTag(id3_tag* pTag, const char* szLabel, const string_t& desc) const
{
  if (szLabel == nullptr) return TEXT("");

  struct id3_frame* p_frame = nullptr;

  for (int i = 0; nullptr != (p_frame = id3_tag_findframe(pTag, szLabel, i)); ++i) {
    int field_num = 1;

    // Compare the first field with the description if supplied
    if (!desc.empty()) {
      string_t tmp = GetRawID3String(&p_frame->fields[field_num++]);

      // Now compare tmp to desc
      if (tmp != desc) {
        // No match - move on.
        continue;
      }
    }

    // Get the value and return it.
    return GetRawID3String(&p_frame->fields[field_num]);
  }

  // Not found.
  return TEXT("");
}


bool cLibID3Tag::WriteTrackTags(const spitfire::audio::cMetaData& metaData, const string_t& sFilePath) const
{
  std::cout<<"cLibID3Tag::WriteTrackTags \""<<sFilePath<<"\""<<std::endl;

  if (!spitfire::filesystem::FileExists(sFilePath)) {
    std::cerr<<"cLibID3Tag::WriteTrackTags File does not exist, returning false"<<std::endl;
    return false;
  }
  if (spitfire::filesystem::GetFileSizeBytes(sFilePath) == 0) {
    std::cerr<<"cLibID3Tag::WriteTrackTags File is empty, returning false"<<std::endl;
    return false;
  }

  id3_file* p_input = id3_file_open(spitfire::string::ToUTF8(sFilePath).c_str(), ID3_FILE_MODE_READWRITE);
  if (p_input == nullptr) p_input = id3_file_open(spitfire::string::ToASCII(sFilePath).c_str(), ID3_FILE_MODE_READWRITE);

  if (p_input == nullptr) {
    std::cout<<"cLibID3Tag::WriteTrackTags Could not open file, returning false"<<std::endl;
    return false;
  }

  std::cout<<"cLibID3Tag::WriteTrackTags 2"<<std::endl;
  // We don't like id3v1 tags... too limiting.
  // We don't write them on encoding, so we delete them if one exists.
  id3_tag* tag = id3_file_tag(p_input);
  if (tag == nullptr) {
    id3_file_close(p_input);
    std::cout<<"cLibID3Tag::WriteTrackTags 3"<<std::endl;
    return false;
  }

  RemoveTag(tag, ID3_FRAME_ARTIST);
  if (!metaData.sArtist.empty()) SetTag(tag, ID3_FRAME_ARTIST, metaData.sArtist);

  RemoveTag(tag, MYTH_ID3_FRAME_ALBUMARTIST);
  RemoveTag(tag, MYTH_ID3_FRAME_COMPILATIONARTIST);
  if (!metaData.sAlbumArtist.empty()) {
    SetTag(tag, MYTH_ID3_FRAME_ALBUMARTIST, metaData.sAlbumArtist);
    SetTag(tag, MYTH_ID3_FRAME_COMPILATIONARTIST, metaData.sAlbumArtist);
  }

  RemoveTag(tag, ID3_FRAME_TITLE);
  if (!metaData.sTitle.empty()) SetTag(tag, ID3_FRAME_TITLE, metaData.sTitle);

  std::cout<<"Removing album"<<std::endl;
  RemoveTag(tag, ID3_FRAME_ALBUM);
  if (!metaData.sAlbum.empty()) {
    std::cout<<"Setting album \""<<metaData.sAlbum<<"\""<<std::endl;
    SetTag(tag, ID3_FRAME_ALBUM, metaData.sAlbum);
  }

  RemoveTag(tag, ID3_FRAME_YEAR);
  if ((metaData.uiYear > 999) && (metaData.uiYear < 10000)) { // 4 digit year
    SetTag(tag, ID3_FRAME_YEAR, spitfire::string::ToString(metaData.uiYear));
  }

  // Write Genre maintaining the ID3v1 genre number if applicable
  RemoveTag(tag, ID3_FRAME_GENRE);
  if (!metaData.sGenre.empty()) {
    id3_ucs4_t* p_ucs4 = id3_utf8_ucs4duplicate((const id3_utf8_t*)spitfire::string::ToUTF8(metaData.sGenre).c_str());

    int genrenum = id3_genre_number(p_ucs4);

    free(p_ucs4);

    // Use the number if it's standard, otherwise just write it (valid in ID3v2)
    if (genrenum >= 0) SetTag(tag, ID3_FRAME_GENRE, spitfire::string::ToString(genrenum));
    else SetTag(tag, ID3_FRAME_GENRE, metaData.sGenre);
  }

  RemoveTag(tag, ID3_FRAME_TRACK);
  if (0 != metaData.uiTracknum) SetTag(tag, ID3_FRAME_TRACK, spitfire::string::ToString(metaData.uiTracknum));

  RemoveTag(tag, ID3_FRAME_COMMENT);
  if (!metaData.sComment.empty()) SetTag(tag, ID3_FRAME_COMMENT, metaData.sComment);

  // Set ID3 tag options
  id3_tag_options(tag, ID3_TAG_OPTION_COMPRESSION, 0);
  id3_tag_options(tag, ID3_TAG_OPTION_CRC, 0);
  id3_tag_options(tag, ID3_TAG_OPTION_UNSYNCHRONISATION, 0);
  id3_tag_options(tag, ID3_TAG_OPTION_ID3V1, 0);

  // The id3_file_update function has a feature deficit
  bool bUpdateResult = (0 == ID3_FILE_UPDATE(p_input));

  bool bCloseResult = (0 == id3_file_close(p_input));

  std::cout<<"cLibID3Tag::WriteTrackTags returning "<<((bCloseResult && bUpdateResult) ? "true" : "false")<<std::endl;

  return (bCloseResult && bUpdateResult);
}

void cLibID3Tag::RemoveTag(id3_tag* pTag, const char* szLabel) const
{
  std::cout<<"cLibID3Tag::RemoveTag \""<<szLabel<<"\""<<std::endl;

  if (szLabel == nullptr) return;

  struct id3_frame* p_frame = nullptr;

  // Delete all tags with label
  for (size_t i = 0; nullptr != (p_frame = id3_tag_findframe(pTag, szLabel, i)); ++i) {
    std::cout<<"cLibID3Tag::RemoveTag Found tag "<<i<<std::endl;
    // Delete it
    if (0 == id3_tag_detachframe(pTag, p_frame)) {
      std::cout<<"cLibID3Tag::RemoveTag Deleting frame"<<std::endl;
      id3_frame_delete(p_frame);
    } else std::cerr<<"cLibID3Tag::RemoveTag Could not detach frame"<<std::endl;
  }
}

bool cLibID3Tag::SetTag(id3_tag* pTag, const char* szLabel, const string_t& value, const string_t& desc) const
{
  if ((szLabel == nullptr) || value.empty()) return false;

  id3_frame* p_frame = id3_frame_new(szLabel);
  if (p_frame == nullptr) return false;

  if (id3_field_settextencoding(&p_frame->fields[0], ID3_FIELD_TEXTENCODING_UTF_16) != 0) {
    id3_frame_delete(p_frame);
    return false;
  }

  id3_ucs4_t* p_ucs4 = nullptr;

  // Write a description in field 1 if needs be.
  if (!desc.empty()) {
    p_ucs4 = id3_utf8_ucs4duplicate((const id3_utf8_t*)spitfire::string::ToUTF8(desc).c_str());

    if (!p_ucs4) {
      id3_frame_delete(p_frame);
      return false;
    }

    if (0 != id3_field_setstring(&p_frame->fields[1], p_ucs4)) {
      free(p_ucs4);
      id3_frame_delete(p_frame);
      return false;
    }

    free(p_ucs4);
  }

  p_ucs4 = id3_utf8_ucs4duplicate((const id3_utf8_t*)spitfire::string::ToUTF8(value).c_str());

  if (!p_ucs4) {
    id3_frame_delete(p_frame);
    return false;
  }

  if ((desc.empty() && id3_field_setstrings(&p_frame->fields[1], 1, &p_ucs4)) || (!desc.empty() && id3_field_setstring(&p_frame->fields[2], p_ucs4))) {
    free(p_ucs4);
    id3_frame_delete(p_frame);
    return false;
  }

  free(p_ucs4);

  if (0 != id3_tag_attachframe(pTag, p_frame)) {
    id3_frame_delete(p_frame);
    return false;
  }

  return true;
}



// ** cTrackPropertiesReader

bool cTrackPropertiesReader::ReadTrackProperties(spitfire::audio::cMetaData& metaData, const string_t& sFilePath) const
{
  metaData.Clear();

  bool bResult = true;

  if (spitfire::filesystem::GetExtensionNoDot(sFilePath) == TEXT("mp3")) {
    if (!ReadTrackTags(metaData, sFilePath)) bResult = false;
  } else {

  }

  if (!ReadTrackLength(metaData, sFilePath)) bResult = false;

  return bResult;
}

bool cTrackPropertiesReader::ReadTrackTags(spitfire::audio::cMetaData& metaData, const string_t& sFilePath) const
{
  cLibID3Tag libID3Tag;
  return libID3Tag.ReadTrackTags(metaData, sFilePath);
}

// Use libmad to read the track length
// http://mythaudio.googlecode.com/svn/trunk/mythaudio/metaioid3v2.cpp

bool cTrackPropertiesReader::ReadTrackLength(spitfire::audio::cMetaData& metaData, const string_t& sFilePath) const
{
  // TODO: CLEAN UP THIS FUNCTION

  metaData.uiDurationMilliSeconds = 0;

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

  size_t iterations = 0;

  bool bContinue = true;
  while (bContinue) {
    // If for some reason we can't progress through this file then we break
    if (iterations > 500) break;

    // Increment our safety counter
    iterations++;

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

  if (bIsVBR) metaData.uiDurationMilliSeconds = mad_timer_count(timer, MAD_UNITS_MILLISECONDS);
  else metaData.uiDurationMilliSeconds = alt_length;

  return true;
}


// ** cTrackPropertiesWriter

bool cTrackPropertiesWriter::WriteTrackProperties(const spitfire::audio::cMetaData& metaData, const string_t& sFilePath) const
{
  bool bResult = true;

  if (!WriteTrackTags(metaData, sFilePath)) bResult = false;

  return bResult;
}

bool cTrackPropertiesWriter::WriteTrackTags(const spitfire::audio::cMetaData& metaData, const string_t& sFilePath) const
{
  cLibID3Tag libID3Tag;
  return libID3Tag.WriteTrackTags(metaData, sFilePath);
}
}
