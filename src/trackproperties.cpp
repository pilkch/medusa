// Standard headers
#include <cstdio>
#include <memory.h>

// Unix headers
#include <sys/stat.h>

// libid3tab headers
#include <id3tag.h>

// libmad headers
#include <mad.h>

// Meduasa headers
#include "trackproperties.h"

// ** cTrackProperties

cTrackProperties::cTrackProperties() :
  uiDurationMilliSeconds(0)
{
}


// ** cTrackPropertiesReader

bool cTrackPropertiesReader::ReadTrackProperties(cTrackProperties& properties, const spitfire::string_t& sFilePath) const
{
  bool bResult = true;

  if (!ReadTrackLength(properties,sFilePath)) bResult = false;

  return bResult;
}

// Use libmad to read the track length
// http://mythaudio.googlecode.com/svn/trunk/mythaudio/metaioid3v2.cpp

bool cTrackPropertiesReader::ReadTrackLength(cTrackProperties& properties, const spitfire::string_t& sFilePath) const
{
  // TODO: CLEAN UP THIS FUNCTION

  properties.uiDurationMilliSeconds = 0;

  const std::string sFileNameUTF8 = spitfire::string::ToUTF8(sFilePath);
  FILE *input = fopen(sFileNameUTF8.c_str(), "r");
  if (!input) return false;

  struct stat s;
  fstat(fileno(input), &s);

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
      int bytes = fread(buffer + buflen, 1, sizeof(buffer) - buflen, input);
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

  fclose(input);

  if (bIsVBR) properties.uiDurationMilliSeconds = mad_timer_count(timer, MAD_UNITS_MILLISECONDS);
  else properties.uiDurationMilliSeconds = alt_length;

  return true;
}
