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

    string_t FormatDateTime(const spitfire::util::cDateTime& dateTime)
    {
      spitfire::ostringstream_t o;

      o<<std::right<<std::setfill(TEXT('0'));

      const int year = dateTime.GetYear();
      o<<std::setw(4)<<year<<TEXT("-");

      const int month = dateTime.GetMonth();
      o<<std::setw(2)<<month<<TEXT("-");

      const int day = dateTime.GetDay();
      o<<std::setw(2)<<day<<TEXT(" ");

      const int hours = dateTime.GetHours();
      o<<std::setw(2)<<hours<<TEXT(":");

      const int minutes = dateTime.GetMinutes();
      o<<std::setw(2)<<minutes<<":";

      const int seconds = dateTime.GetSeconds();
      o<<std::setw(2)<<seconds<<".";

      const int milliseconds = dateTime.GetMilliSeconds();
      o<<std::setw(3)<<milliseconds;

      return o.str();
    }

    bool ParseDateTime(spitfire::util::cDateTime& dateTime, const string_t& sText)
    {
      spitfire::string::cStringParser sp(sText);

      if (sp.IsEnd()) return false;
      const string_t sYear = sp.GetCharactersAndSkip(4);

      if (sp.IsEnd()) return false;
      char_t c = sp.GetCharacterAndSkip();
      if (c != TEXT('-')) return false;

      if (sp.IsEnd()) return false;
      const string_t sMonth = sp.GetCharactersAndSkip(2);

      if (sp.IsEnd()) return false;
      c = sp.GetCharacterAndSkip();
      if (c != TEXT('-')) return false;

      if (sp.IsEnd()) return false;
      const string_t sDay = sp.GetCharactersAndSkip(2);

      if (sp.IsEnd()) return false;
      c = sp.GetCharacterAndSkip();
      if (c != TEXT(' ')) return false;

      if (sp.IsEnd()) return false;
      const string_t sHour = sp.GetCharactersAndSkip(2);

      if (sp.IsEnd()) return false;
      c = sp.GetCharacterAndSkip();
      if (c != TEXT(':')) return false;

      if (sp.IsEnd()) return false;
      const string_t sMinute = sp.GetCharactersAndSkip(2);

      if (sp.IsEnd()) return false;
      c = sp.GetCharacterAndSkip();
      if (c != TEXT(':')) return false;

      if (sp.IsEnd()) return false;
      const string_t sSecond = sp.GetCharactersAndSkip(2);

      if (sp.IsEnd()) return false;
      c = sp.GetCharacterAndSkip();
      if (c != TEXT('.')) return false;

      if (sp.IsEnd()) return false;
      const string_t sMilliSecond = sp.GetCharactersAndSkip(3);

      const unsigned int year = spitfire::string::ToUnsignedInt(sYear);
      const unsigned int month = spitfire::string::ToUnsignedInt(sMonth);
      const unsigned int day = spitfire::string::ToUnsignedInt(sDay);
      const unsigned int hour = spitfire::string::ToUnsignedInt(sHour);
      const unsigned int minute = spitfire::string::ToUnsignedInt(sMinute);
      const unsigned int second = spitfire::string::ToUnsignedInt(sSecond);
      const unsigned int millisecond = spitfire::string::ToUnsignedInt(sMilliSecond);
      // Sanity check
      if ((month > 12) || (day > 31) || (hour > 24) || (minute > 60) || (second > 60) || (millisecond > 1000)) return false;

      dateTime = spitfire::util::cDateTime(year, month, day, hour, minute, second, millisecond);

      return true;
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

    string_t GetLastPlayedFilePath()
    {
      return spitfire::filesystem::GetThisApplicationSettingsDirectory() + TEXT("lastplayed.txt");
    }

    bool LoadPlaylistFromCSV(const string_t& sFilePath, std::vector<cTrack*>& playlist)
    {
      const size_t n = playlist.size();
      for (size_t i = 0; i < n; i++) spitfire::SAFE_DELETE(playlist[i]);

      playlist.clear();

      spitfire::csv::cReader reader;
      if (!reader.Open(sFilePath)) {
        std::cerr<<"LoadPlaylistFromCSV Error opening \""<<sFilePath<<"\", returning false"<<std::endl;
        return false;
      }

      // Read titles
      std::vector<string_t> values;
      reader.ReadLine(values);

      while (reader.ReadLine(values)) {
        std::cerr<<"LoadPlaylistFromCSV Reading "<<values.size()<<" values"<<std::endl;
        if (values.size() != 11) break;

        std::cerr<<"LoadPlaylistFromCSV Adding track \""<<values[0]<<"\", \""<<values[1]<<"\", \""<<values[2]<<std::endl;
        cTrack* pTrack = new cTrack;
        pTrack->sFilePath = values[0];
        ParseDateTime(pTrack->dateAdded, values[1]);
        pTrack->metaData.sArtist = values[2];
        pTrack->metaData.sTitle = values[3];
        pTrack->metaData.sAlbum = values[4];
        pTrack->metaData.sAlbumArtist = values[5];
        pTrack->metaData.sGenre = values[6];
        pTrack->metaData.sComment = values[7];
        pTrack->metaData.uiYear = spitfire::string::ToUnsignedInt(values[8]);
        pTrack->metaData.uiTracknum = spitfire::string::ToUnsignedInt(values[9]);
        pTrack->metaData.uiDurationMilliSeconds = spitfire::string::ToUnsignedInt(values[10]);

        playlist.push_back(pTrack);
      }

      return true;
    }

    bool SavePlaylistToCSV(const string_t& sFilePath, const std::vector<cTrack*>& playlist)
    {
      spitfire::csv::cWriter writer;
      if (!writer.Open(sFilePath)) {
        std::cerr<<"SavePlaylistToCSV Error opening \""<<sFilePath<<"\", returning false"<<std::endl;
        return false;
      }

      // Write titles
      writer.AddValue(TEXT("Full Path"));
      writer.AddValue(TEXT("Date Added"));
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
        writer.AddValue(FormatDateTime(track.dateAdded));
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
