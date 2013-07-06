// Standard headers
#include <sstream>
#include <iomanip>

// Spitfire headers
#include <spitfire/storage/csv.h>
#include <spitfire/storage/document.h>
#include <spitfire/storage/filesystem.h>
#include <spitfire/storage/xml.h>

// libxdgmm headers
#include <libxdgmm/libxdgmm.h>

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

    void ClearPlaylist(std::set<cTrack*>& playlist)
    {
      std::set<cTrack*>::iterator iter = playlist.begin();
      const std::set<cTrack*>::iterator iterEnd = playlist.end();
      while (iter != iterEnd) {
        delete *iter;

        iter++;
      }

      playlist.clear();
    }

    bool LoadPlaylistFromCSV(const string_t& sFilePath, std::set<cTrack*>& playlist)
    {
      ClearPlaylist(playlist);

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
        if (values.size() != 12) break;

        std::cerr<<"LoadPlaylistFromCSV Adding track \""<<values[0]<<"\", \""<<values[1]<<"\", \""<<values[2]<<"\", \""<<values[3]<<std::endl;
        cTrack* pTrack = new cTrack;
        pTrack->sFilePath = values[0];
        ParseDateTime(pTrack->dateAdded, values[1]);
        ParseDateTime(pTrack->dateLastPlayed, values[2]);
        pTrack->metaData.sArtist = values[3];
        pTrack->metaData.sTitle = values[4];
        pTrack->metaData.sAlbum = values[5];
        pTrack->metaData.sAlbumArtist = values[6];
        pTrack->metaData.sGenre = values[7];
        pTrack->metaData.sComment = values[8];
        pTrack->metaData.uiYear = spitfire::string::ToUnsignedInt(values[9]);
        pTrack->metaData.uiTracknum = spitfire::string::ToUnsignedInt(values[10]);
        pTrack->metaData.uiDurationMilliSeconds = spitfire::string::ToUnsignedInt(values[11]);

        playlist.insert(pTrack);
      }

      return true;
    }

    bool SavePlaylistToCSV(const string_t& sFilePath, const std::set<cTrack*>& playlist)
    {
      spitfire::csv::cWriter writer;
      if (!writer.Open(sFilePath)) {
        std::cerr<<"SavePlaylistToCSV Error opening \""<<sFilePath<<"\", returning false"<<std::endl;
        return false;
      }

      // Write titles
      writer.AddValue(TEXT("Full Path"));
      writer.AddValue(TEXT("Date Added"));
      writer.AddValue(TEXT("Date Last Played"));
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

      std::set<cTrack*>::const_iterator iter = playlist.begin();
      const std::set<cTrack*>::const_iterator iterEnd = playlist.end();
      while (iter != iterEnd) {
        const cTrack* pTrack = *iter;
        assert(pTrack != nullptr);
        const cTrack& track = *pTrack;
        writer.AddValue(track.sFilePath);
        writer.AddValue(FormatDateTime(track.dateAdded));
        writer.AddValue(FormatDateTime(track.dateLastPlayed));
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

        iter++;
      }

      return true;
    }


    #ifdef BUILD_MEDUSA_IMPORT_BANSHEE_PLAYLIST
    string_t GetBansheePlaylistPath()
    {
      return "";
    }

    bool IsBansheePlaylistFilePresent()
    {
      return spitfire::filesystem::FileExists(GetBansheePlaylistPath());
    }

    bool LoadBansheePlaylistFile(std::list<string_t>& files)
    {
      files.clear();

      // http://code.google.com/p/rhythmbox-banshee-import/source/browse/trunk/import.py

      return false;
    }
    #endif // BUILD_MEDUSA_IMPORT_BANSHEE_PLAYLIST


    string_t DecodeRhythmBoxPlaylistLocation(const string_t& sLocation)
    {
      return spitfire::string::Replace(sLocation, TEXT("%20"), TEXT(" "));
    }

    string_t GetRhythmBoxPlaylistPath()
    {
      // ~/.local/share/rhythmbox/rhythmdb.xml
      xdg::cXdg xdg;
      const string_t sHomeDataDirectory(spitfire::string::ToString_t(xdg.GetHomeDataDirectory()));
      return spitfire::filesystem::MakeFilePath(spitfire::filesystem::MakeFilePath(sHomeDataDirectory, TEXT("rhythmbox")), TEXT("rhythmdb.xml"));
    }

    bool IsRhythmBoxPlaylistFilePresent()
    {
      return spitfire::filesystem::FileExists(GetRhythmBoxPlaylistPath());
    }

    spitfire::util::PROCESS_RESULT LoadRhythmBoxPlaylistFile(spitfire::util::cProcessInterface& interface, std::list<string_t>& files)
    {
      files.clear();

      const string_t sFilePath(GetRhythmBoxPlaylistPath());

      // <?xml version="1.0" standalone="yes"?>
      // <rhythmdb version="1.8">
      //   <entry type="song">
      //     <location>file:///home/chris/Music/new%20music/The%20Raconteurs%20-%20Consoler%20Of%20The%20Lonely.mp3</location>
      //     ...
      //   </entry>
      //   ...
      // </rhythmdb>

      spitfire::document::cDocument document;

      spitfire::xml::reader reader;
      spitfire::util::PROCESS_RESULT result = reader.ReadFromFile(interface, document, sFilePath);
      if (result != spitfire::util::PROCESS_RESULT::COMPLETE) {
        std::cerr<<"LoadRhythmBoxPlaylistFile Error opening \""<<sFilePath<<"\", returning"<<std::endl;
        return result;
      }

      spitfire::document::cDocument::cConstIterator iter(document);
      if (!iter.IsValid()) {
        std::cerr<<"LoadRhythmBoxPlaylistFile Error parsing \""<<sFilePath<<"\", returning FAILED"<<std::endl;
        return spitfire::util::PROCESS_RESULT::FAILED;
      }

      iter.FindChild("rhythmdb");
      if (!iter.IsValid()) {
        std::cerr<<"LoadRhythmBoxPlaylistFile rhythmdb not found, returning FAILED"<<std::endl;
        return spitfire::util::PROCESS_RESULT::FAILED;
      }

      // Iterate through the entry elements
      string_t sType;
      iter.FindChild("entry");
      while (iter.IsValid()) {
        std::cout<<"LoadRhythmBoxPlaylistFile entry"<<std::endl;
        // Look for entries of type "song"
        if (iter.GetAttribute("type", sType) && (sType == "song")) {
          std::cout<<"LoadRhythmBoxPlaylistFile song"<<std::endl;
          spitfire::document::cDocument::cConstIterator iterLocation = iter.GetChild("location");
          if (iterLocation.IsValid()) {
            std::cout<<"LoadRhythmBoxPlaylistFile song with location"<<std::endl;
            iterLocation.FirstChild();
            const string_t sLocation = iterLocation.GetContent();
            if (spitfire::string::StartsWith(sLocation, "file://")) {
              std::cout<<"LoadRhythmBoxPlaylistFile song with file location"<<std::endl;
              const string_t sFilePath = DecodeRhythmBoxPlaylistLocation(sLocation.substr(7));
              files.push_back(sFilePath);
            }
          }
        }

        iter.Next();
      };

      std::cout<<"LoadRhythmBoxPlaylistFile returning true"<<std::endl;
      return spitfire::util::PROCESS_RESULT::FAILED;
    }
  }
}
