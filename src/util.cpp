// Standard headers
#include <sstream>
#include <iomanip>

// Spitfire headers
#include <spitfire/storage/csv.h>
#include <spitfire/storage/filesystem.h>

// Medusa headers
#include "util.h"

namespace medusa
{
  namespace util
  {
    string_t FormatNumber(uint64_t uiValue)
    {
      if (uiValue == 0) return TEXT("");

      return spitfire::string::ToString(uiValue);
    }

    string_t FormatTime(uint64_t milliseconds)
    {
      spitfire::ostringstream_t o;

      o<<std::right<<std::setfill(TEXT('0'));

      uint64_t time = milliseconds / 1000;

      const uint64_t seconds = time % 60;
      time /= 60;

      const uint64_t minutes = time % 60;
      time /= 60;

      const uint64_t hours = time % 24;
      time /= 24;

      const uint64_t days = hours;

      if (days != 0) {
        o<<days<<TEXT(":");
        o<<std::setw(2); // Set width of 2 for the next value
      }

      if (hours != 0)  {
        o<<hours<<TEXT(":");
        o<<std::setw(2); // Set width of 2 for the next value
      }

      o<<minutes<<":";
      o<<std::setw(2)<<seconds;

      return o.str();
    }

    void ClearPassword(std::string& sPassword)
    {
      const size_t n = sPassword.length();
      for (size_t i = 0; i < n; i++) sPassword[i] = 0;
    }

    void ClearPassword(std::wstring& sPassword)
    {
      const size_t n = sPassword.length();
      for (size_t i = 0; i < n; i++) sPassword[i] = 0;
    }

    string_t GetPlayListFilePath()
    {
      return spitfire::filesystem::GetThisApplicationSettingsDirectory() + TEXT("playlist.csv");
    }

    void AddTrackToPlaylist(spitfire::audio::cPlaylist& playlist, const cTrack* pTrack)
    {
      spitfire::audio::cTrack* pPlaylistTrack = new spitfire::audio::cTrack;
      pPlaylistTrack->sFullPath = pTrack->sFilePath;

      pPlaylistTrack->sArtist = pTrack->metaData.sArtist;
      pPlaylistTrack->sTitle = pTrack->metaData.sTitle;
      pPlaylistTrack->uiTrackLengthMS = pTrack->metaData.uiDurationMilliSeconds;

      playlist.tracks.push_back(pPlaylistTrack);
    }

    bool LoadPlaylistFromCSV(const string_t& sFilePath, spitfire::audio::cPlaylist& playlist)
    {
      playlist.Clear();

      spitfire::csv::cReader reader;
      if (!reader.Open(sFilePath)) {
        std::wcerr<<"LoadPlaylistFromCSV Error opening \""<<sFilePath<<"\", returning false"<<std::endl;
        return false;
      }

      // Read titles
      std::vector<string_t> values;
      reader.ReadLine(values);

      while (reader.ReadLine(values)) {
        std::wcerr<<"LoadPlaylistFromCSV Reading "<<values.size()<<" values"<<std::endl;
        if (values.size() != 4) break;

        std::wcerr<<"LoadPlaylistFromCSV Adding track \""<<values[0]<<"\", \""<<values[1]<<"\", \""<<values[2]<<"\", \""<<values[3]<<"\""<<std::endl;
        spitfire::audio::cTrack* pTrack = new spitfire::audio::cTrack;
        pTrack->sFullPath = values[0];
        pTrack->sArtist = values[1];
        pTrack->sTitle = values[2];
        pTrack->uiTrackLengthMS = spitfire::string::ToUnsignedInt(values[3]);

        playlist.tracks.push_back(pTrack);
      }

      return true;
    }

    bool SavePlaylistToCSV(const string_t& sFilePath, const spitfire::audio::cPlaylist& playlist)
    {
      spitfire::csv::cWriter writer;
      if (!writer.Open(sFilePath)) {
        std::wcerr<<"SavePlaylistToCSV Error opening \""<<sFilePath<<"\", returning false"<<std::endl;
        return false;
      }

      // Write titles
      writer.AddValue(TEXT("Full Path"));
      writer.AddValue(TEXT("Artist"));
      writer.AddValue(TEXT("Title"));
      writer.AddValue(TEXT("Duration MS"));
      writer.EndRow();

      const std::vector<spitfire::audio::cTrack*>& tracks = playlist.tracks;
      const size_t n = tracks.size();
      for (size_t i = 0; i < n; i++) {
        const spitfire::audio::cTrack* pTrack = tracks[i];
        assert(pTrack != nullptr);
        const spitfire::audio::cTrack& track = *pTrack;
        writer.AddValue(track.sFullPath);
        writer.AddValue(track.sArtist);
        writer.AddValue(track.sTitle);
        writer.AddValue(spitfire::string::ToString(track.uiTrackLengthMS));
        writer.EndRow();
      }

      return true;
    }
  }
}
