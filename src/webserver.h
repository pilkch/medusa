#ifndef webserver_h
#define webserver_h

// Medusa headers
#include "medusa.h"

// Spitfire headers
#include <spitfire/audio/metadata.h>
#include <spitfire/communication/http.h>
#include <spitfire/storage/html.h>
#include <spitfire/util/mutex.h>

namespace medusa
{
  class cGtkmmView;

  struct cWebServerSongEntry
  {
  public:
    trackid_t id;
    string_t sArtist;
    string_t sTitle;
    uint64_t uiDurationMilliSeconds;
  };

  class cWebServer : public spitfire::network::http::cServerRequestHandler
  {
  public:
    explicit cWebServer(cGtkmmView& view);
    ~cWebServer();

    void Start();
    void Stop();

    void OnActionPlayTrack(trackid_t id, const spitfire::audio::cMetaData& metaData);

    void GetLastPlayedTracks(std::list<cWebServerSongEntry>& tracks);

  private:
    virtual override bool HandleRequest(spitfire::network::http::cServer& server, spitfire::network::http::cConnectedClient& connection, const spitfire::network::http::cRequest& request);

    cGtkmmView& view;

    spitfire::network::http::cServer server;

    spitfire::util::cMutex mutexEntries;
    std::list<cWebServerSongEntry*> entries;
  };
}

#endif // webserver_h
