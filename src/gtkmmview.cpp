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

  void cGtkmmViewEventPlayerError::EventFunction(cGtkmmView& view)
  {
    view.OnPlayerError();
  }

  cGtkmmViewEventLoadingFilesToLoadIncrement::cGtkmmViewEventLoadingFilesToLoadIncrement(size_t _nFiles) :
    nFiles(_nFiles)
  {
  }

  void cGtkmmViewEventLoadingFilesToLoadIncrement::EventFunction(cGtkmmView& view)
  {
    view.OnLoadingFilesToLoadIncrement(nFiles);
  }

  cGtkmmViewEventLoadingFilesToLoadDecrement::cGtkmmViewEventLoadingFilesToLoadDecrement(size_t _nFiles) :
    nFiles(_nFiles)
  {
  }

  void cGtkmmViewEventLoadingFilesToLoadDecrement::EventFunction(cGtkmmView& view)
  {
    view.OnLoadingFilesToLoadDecrement(nFiles);
  }

  void cGtkmmViewEventPlaylistLoading::EventFunction(cGtkmmView& view)
  {
    view.OnPlaylistLoading();
  }

  cGtkmmViewEventPlaylistLoaded::cGtkmmViewEventPlaylistLoaded(trackid_t _idLastPlayed) :
    idLastPlayed(_idLastPlayed)
  {
  }

  void cGtkmmViewEventPlaylistLoaded::EventFunction(cGtkmmView& view)
  {
    view.OnPlaylistLoaded(idLastPlayed);
  }

  cGtkmmViewEventTracksAdded::cGtkmmViewEventTracksAdded(const std::list<trackid_t>& _ids, const std::list<cTrack*>& _tracks) :
    ids(_ids),
    tracks(_tracks)
  {
  }

  void cGtkmmViewEventTracksAdded::EventFunction(cGtkmmView& view)
  {
    view.OnTracksAdded(ids, tracks);
  }


  void cGtkmmViewEventWebServerPreviousTrack::EventFunction(cGtkmmView& view)
  {
    view.OnWebServerPreviousTrack();
  }

  void cGtkmmViewEventWebServerPlayPause::EventFunction(cGtkmmView& view)
  {
    view.OnWebServerPlayPause();
  }

  cGtkmmViewEventWebServerPlayTrack::cGtkmmViewEventWebServerPlayTrack(trackid_t _id) :
    id(_id)
  {
  }

  void cGtkmmViewEventWebServerPlayTrack::EventFunction(cGtkmmView& view)
  {
    view.OnWebServerPlayTrack(id);
  }

  void cGtkmmViewEventWebServerNextTrack::EventFunction(cGtkmmView& view)
  {
    view.OnWebServerNextTrack();
  }

  void cGtkmmViewEventWebServerSetVolumeMute::EventFunction(cGtkmmView& view)
  {
    view.OnWebServerSetVolumeMute();
  }

  void cGtkmmViewEventWebServerSetVolumeFull::EventFunction(cGtkmmView& view)
  {
    view.OnWebServerSetVolumeFull();
  }

  cGtkmmViewEventWebServerTrackMoveToRubbishBin::cGtkmmViewEventWebServerTrackMoveToRubbishBin(trackid_t _id) :
    id(_id)
  {
  }

  void cGtkmmViewEventWebServerTrackMoveToRubbishBin::EventFunction(cGtkmmView& view)
  {
    view.OnWebServerTrackMoveToRubbishBin(id);
  }

  cGtkmmViewEventUpdateCheckerNewVersionFound::cGtkmmViewEventUpdateCheckerNewVersionFound(int _iMajorVersion, int _iMinorVersion, const string_t& _sDownloadPage) :
    iMajorVersion(_iMajorVersion),
    iMinorVersion(_iMinorVersion),
    sDownloadPage(_sDownloadPage)
  {
  }

  void cGtkmmViewEventUpdateCheckerNewVersionFound::EventFunction(cGtkmmView& view)
  {
    view.OnNewVersionFound(iMajorVersion, iMinorVersion, sDownloadPage);
  }


  // ** cGtkmmView

cGtkmmView::cGtkmmView(int argc, char** argv) :
  kit(argc, argv),
  pMainWindow(nullptr),
  player(*this),
  webServer(*this),
  pCurrentTrack(nullptr),
  soAction("cGtkmmView_soAction"),
  eventQueue(soAction),
  mutexSettings("settings")
{
  if (argc >= 1) sExecutableFolder = spitfire::filesystem::GetFolder(spitfire::string::ToString_t(argv[0]));

  InstallDesktopFile();

  settings.Load();

  pMainWindow = new cGtkmmMainWindow(*this, settings);

  player.Create(argc, argv);

  notifyMainThread.Create(*this, &cGtkmmView::OnNotify);
}

cGtkmmView::~cGtkmmView()
{
  // Stop our web server
  if (webServer.IsRunning()) webServer.Stop();

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

  // Tell our control that the mainwindow is ready
  assert(pController != nullptr);
  pController->OnActionCreated();
}

void cGtkmmView::OnActionMainWindowQuitSoon()
{
  pController->OnActionQuitSoon();
}

  void cGtkmmView::OnActionMainWindowApplySettings()
  {
    // Make sure that the web server reflects the settings
    const bool bIsWebServerEnabled = settings.IsWebServerEnabled();
    if (bIsWebServerEnabled && !webServer.IsRunning()) {
      // We can now start our web server thread
      webServer.Start();
    } else if (!bIsWebServerEnabled && webServer.IsRunning()) {
      // We can now stop our web server thread
      webServer.Stop();
    }
  }

void cGtkmmView::OnActionMainWindowQuitNow()
{
  pController->OnActionQuitNow();
}

  bool cGtkmmView::IsPlaying() const
  {
    return player.IsPlaying();
  }

void cGtkmmView::OnActionAddTracks(const std::list<string_t>& files)
{
  pController->AddTracks(files);
}

void cGtkmmView::OnActionAddTracksFromFolder(const string_t& sFolderPath)
{
  pController->AddTracksFromFolder(sFolderPath);
}

  #ifdef BUILD_MEDUSA_IMPORT_BANSHEE_PLAYLIST
  void cGtkmmView::OnActionImportFromBanshee()
  {
    pController->ImportFromBanshee();
  }
  #endif

  void cGtkmmView::OnActionImportFromRhythmbox()
  {
    pController->ImportFromRhythmbox();
  }

  void cGtkmmView::OnActionStopLoading()
  {
    pController->StopLoading();
  }

  void cGtkmmView::OnActionRemoveTracks(const std::list<trackid_t>& tracks)
  {
    pController->RemoveTracks(tracks);
  }

  void cGtkmmView::OnActionRemoveTracks(const std::unordered_set<trackid_t>& tracks)
  {
    pController->RemoveTracks(tracks);
  }

  void cGtkmmView::OnActionTrackMoveToFolder(trackid_t id, const string_t& sFilePath)
  {
    pController->UpdateTrackFilePath(id, sFilePath);
  }

void cGtkmmView::OnActionPlayTrack(trackid_t id, const string_t& sFilePath, const spitfire::audio::cMetaData& metaData)
{
  player.SetTrack(sFilePath, metaData.uiDurationMilliSeconds);

  pCurrentTrack = id;

  OnActionPlay();

  webServer.OnActionPlayTrack(id, metaData, sFilePath);

  pController->OnActionPlayTrack(id);
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

  pMainWindow->SetStatePlaying(pCurrentTrack);
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

  void cGtkmmView::OnPlayerError()
  {
    if (!spitfire::util::IsMainThread()) {
      cGtkmmViewEventPlayerError* pEvent = new cGtkmmViewEventPlayerError;
      eventQueue.AddItemToBack(pEvent);
      notifyMainThread.Notify();
    } else {
      // Pause at the current track
      pMainWindow->SetStatePaused();

      // Skip to the next track
      pMainWindow->OnActionPlayNextTrack();
    }
  }

  void cGtkmmView::OnLoadingFilesToLoadIncrement(size_t nFiles)
  {
    if (!spitfire::util::IsMainThread()) {
      cGtkmmViewEventLoadingFilesToLoadIncrement* pEvent = new cGtkmmViewEventLoadingFilesToLoadIncrement(nFiles);
      eventQueue.AddItemToBack(pEvent);
      notifyMainThread.Notify();
    } else {
      pMainWindow->OnLoadingFilesToLoadIncrement(nFiles);
    }
  }

  void cGtkmmView::OnLoadingFilesToLoadDecrement(size_t nFiles)
  {
    if (!spitfire::util::IsMainThread()) {
      cGtkmmViewEventLoadingFilesToLoadDecrement* pEvent = new cGtkmmViewEventLoadingFilesToLoadDecrement(nFiles);
      eventQueue.AddItemToBack(pEvent);
      notifyMainThread.Notify();
    } else {
      pMainWindow->OnLoadingFilesToLoadDecrement(nFiles);
    }
  }

  void cGtkmmView::OnPlaylistLoading()
  {
    if (!spitfire::util::IsMainThread()) {
      cGtkmmViewEventPlaylistLoading* pEvent = new cGtkmmViewEventPlaylistLoading;
      eventQueue.AddItemToBack(pEvent);
      notifyMainThread.Notify();
    } else {
      pMainWindow->OnPlaylistLoading();
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

void cGtkmmView::OnTracksAdded(const std::list<trackid_t>& ids, const std::list<cTrack*>& tracks)
{
  std::cout<<"cGtkmmView::OnTracksAdded"<<std::endl;

  if (!spitfire::util::IsMainThread()) {
    cGtkmmViewEventTracksAdded* pEvent = new cGtkmmViewEventTracksAdded(ids, tracks);
    eventQueue.AddItemToBack(pEvent);
    notifyMainThread.Notify();
  } else {
    pMainWindow->OnTracksAdded(ids, tracks);
  }
}

  void cGtkmmView::OnWebServerPreviousTrack()
  {
    std::cout<<"cGtkmmView::OnWebServerPreviousTrack"<<std::endl;

    if (!spitfire::util::IsMainThread()) {
      cGtkmmViewEventWebServerPreviousTrack* pEvent = new cGtkmmViewEventWebServerPreviousTrack();
      eventQueue.AddItemToBack(pEvent);
      notifyMainThread.Notify();
    } else {
      pMainWindow->OnWebServerPreviousTrack();
    }
  }

  void cGtkmmView::OnWebServerPlayPause()
  {
    std::cout<<"cGtkmmView::OnWebServerPlayPause"<<std::endl;

    if (!spitfire::util::IsMainThread()) {
      cGtkmmViewEventWebServerPlayPause* pEvent = new cGtkmmViewEventWebServerPlayPause();
      eventQueue.AddItemToBack(pEvent);
      notifyMainThread.Notify();
    } else {
      pMainWindow->OnWebServerPlayPause();
    }
  }

  void cGtkmmView::OnWebServerPlayTrack(trackid_t id)
  {
    LOG<<"cGtkmmView::OnWebServerPlayTrack"<<std::endl;

    if (!spitfire::util::IsMainThread()) {
      cGtkmmViewEventWebServerPlayTrack* pEvent = new cGtkmmViewEventWebServerPlayTrack(id);
      eventQueue.AddItemToBack(pEvent);
      notifyMainThread.Notify();
    } else {
      pMainWindow->OnWebServerPlayTrack(id);
    }
  }

  void cGtkmmView::OnWebServerNextTrack()
  {
    std::cout<<"cGtkmmView::OnWebServerNextTrack"<<std::endl;

    if (!spitfire::util::IsMainThread()) {
      cGtkmmViewEventWebServerNextTrack* pEvent = new cGtkmmViewEventWebServerNextTrack();
      eventQueue.AddItemToBack(pEvent);
      notifyMainThread.Notify();
    } else {
      pMainWindow->OnWebServerNextTrack();
    }
  }

  void cGtkmmView::OnWebServerSetVolumeMute()
  {
    std::cout<<"cGtkmmView::OnWebServerSetVolumeMute"<<std::endl;

    if (!spitfire::util::IsMainThread()) {
      cGtkmmViewEventWebServerSetVolumeMute* pEvent = new cGtkmmViewEventWebServerSetVolumeMute();
      eventQueue.AddItemToBack(pEvent);
      notifyMainThread.Notify();
    } else {
      pMainWindow->OnWebServerSetVolumeMute();
    }
  }

  void cGtkmmView::OnWebServerSetVolumeFull()
  {
    std::cout<<"cGtkmmView::OnWebServerSetVolumeFull"<<std::endl;

    if (!spitfire::util::IsMainThread()) {
      cGtkmmViewEventWebServerSetVolumeFull* pEvent = new cGtkmmViewEventWebServerSetVolumeFull();
      eventQueue.AddItemToBack(pEvent);
      notifyMainThread.Notify();
    } else {
      pMainWindow->OnWebServerSetVolumeFull();
    }
  }

  void cGtkmmView::OnWebServerTrackMoveToRubbishBin(trackid_t id)
  {
    std::cout<<"cGtkmmView::OnWebServerTrackMoveToRubbishBin"<<std::endl;

    if (!spitfire::util::IsMainThread()) {
      cGtkmmViewEventWebServerTrackMoveToRubbishBin* pEvent = new cGtkmmViewEventWebServerTrackMoveToRubbishBin(id);
      eventQueue.AddItemToBack(pEvent);
      notifyMainThread.Notify();
    } else {
      pMainWindow->OnWebServerTrackMoveToRubbishBin(id);
    }
  }

  void cGtkmmView::OnNewVersionFound(int iNewMajorVersion, int iNewMinorVersion, const string_t& sDownloadPage)
  {
    std::cout<<"cGtkmmView::OnNewVersionFound"<<std::endl;

    if (!spitfire::util::IsMainThread()) {
      cGtkmmViewEventUpdateCheckerNewVersionFound* pEvent = new cGtkmmViewEventUpdateCheckerNewVersionFound(iNewMajorVersion, iNewMinorVersion, sDownloadPage);
      eventQueue.AddItemToBack(pEvent);
      notifyMainThread.Notify();
    } else {
      pMainWindow->OnNewVersionFound(iNewMajorVersion, iNewMinorVersion, sDownloadPage);
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
