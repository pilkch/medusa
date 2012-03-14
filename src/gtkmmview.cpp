// Standard headers
#include <iostream>

// Medusa headers
#include "controller.h"
#include "gtkmmview.h"
#include "gtkmmmainwindow.h"

namespace medusa
{
cGtkmmView::cGtkmmView(int argc, char** argv) :
  kit(argc, argv),
  pMainWindow(nullptr),
  player(*this),
  mutexSettings("settings")
{
  settings.Load();

  pMainWindow = new cGtkmmMainWindow(*this, settings);

  player.Create(argc, argv);
}

cGtkmmView::~cGtkmmView()
{
  player.Destroy();

  delete pMainWindow;

  settings.Save();
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

void cGtkmmView::OnActionPlayTrack(const cTrack* pTrack)
{
  player.SetTrack(pTrack->sFilePath, pTrack->metaData.uiDurationMilliSeconds);

  pCurrentTrack = pTrack;

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

void cGtkmmView::OnPlayerAboutToFinish()
{
  // TODO: Should we just tell the player which track to play and not actually start playing it yet?

  // TODO: THIS NEEDS TO BE CALLED ON THE MAIN THREAD SO WE HAVE TO USE A GTKMM DISPATCHER
  ... call this via a dispatcher: pMainWindow->OnActionPlayNextTrack();
}

void cGtkmmView::OnTrackAdded(trackid_t id, const string_t& sFilePath, const spitfire::audio::cMetaData& metaData)
{
  std::wcout<<"cGtkmmView::OnTrackAdded \""<<sFilePath<<"\""<<std::endl;

  // TODO: THIS NEEDS TO BE CALLED ON THE MAIN THREAD SO WE HAVE TO USE A GTKMM DISPATCHER
  ... call this via a dispatcher: pMainWindow->OnTrackAdded(id, sFilePath, metaData);
}

void cGtkmmView::_Run()
{
  // Display our window
  Gtk::Main::run(*pMainWindow);
}
}
