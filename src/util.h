#ifndef MEDUSA_UTIL_H
#define MEDUSA_UTIL_H

// Spitfire headers
#include <spitfire/audio/metadata.h>

// Medusa headers
#include "medusa.h"
#include "track.h"

namespace medusa
{
  namespace util
  {
    string_t FormatNumber(uint64_t uiValue);
    string_t FormatTime(uint64_t milliseconds);

    void ClearPassword(std::string& sPassword);
    void ClearPassword(std::wstring& sPassword);

    string_t GetPlayListFilePath();

    bool LoadPlaylistFromCSV(const string_t& sFilePath, std::vector<cTrack*>& playlist);
    bool SavePlaylistToCSV(const string_t& sFilePath, const std::vector<cTrack*>& playlist);
  }
}

#endif // !MEDUSA_UTIL_H
