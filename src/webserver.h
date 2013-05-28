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
    cWebServer();

    void Start();
    void Stop();

    void OnActionPlayTrack(trackid_t id, const spitfire::audio::cMetaData& metaData);

  private:
    virtual override bool HandleRequest(spitfire::network::http::cServer& server, spitfire::network::http::cConnectedClient& connection, const spitfire::network::http::cRequest& request);

    spitfire::network::http::cServer server;

    spitfire::util::cMutex mutexEntries;
    std::list<cWebServerSongEntry> entries;
  };
}

#endif // webserver_h
