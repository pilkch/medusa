// Standard headers
#include <iostream>

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

  cGtkmmViewEventTrackAdded::cGtkmmViewEventTrackAdded(trackid_t _id, const string_t& _sFilePath, const spitfire::audio::cMetaData& _metaData) :
    id(_id),
    sFilePath(_sFilePath),
    metaData(_metaData)
  {
  }

  void cGtkmmViewEventTrackAdded::EventFunction(cGtkmmView& view)
  {
    view.OnTrackAdded(id, sFilePath, metaData);
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

void cGtkmmView::OnActionRemoveTrack(trackid_t id)
{
  pController->RemoveTrack(id);
}

void cGtkmmView::OnActionPlayTrack(trackid_t id, const string_t& sFilePath, const spitfire::audio::cMetaData& metaData)
{
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

void cGtkmmView::OnTrackAdded(trackid_t id, const string_t& sFilePath, const spitfire::audio::cMetaData& metaData)
{
  std::wcout<<"cGtkmmView::OnTrackAdded \""<<sFilePath<<"\""<<std::endl;

  if (!spitfire::util::IsMainThread()) {
    cGtkmmViewEventTrackAdded* pEvent = new cGtkmmViewEventTrackAdded(id, sFilePath, metaData);
    eventQueue.AddItemToBack(pEvent);
    notifyMainThread.Notify();
  } else {
    pMainWindow->OnTrackAdded(id, sFilePath, metaData);
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
