// Standard headers
#include <iostream>

// Spitfire headers
#include <spitfire/storage/filesystem.h>

// Medusa headers
#include "controller.h"
#include "gtkmmview.h"
#include "gtkmmmainwindow.h"

namespace medusa
{
  void cGtkmmViewEventPlayerAboutToFinish::EventFunction(cGtkmmView& view)
  {
    view.OnPlayerAboutToFinish();
  }

  cGtkmmViewEventTrackAdded::cGtkmmViewEventTrackAdded(trackid_t _id, const cTrack& _track) :
    id(_id),
    track(_track)
  {
  }

  void cGtkmmViewEventTrackAdded::EventFunction(cGtkmmView& view)
  {
    view.OnTrackAdded(id, track);
  }

  cGtkmmViewEventTracksAdded::cGtkmmViewEventTracksAdded(const std::vector<cTrack*>& _tracks) :
    tracks(_tracks)
  {
  }

  void cGtkmmViewEventTracksAdded::EventFunction(cGtkmmView& view)
  {
    view.OnTracksAdded(tracks);
  }

  cGtkmmViewEventPlaylistLoaded::cGtkmmViewEventPlaylistLoaded(trackid_t _idLastPlayed) :
    idLastPlayed(_idLastPlayed)
  {
  }

  void cGtkmmViewEventPlaylistLoaded::EventFunction(cGtkmmView& view)
  {
    view.OnPlaylistLoaded(idLastPlayed);
  }


  // ** cGtkmmView

cGtkmmView::cGtkmmView(int argc, char** argv) :
  kit(argc, argv),
  pMainWindow(nullptr),
  player(*this),
  soAction("cGtkmmView_soAction"),
  eventQueue(soAction),
  mutexSettings("settings")
{
  if (argc >= 1) sExecutableFolder = spitfire::filesystem::GetPath(spitfire::string::ToString_t(argv[0]));

  InstallDesktopFile();

  settings.Load();

  pMainWindow = new cGtkmmMainWindow(*this, settings);

  player.Create(argc, argv);

  notifyMainThread.Create(*this, &cGtkmmView::OnNotify);
}

cGtkmmView::~cGtkmmView()
{
  // Destroy any further events
  while (true) {
    cGtkmmViewEvent* pEvent = eventQueue.RemoveItemFromFront();
    if (pEvent == nullptr) break;
    else spitfire::SAFE_DELETE(pEvent);
  }

  player.Destroy();

  delete pMainWindow;

  settings.Save();
}

  void cGtkmmView::InstallDesktopFile()
  {
    const string_t sDesktopFilePath = spitfire::filesystem::GetHomeDirectory() + TEXT("/.local/share/applications/medusa.desktop");
    std::cout<<"cGtkmmView::InstallDesktopFile \""<<sDesktopFilePath<<"\""<<std::endl;
    std::ofstream o(spitfire::string::ToUTF8(sDesktopFilePath));

    const string_t sApplicationFolder = sExecutableFolder;

    o<<"#!/usr/bin/env xdg-open"<<std::endl;
    o<<"[Desktop Entry]"<<std::endl;
    o<<"Name=Medusa"<<std::endl;
    o<<"GenericName=Music Player"<<std::endl;
    o<<"X-GNOME-FullName=Medusa Music Player"<<std::endl;
    o<<"Comment=Play and organize your music collection"<<std::endl;
    o<<"Version="<<BUILD_APPLICATION_VERSION_STRING<<std::endl;
    o<<"Type=Application"<<std::endl;
    //o<<"Exec="<<spitfire::string::ToUTF8(sApplicationFolder)<<"/medusa %U"<<std::endl;
    //o<<"Exec=medusa %U"<<std::endl;
    //o<<"TryExec="<<spitfire::string::ToUTF8(sApplicationFolder)<<"/medusa"<<std::endl;
    o<<"Terminal=false"<<std::endl;
    o<<"Path="<<spitfire::string::ToUTF8(sApplicationFolder)<<std::endl;
    //o<<"Icon=medusa"<<std::endl;
    o<<"Icon="<<spitfire::string::ToUTF8(sApplicationFolder)<<"data/application_256x256.png"<<std::endl;
    o<<"Categories=GNOME;GTK;AudioVideo;"<<std::endl;
    //o<<"MimeType=application/x-ogg;application/ogg;audio/x-vorbis+ogg;audio/x-scpls;audio/x-mp3;audio/x-mpeg;audio/mpeg;audio/x-mpegurl;audio/x-flac;"<<std::endl;
    //o<<"MimeType=audio/x-scpls;audio/x-mp3;audio/x-mpeg;audio/mpeg;audio/x-wav;"<<std::endl;
    o<<"StartupNotify=true"<<std::endl;
    o<<"X-Ubuntu-Gettext-Domain=medusa"<<std::endl;
  }

void cGtkmmView::OnActionMainWindowCreated()
{
  std::cout<<"cGtkmmView::OnActionMainWindowCreated"<<std::endl;
  assert(pController != nullptr);
  pController->OnActionCreated();
}

void cGtkmmView::OnActionMainWindowQuitSoon()
{
  pController->OnActionQuitSoon();
}

void cGtkmmView::OnActionMainWindowQuitNow()
{
  pController->OnActionQuitNow();
}

void cGtkmmView::OnActionAddTrack(const string_t& sFilePath)
{
  pController->AddTrack(sFilePath);
}

void cGtkmmView::OnActionAddTracks(const std::vector<string_t>& files)
{
  pController->AddTracks(files);
}

void cGtkmmView::OnActionAddTracksFromFolder(const string_t& sFolderPath)
{
  pController->AddTracksFromFolder(sFolderPath);
}

  void cGtkmmView::OnActionTrackMoveToFolder(trackid_t id, const string_t& sFilePath)
  {
    pController->UpdateTrackFilePath(id, sFilePath);
  }

void cGtkmmView::OnActionRemoveTrack(trackid_t id)
{
  pController->RemoveTrack(id);
}

void cGtkmmView::OnActionPlayTrack(trackid_t id, const string_t& sFilePath, const spitfire::audio::cMetaData& metaData)
{
  pController->OnActionPlayTrack(id);

  player.SetTrack(sFilePath, metaData.uiDurationMilliSeconds);

  pCurrentTrack = id;

  OnActionPlay();
}

void cGtkmmView::OnActionPlaybackPositionChanged(uint64_t seconds)
{
  player.SeekMS(seconds * 1000);
}

void cGtkmmView::OnActionVolumeChanged(unsigned int uiVolume0To100)
{
  player.SetVolume0To100(uiVolume0To100);
}

void cGtkmmView::OnActionPlay()
{
  if (player.IsStopped()) player.Play();

  // Now we want to query the player again in case it failed to start or stop playback
  if (player.IsStopped()) pMainWindow->SetStatePaused();
  else pMainWindow->SetStatePlaying(pCurrentTrack);
}

void cGtkmmView::OnActionPlayPause()
{
  if (player.IsStopped()) player.Play();
  else player.Pause();

  // Now we want to query the player again in case it failed to start or stop playback
  if (player.IsStopped()) pMainWindow->SetStatePaused();
  else pMainWindow->SetStatePlaying(pCurrentTrack);
}

void cGtkmmView::OnPlayerUpdatePlaybackPosition()
{
  pMainWindow->SetPlaybackPositionMS(player.GetPlaybackPositionMS());
}

void cGtkmmView::OnNotify()
{
  std::cout<<"cGtkmmView::OnNotify"<<std::endl;
  assert(spitfire::util::IsMainThread());
  cGtkmmViewEvent* pEvent = eventQueue.RemoveItemFromFront();
  if (pEvent != nullptr) {
    pEvent->EventFunction(*this);
    spitfire::SAFE_DELETE(pEvent);
  }
}

void cGtkmmView::OnPlayerAboutToFinish()
{
  if (!spitfire::util::IsMainThread()) {
    cGtkmmViewEventPlayerAboutToFinish* pEvent = new cGtkmmViewEventPlayerAboutToFinish;
    eventQueue.AddItemToBack(pEvent);
    notifyMainThread.Notify();
  } else {
    // TODO: Should we just tell the player which track to play and not actually start playing it yet?

    pMainWindow->OnActionPlayNextTrack();
  }
}

void cGtkmmView::OnTrackAdded(trackid_t id, const cTrack& track)
{
  std::cout<<"cGtkmmView::OnTrackAdded \""<<track.sFilePath<<"\""<<std::endl;

  if (!spitfire::util::IsMainThread()) {
    cGtkmmViewEventTrackAdded* pEvent = new cGtkmmViewEventTrackAdded(id, track);
    eventQueue.AddItemToBack(pEvent);
    notifyMainThread.Notify();
  } else {
    pMainWindow->OnTrackAdded(id, track);
  }
}

void cGtkmmView::OnTracksAdded(const std::vector<cTrack*>& tracks)
{
  std::cout<<"cGtkmmView::OnTracksAdded"<<std::endl;

  if (!spitfire::util::IsMainThread()) {
    cGtkmmViewEventTracksAdded* pEvent = new cGtkmmViewEventTracksAdded(tracks);
    eventQueue.AddItemToBack(pEvent);
    notifyMainThread.Notify();
  } else {
    const size_t n = tracks.size();
    for (size_t i = 0; i < n; i++) {
      const cTrack* pTrack = tracks[i];
      ASSERT(pTrack != nullptr);
      pMainWindow->OnTrackAdded(pTrack, *pTrack);
    }
  }
}

  void cGtkmmView::OnPlaylistLoaded(trackid_t idLastPlayed)
  {
    if (!spitfire::util::IsMainThread()) {
      cGtkmmViewEventPlaylistLoaded* pEvent = new cGtkmmViewEventPlaylistLoaded(idLastPlayed);
      eventQueue.AddItemToBack(pEvent);
      notifyMainThread.Notify();
    } else {
      pMainWindow->OnPlaylistLoaded(idLastPlayed);
    }
  }

void cGtkmmView::_Run()
{
  // Listen for the first signal idle event to start our model thread
  Glib::signal_idle().connect(sigc::bind_return(sigc::mem_fun(*this, &cGtkmmView::OnActionMainWindowCreated), false));

  // Display our window
  Gtk::Main::run(*pMainWindow);
}
}
