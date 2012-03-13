#ifndef MEDUSA_UTIL_H
#define MEDUSA_UTIL_H

// Spitfire headers
#include <spitfire/audio/playlist.h>

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

    void AddTrackToPlaylist(spitfire::audio::cPlaylist& playlist, const cTrack* pTrack);

    bool LoadPlaylistFromCSV(const string_t& sFilePath, spitfire::audio::cPlaylist& playlist);
    bool SavePlaylistToCSV(const string_t& sFilePath, const spitfire::audio::cPlaylist& playlist);
  }
}

#endif // !MEDUSA_UTIL_H
