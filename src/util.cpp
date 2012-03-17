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

    bool LoadPlaylistFromCSV(const string_t& sFilePath, std::vector<cTrack*>& playlist)
    {
      const size_t n = playlist.size();
      for (size_t i = 0; i < n; i++) spitfire::SAFE_DELETE(playlist[i]);

      playlist.clear();

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
        if (values.size() != 10) break;

        std::wcerr<<"LoadPlaylistFromCSV Adding track \""<<values[0]<<"\", \""<<values[1]<<"\", \""<<values[2]<<std::endl;
        cTrack* pTrack = new cTrack;
        pTrack->sFilePath = values[0];
        pTrack->metaData.sArtist = values[1];
        pTrack->metaData.sTitle = values[2];
        pTrack->metaData.sAlbum = values[3];
        pTrack->metaData.sAlbumArtist = values[4];
        pTrack->metaData.sGenre = values[5];
        pTrack->metaData.sComment = values[6];
        pTrack->metaData.uiYear = spitfire::string::ToUnsignedInt(values[7]);
        pTrack->metaData.uiTracknum = spitfire::string::ToUnsignedInt(values[8]);
        pTrack->metaData.uiDurationMilliSeconds = spitfire::string::ToUnsignedInt(values[9]);

        playlist.push_back(pTrack);
      }

      return true;
    }

    bool SavePlaylistToCSV(const string_t& sFilePath, const std::vector<cTrack*>& playlist)
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
      writer.AddValue(TEXT("Album"));
      writer.AddValue(TEXT("Album Artist"));
      writer.AddValue(TEXT("Genre"));
      writer.AddValue(TEXT("Comment"));
      writer.AddValue(TEXT("Year"));
      writer.AddValue(TEXT("Track Number"));
      writer.AddValue(TEXT("Duration MS"));
      writer.EndRow();

      const std::vector<cTrack*>& tracks = playlist;
      const size_t n = tracks.size();
      for (size_t i = 0; i < n; i++) {
        const cTrack* pTrack = tracks[i];
        assert(pTrack != nullptr);
        const cTrack& track = *pTrack;
        writer.AddValue(track.sFilePath);
        writer.AddValue(track.metaData.sArtist);
        writer.AddValue(track.metaData.sTitle);
        writer.AddValue(track.metaData.sAlbum);
        writer.AddValue(track.metaData.sAlbumArtist);
        writer.AddValue(track.metaData.sGenre);
        writer.AddValue(track.metaData.sComment);
        writer.AddValue(spitfire::string::ToString(track.metaData.uiYear));
        writer.AddValue(spitfire::string::ToString(track.metaData.uiTracknum));
        writer.AddValue(spitfire::string::ToString(track.metaData.uiDurationMilliSeconds));
        writer.EndRow();
      }

      return true;
    }
  }
}
