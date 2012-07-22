#ifndef MEDUSA_UTIL_H
#define MEDUSA_UTIL_H

// Standard headers
#include <list>

// Spitfire headers
#include <spitfire/audio/metadata.h>
#include <spitfire/util/datetime.h>

// Medusa headers
#include "medusa.h"
#include "track.h"

namespace medusa
{
  namespace util
  {
    string_t FormatNumber(uint64_t uiValue);
    string_t FormatTime(uint64_t milliseconds);

    // YYYY-MM-DD HH:MM:SS.MMM
    string_t FormatDateTime(const spitfire::util::cDateTime& dateTime);
    bool ParseDateTime(spitfire::util::cDateTime& dateTime, const string_t& sDateTime);

    void ClearPassword(std::string& sPassword);
    void ClearPassword(std::wstring& sPassword);

    string_t GetPlayListFilePath();
    string_t GetLastPlayedFilePath();

    bool LoadPlaylistFromCSV(const string_t& sFilePath, std::list<cTrack*>& playlist);
    bool SavePlaylistToCSV(const string_t& sFilePath, const std::list<cTrack*>& playlist);
  }
}

#endif // !MEDUSA_UTIL_H
