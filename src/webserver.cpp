// Medusa headers
#include "gtkmmview.h"
#include "util.h"
#include "webserver.h"

namespace medusa
{
  const size_t nMaxSongEntries = 20;

  // ** cWebPageController

  class cWebPageController
  {
  public:
    explicit cWebPageController(const std::list<cWebServerSongEntry>& tracks);

    std::vector<std::string> GetExternalJavaScripts() const;

    void GetProperties(spitfire::storage::cHTMLDocumentProperties& properties) const;

    spitfire::string_t GetTitle() const;
    spitfire::string_t GetDescription() const;
    spitfire::string_t GetBannerTitle() const;
    spitfire::string_t GetSectionTitle() const;

    std::vector<std::pair<std::string, std::string> > GetArticles() const;
    template <class W>
    void AddArticle(W& writer, const std::string& sArticle) const;

    std::vector<std::pair<std::string, std::string> > GetAsides() const;
    template <class W>
    void AddAside(W& writer, const std::string& sAside) const;

  private:
    template <class W>
    void AddFormWithImageButton(W& writer, const std::string& sAction, const std::string& sImage, const std::string& sAlternativeText, size_t nSize) const;

    std::list<cWebServerSongEntry> tracks;
  };

  cWebPageController::cWebPageController(const std::list<cWebServerSongEntry>& _tracks) :
    tracks(_tracks)
  {
  }

  std::vector<std::string> cWebPageController::GetExternalJavaScripts() const
  {
    std::vector<std::string> scripts;
    scripts.push_back("scripts/actions.js");
    scripts.push_back("scripts/events.js");
    return scripts;
  }

  void cWebPageController::GetProperties(spitfire::storage::cHTMLDocumentProperties& properties) const
  {
    properties.sLanguageCode = TEXT("EN");
  }

  spitfire::string_t cWebPageController::GetTitle() const
  {
    return TEXT("Medusa");
  }

  spitfire::string_t cWebPageController::GetDescription() const
  {
    return TEXT("My Description");
  }

  spitfire::string_t cWebPageController::GetBannerTitle() const
  {
    return TEXT("My Banner");
  }

  spitfire::string_t cWebPageController::GetSectionTitle() const
  {
    return TEXT("My Section");
  }

  std::vector<std::pair<std::string, std::string> > cWebPageController::GetArticles() const
  {
    std::vector<std::pair<std::string, std::string> > articles;
    articles.push_back(std::make_pair(TEXT("playback"), TEXT("Playback")));
    articles.push_back(std::make_pair(TEXT("tracks"), TEXT("Tracks")));
    return articles;
  }

  template <class W>
  void cWebPageController::AddFormWithImageButton(W& writer, const std::string& sAction, const std::string& sImage, const std::string& sAlternativeText, size_t nSize) const
  {
    writer.WriteLine("      <a href=\"#\" onclick=\"" + sAction + "\" class=\"image_button\">");
    writer.WriteLine("        <img src=\"images/" + sImage + ".png\" width=\"" + spitfire::string::ToString(nSize) + "\" height=\"" + spitfire::string::ToString(nSize) + "\" alt=\"" + sAlternativeText + "\"/>");
    writer.WriteLine("      </a>");
  }

  template <class W>
  void cWebPageController::AddArticle(W& writer, const std::string& sArticle) const
  {
    if (sArticle == TEXT("playback")) {
      writer.WriteLine("<img src=\"images/medusa.png\"/><br/>");

      const size_t nSize = 32;
      AddFormWithImageButton(writer, "OnActionPlaybackPrevious()", "playback_previous", "Previous", nSize);
      AddFormWithImageButton(writer, "OnActionPlaybackPlay()", "playback_play", "Play", nSize);
      AddFormWithImageButton(writer, "OnActionPlaybackNext()", "playback_next", "Next", nSize);
      AddFormWithImageButton(writer, "OnActionVolumeMute()", "volume_mute", "Mute Volume", nSize);
      AddFormWithImageButton(writer, "OnActionVolumeFull()", "volume_full", "Full Volume", nSize);
    } else if (sArticle == TEXT("tracks")) {
      writer.WriteLine("<h3>Last " + spitfire::string::ToString(nMaxSongEntries) + " Songs Played</h3>");

      if (tracks.empty()) {
        writer.WriteLine("No songs have been played yet");
      } else {
        writer.WriteLine("<table class=\"table_border\">");
        writer.WriteLine("  <tr id=\"tracklist_header\" class=\"table_heading\">");
        writer.WriteLine("    <th class=\"table_border\">Artist</th>");
        writer.WriteLine("    <th class=\"table_border\">Title</th>");
        writer.WriteLine("    <th class=\"table_border\">Duration</th>");
        writer.WriteLine("    <th class=\"table_border\">Download</th>");
        writer.WriteLine("    <th class=\"table_border\">Delete</th>");
        writer.WriteLine("  </tr>");
        writer.WriteLine("");

        const size_t nSize = 16;
        const std::string sSize = spitfire::string::ToString(nSize);

        std::list<cWebServerSongEntry>::const_iterator iter(tracks.begin());
        const std::list<cWebServerSongEntry>::const_iterator iterEnd(tracks.end());
        while (iter != iterEnd) {
          writer.WriteLine("  <tr class=\"table_border\">");
          writer.WriteLine("    <th class=\"table_border\">" + iter->sArtist + "</th>");
          writer.WriteLine("    <th class=\"table_border\">" + iter->sTitle + "</th>");
          writer.WriteLine("    <th class=\"table_border\">" + medusa::util::FormatTime(iter->uiDurationMilliSeconds) + "</th>");
          writer.WriteLine("    <th class=\"table_border\">");
          writer.WriteLine("      <a href=\"download/" + spitfire::filesystem::GetFile(iter->sFilePath) + "\"><img src=\"images/file_save.png\" alt=\"Save File\" width=\"" + spitfire::string::ToString(nSize) + "\" height=\"" + spitfire::string::ToString(nSize) + "\"/></a>");
          writer.WriteLine("    </th>");
          writer.WriteLine("    <th class=\"table_border\">");
          AddFormWithImageButton(writer, "OnActionDeleteTrack(" + spitfire::string::ToString(iter->id) + ")", "file_trash", "Delete File", nSize);
          writer.WriteLine("    </th>");
          writer.WriteLine("  </tr>");

          iter++;
        }

        writer.WriteLine("</table>");
      }
    }
  }

  std::vector<std::pair<std::string, std::string> > cWebPageController::GetAsides() const
  {
    std::vector<std::pair<std::string, std::string> > asides;
    asides.push_back(std::make_pair(TEXT("myaside"), TEXT("My Aside")));
    return asides;
  }

  template <class W>
  void cWebPageController::AddAside(W& writer, const std::string& sAside) const
  {
    if (sAside == TEXT("myaside")) {
      writer.WriteLine("      <p>This is my aside. Have an article: <a href=\"http://www.iandevlin.com/blog/2011/04/html5/html5-section-or-article\">section or article?</a>.</p>");
    }
  }


  class cStringStreamWriter
  {
  public:
    explicit cStringStreamWriter(std::ostringstream& o);

    void WriteLine(const std::string& sLine);

  private:
    std::ostringstream& o;
  };

  cStringStreamWriter::cStringStreamWriter(std::ostringstream& _o) :
    o(_o)
  {
  }

  void cStringStreamWriter::WriteLine(const std::string& sLine)
  {
    o<<sLine<<"\n";
  }


  // ** cWebServer

  cWebServer::cWebServer(cGtkmmView& _view) :
    view(_view),
    mutexEntries("mutexEntries")
  {
  }

  void cWebServer::Start()
  {
    // Set our request handler
    server.SetRequestHandler(*this);

    // Run the server thread
    server.Start();
  }

  void cWebServer::Stop()
  {
    // Stop our server thread
    server.Stop();
  }

  void cWebServer::OnActionPlayTrack(trackid_t id, const spitfire::audio::cMetaData& metaData, const string_t& sFilePath)
  {
    cWebServerSongEntry entry;
    entry.id = id;
    entry.sArtist = metaData.sArtist;
    entry.sTitle = metaData.sTitle;
    entry.uiDurationMilliSeconds = metaData.uiDurationMilliSeconds;
    entry.sFilePath = sFilePath;
    entry.sFileName = spitfire::filesystem::GetFile(sFilePath);

    spitfire::util::cLockObject lock(mutexEntries);

    entries.push_front(entry);

    // Limit the number of entries
    if (entries.size() > nMaxSongEntries) {
      // Remove the last entry
      entries.pop_back();
    }
  }

  bool cWebServer::GetFilePathFromFileName(const string_t& sFileName, string_t& sFilePath)
  {
    spitfire::util::cLockObject lock(mutexEntries);

    // Find the file name in the list
    std::list<cWebServerSongEntry>::iterator iter(entries.begin());
    const std::list<cWebServerSongEntry>::iterator iterEnd(entries.end());
    while (iter != iterEnd) {
      if (iter->sFileName == sFileName) {
        sFilePath = iter->sFilePath;
        return true;
      }

      iter++;
    }

    return false;
  }

  void cWebServer::ServePlainTextContent(spitfire::network::http::cConnectedClient& connection, spitfire::network::http::STATUS status, const std::string& sContentUTF8)
  {
    // Send the response
    spitfire::network::http::cResponse response;
    response.SetStatus(status);
    response.SetContentTypeTextPlainUTF8();
    response.SetContentLengthBytes(sContentUTF8.length());
    connection.SendResponse(response);

    // Send the content
    connection.SendContent(sContentUTF8);
    connection.Write("\n\n");
  }

  bool cWebServer::IsTrackInList(trackid_t id, const std::list<cWebServerSongEntry>& knownEntries) const
  {
    std::list<cWebServerSongEntry>::const_iterator iter(knownEntries.begin());
    const std::list<cWebServerSongEntry>::const_iterator iterEnd(knownEntries.end());
    while (iter != iterEnd) {
      if (iter->id == id) return true;

      iter++;
    }

    return false;
  }

  std::list<cWebServerSongEntry> cWebServer::GetNewEntries(const std::list<cWebServerSongEntry>& knownEntries)
  {
    std::list<cWebServerSongEntry> tempEntries;

    {
      // Lock and make a copy of the entries
      spitfire::util::cLockObject lock(mutexEntries);

      // Add all the entries
      tempEntries = entries;
    }

    std::list<cWebServerSongEntry> newEntries;

    std::list<cWebServerSongEntry>::const_iterator iter(tempEntries.begin());
    const std::list<cWebServerSongEntry>::const_iterator iterEnd(tempEntries.end());
    while (iter != iterEnd) {
      if (!IsTrackInList(iter->id, knownEntries)) newEntries.push_back(*iter);

      iter++;
    }

    return newEntries;
  }


  void cWebServer::ServeEventSource(spitfire::network::http::cServer& server, spitfire::network::http::cConnectedClient& connection)
  {
    // Turn off buffering for our writes
    connection.SetNoDelay();

    // Make sure we are starting an event stream
    spitfire::network::http::cResponse response;
    response.SetContentTypeTextEventStream();
    response.SetCacheControlNoCache();
    response.SetConnectionKeepAlive();
    connection.SendResponse(response);

    std::list<cWebServerSongEntry> knownEntries;

    {
      // Lock and make a copy of the entries
      spitfire::util::cLockObject lock(mutexEntries);

      // Add all the entries
      knownEntries = entries;
    }

    while (!server.IsToStop() && connection.IsOpen()) {
      spitfire::util::SleepThisThreadMS(1000);

      std::list<cWebServerSongEntry> newEntries = GetNewEntries(knownEntries);
      if (!newEntries.empty()) {
        std::list<cWebServerSongEntry>::const_iterator iter(newEntries.begin());
        const std::list<cWebServerSongEntry>::const_iterator iterEnd(newEntries.end());
        while (iter != iterEnd) {
          LOG<<"cWebServer::HandleRequest Sending update"<<std::endl;

          // Dummy entry for testing
          const cWebServerSongEntry& entry = *iter;

          // Send an "OnActionPlayTrack" event
          connection.Write("event: OnActionPlayTrack\n");
          connection.Write("data: { ");
          connection.Write("\"id\": \"" + spitfire::string::ToString(entry.id) + "\",");
          connection.Write("\"sArtist\": \"" + entry.sArtist + "\",");
          connection.Write("\"sTitle\": \"" + entry.sTitle + "\",");
          connection.Write("\"sDurationMS\": \"" + medusa::util::FormatTime(entry.uiDurationMilliSeconds) + "\",");
          connection.Write("\"sFilePath\": \"" + spitfire::filesystem::GetFile(entry.sFilePath) + "\",");
          connection.Write("\"sFileName\": \"" + entry.sFileName + "\"");
          connection.Write(" }\n");

          connection.Write("\n");

          // Add the entry to our known list of known entries
          knownEntries.push_front(entry);

          // Limit the number of entries
          if (knownEntries.size() > nMaxSongEntries) {
            // Remove the last entry
            knownEntries.pop_back();
          }

          iter++;
        }
      }
    }
  }

  bool cWebServer::HandleRequest(spitfire::network::http::cServer& server, spitfire::network::http::cConnectedClient& connection, const spitfire::network::http::cRequest& request)
  {
    //LOG<<"cWebServer::HandleRequest method="<<(request.IsMethodGet() ? "is get" : (request.IsMethodPost() ? "is post" : "unknown"))<<", path="<<request.GetPath()<<std::endl;

    if (spitfire::string::StartsWith(request.GetPath(), "/download/")) {
      const string_t sFileName = spitfire::filesystem::GetFile(request.GetPath());
      string_t sFilePath;
      if (!GetFilePathFromFileName(sFileName, sFilePath)) {
        LOG<<"cWebServer::HandleRequest Invalid download path \""<<request.GetPath()<<"\", returning false"<<std::endl;
        return false;
      }

      server.ServeFileWithResolvedFilePath(connection, request, sFilePath);
      return true;
    }

    if (request.GetPath() == "/action") {
      // Actions sent to the server
      if (!request.IsMethodPost()) {
        LOG<<"cWebServer::HandleRequest Action used without POST"<<std::endl;
        ServePlainTextContent(connection, spitfire::network::http::STATUS::METHOD_NOT_ALLOWED, "POST must be used for all actions");
        return true;
      }

      // Deleting a track
      const std::map<std::string, std::string>& values = request.GetFormData();
      std::map<std::string, std::string>::const_iterator iter = values.find("action");
      if (iter == values.end()) {
        LOG<<"cWebServer::HandleRequest Action was not found"<<std::endl;
        ServePlainTextContent(connection, spitfire::network::http::STATUS::BAD_REQUEST, "An action must be specified");
        return true;
      }

      if (iter->second == "playback_previous") {
        // Notify the view
        view.OnWebServerPreviousTrack();
      } else if (iter->second == "playback_play") {
        // Notify the view
        view.OnWebServerPlayPause();
      } else if (iter->second == "playback_next") {
        // Notify the view
        view.OnWebServerNextTrack();
      } else if (iter->second == "volume_mute") {
        // Notify the view
        view.OnWebServerSetVolumeMute();
      } else if (iter->second == "volume_full") {
        // Notify the view
        view.OnWebServerSetVolumeFull();
      } else if (iter->second == "file_trash") {
        iter = values.find("track");
        if (iter == values.end()) {
          LOG<<"cWebServer::HandleRequest Track was not found"<<std::endl;
          ServePlainTextContent(connection, spitfire::network::http::STATUS::BAD_REQUEST, "The action \"delete\" must specify a track id in the \"id\" field");
          return true;
        }

        const std::string sTrack = iter->second;

        trackid_t track = trackid_t(uintptr_t(spitfire::string::ToUnsignedInt(sTrack)));

        // Notify the view
        view.OnWebServerTrackMoveToRubbishBin(track);
      } else {
        LOG<<"cWebServer::HandleRequest Unknown action"<<std::endl;
        ServePlainTextContent(connection, spitfire::network::http::STATUS::BAD_REQUEST, "Unknown action, valid actions are \"playback_previous\", \"playback_play\", \"playback_next\", \"volume_mute\", \"volume_full\", \"file_trash\", ");
        return true;
      }

      ServePlainTextContent(connection, spitfire::network::http::STATUS::OK, "OK Action submitted");
      return true;
    } else if (request.GetPath() == "/updates") {
      // Updates sent by the server
      // http://www.html5rocks.com/en/tutorials/eventsource/basics/

      if (!request.IsMethodGet()) {
        LOG<<"cWebServer::HandleRequest Updates used without GET"<<std::endl;
        ServePlainTextContent(connection, spitfire::network::http::STATUS::METHOD_NOT_ALLOWED, "GET must be used for the event source");
        return true;
      }

      if (request.GetAccept() != "text/event-stream") {
        LOG<<"cWebServer::HandleRequest Updates used without Accept: text/event-stream"<<std::endl;
        ServePlainTextContent(connection, spitfire::network::http::STATUS::METHOD_NOT_ALLOWED, "Updates must be used with the Accept: text/event-stream header field set");
        return true;
      }

      LOG<<"cWebServer::HandleRequest Starting event source"<<std::endl;

      ServeEventSource(server, connection);
      return true;
    } else if (request.GetPath() != "/") {
      LOG<<"cWebServer::HandleRequest Invalid path \""<<request.GetPath()<<"\", returning false"<<std::endl;
      return false;
    } else if (!request.IsMethodGet()) {
      LOG<<"cWebServer::HandleRequest Invalid method, returning false"<<std::endl;
      return false;
    }

    std::list<cWebServerSongEntry> tempEntries;

    {
      // Lock and make a copy of the entries
      spitfire::util::cLockObject lock(mutexEntries);

      // Add all the entries
      tempEntries = entries;
    }

    std::ostringstream o;

    cStringStreamWriter writer(o);
    cWebPageController controller(tempEntries);
    spitfire::storage::cHTMLDocument<cStringStreamWriter, cWebPageController> document;

    document.Create(writer, controller);

    const std::string sContentUTF8 = o.str();

    // TODO: Fill out the response from the request
    spitfire::network::http::cResponse response;
    response.SetContentLengthBytes(sContentUTF8.length());
    response.SetConnectionClose();
    connection.SendResponse(response);

    // TODO: Send actual content
    connection.SendContent(sContentUTF8);

    connection.Write("\n\n");

    return true;
  }
}
