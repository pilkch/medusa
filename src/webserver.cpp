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
    template <class W>
    void AddFormWithImageButton(W& writer, trackid_t track, const std::string& sAction, const std::string& sImage, const std::string& sAlternativeText, size_t nSize) const;

    std::list<cWebServerSongEntry> tracks;
  };

  cWebPageController::cWebPageController(const std::list<cWebServerSongEntry>& _tracks) :
    tracks(_tracks)
  {
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
  void cWebPageController::AddFormWithImageButton(W& writer, trackid_t track, const std::string& sAction, const std::string& sImage, const std::string& sAlternativeText, size_t nSize) const
  {
    writer.WriteLine("      <form action=\"/\" method=\"POST\">");
    writer.WriteLine("        <input type=\"hidden\" name=\"action\" value=\"" + sAction + "\"/>");
    writer.WriteLine("        <input type=\"hidden\" name=\"track\" value=\"" + spitfire::string::ToString(uintptr_t(track)) + "\"/>");
    writer.WriteLine("        <input type=\"image\" src=\"images/" + sImage + ".png\" alt=\"" + sAlternativeText + "\" width=\"" + spitfire::string::ToString(nSize) + "\" height=\"" + spitfire::string::ToString(nSize) + "\"/>");
    writer.WriteLine("      </form>");
  }

  template <class W>
  void cWebPageController::AddFormWithImageButton(W& writer, const std::string& sAction, const std::string& sImage, const std::string& sAlternativeText, size_t nSize) const
  {
    writer.WriteLine("      <form action=\"/\" method=\"POST\">");
    writer.WriteLine("        <input type=\"hidden\" name=\"action\" value=\"" + sAction + "\"/>");
    writer.WriteLine("        <input type=\"image\" src=\"images/" + sImage + ".png\" alt=\"" + sAlternativeText + "\" width=\"" + spitfire::string::ToString(nSize) + "\" height=\"" + spitfire::string::ToString(nSize) + "\"/>");
    writer.WriteLine("      </form>");
  }

  template <class W>
  void cWebPageController::AddArticle(W& writer, const std::string& sArticle) const
  {
    if (sArticle == TEXT("playback")) {
      writer.WriteLine("<img src=\"images/medusa.png\"/><br/>");

      const size_t nSize = 32;
      AddFormWithImageButton(writer, "playback_previous", "playback_previous", "Previous", nSize);
      AddFormWithImageButton(writer, "playback_play", "playback_play", "Play", nSize);
      AddFormWithImageButton(writer, "playback_next", "playback_next", "Next", nSize);
      AddFormWithImageButton(writer, "volume_mute", "volume_mute", "Mute Volume", nSize);
      AddFormWithImageButton(writer, "volume_full", "volume_full", "Full Volume", nSize);
    } else if (sArticle == TEXT("tracks")) {
      writer.WriteLine("<h3>Last " + spitfire::string::ToString(nMaxSongEntries) + " Songs Played</h3>");

      if (tracks.empty()) {
        writer.WriteLine("No songs have been played yet");
      } else {
        writer.WriteLine("<table class=\"table_border\">");
        writer.WriteLine("  <tr class=\"table_heading\">");
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
          writer.WriteLine("    </th>");
          writer.WriteLine("    <th class=\"table_border\">");
          AddFormWithImageButton(writer, iter->id, "file_trash", "file_trash", "Delete File", nSize);
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

  cWebServer::~cWebServer()
  {
    // Delete the entries
    std::list<cWebServerSongEntry*>::iterator iter(entries.begin());
    const std::list<cWebServerSongEntry*>::iterator iterEnd(entries.end());
    while (iter != iterEnd) {
      spitfire::SAFE_DELETE(*iter);

      iter++;
    }
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

  void cWebServer::OnActionPlayTrack(trackid_t id, const spitfire::audio::cMetaData& metaData)
  {
    spitfire::util::cLockObject lock(mutexEntries);

    cWebServerSongEntry* pEntry = new cWebServerSongEntry;
    pEntry->id = id;
    pEntry->sArtist = metaData.sArtist;
    pEntry->sTitle = metaData.sTitle;
    pEntry->uiDurationMilliSeconds = metaData.uiDurationMilliSeconds;

    entries.push_front(pEntry);

    // Limit the number of entries
    if (entries.size() > nMaxSongEntries) {
      // Remove the last entry
      pEntry = entries.back();
      entries.pop_back();

      // Delete the entry
      spitfire::SAFE_DELETE(pEntry);
    }
  }

  bool cWebServer::HandleRequest(spitfire::network::http::cServer& server, spitfire::network::http::cConnectedClient& connection, const spitfire::network::http::cRequest& request)
  {
    //LOG<<"cWebServer::HandleRequest method="<<(request.IsMethodGet() ? "is get" : (request.IsMethodPost() ? "is post" : "unknown"))<<", path="<<request.GetPath()<<std::endl;

    if (request.GetPath() != "/") {
      LOG<<"cWebServer::HandleRequest Invalid path, returning false"<<std::endl;
      return false;
    }

    trackid_t track = nullptr;

    if (request.IsMethodPost()) {
      // Deleting a track
      const std::map<std::string, std::string>& values = request.GetFormData();
      std::map<std::string, std::string>::const_iterator iter = values.find("action");
      if (iter == values.end()) {
        LOG<<"cWebServer::HandleRequest Action was not found, returning false"<<std::endl;
        return false;
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
          LOG<<"cWebServer::HandleRequest Track was not found, returning false"<<std::endl;
          return false;
        }

        const std::string sTrack = iter->second;

        track = trackid_t(uintptr_t(spitfire::string::ToUnsignedInt(sTrack)));

        // Notify the view
        view.OnWebServerTrackMoveToRubbishBin(track);
      } else {
        LOG<<"cWebServer::HandleRequest Unknown action, returning false"<<std::endl;
        return false;
      }
    } else if (!request.IsMethodGet()) {
      return false;
    }

    std::list<cWebServerSongEntry> tempEntries;

    {
      // Lock and make a copy of the entries
      spitfire::util::cLockObject lock(mutexEntries);

      // If we have a track to delete then we need to remove it from our list
      if (track != nullptr) {
        std::list<cWebServerSongEntry*>::iterator iter(entries.begin());
        const std::list<cWebServerSongEntry*>::iterator iterEnd(entries.end());
        while (iter != iterEnd) {
          if ((*iter)->id == track) {
            entries.remove(*iter);
            break;
          }

          iter++;
        }
      }

      // Add all the entries
      std::list<cWebServerSongEntry*>::const_iterator iter(entries.begin());
      const std::list<cWebServerSongEntry*>::const_iterator iterEnd(entries.end());
      while (iter != iterEnd) {
        tempEntries.push_back(*(*iter));

        iter++;
      }
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
    connection.SendResponse(response);

    // TODO: Send actual content
    connection.SendContent(sContentUTF8);

    connection.Write("\n\n");

    return true;
  }
}
