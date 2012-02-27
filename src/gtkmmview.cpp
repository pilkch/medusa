// Standard headers
#include <iostream>

// Medusa headers
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

void cGtkmmView::OnActionPlayTrack(const cTrack* pTrack)
{
  player.SetTrack(pTrack->sFilePath, pTrack->metaData.uiDurationMilliSeconds);

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
  if (player.IsStopped()) mainWindow.SetStatePaused();
  else mainWindow.SetStatePlaying(player.GetTrack());
}

void cGtkmmView::OnActionPlayPause()
{
  if (player.IsStopped()) player.Play();
  else player.Pause();

  // Now we want to query the player again in case it failed to start or stop playback
  if (player.IsStopped()) mainWindow.SetStatePaused();
  else mainWindow.SetStatePlaying(player.GetTrack());
}

void cGtkmmView::OnPlayerUpdatePlaybackPosition()
{
  mainWindow.SetPlaybackPositionMS(player.GetPlaybackPositionMS());
}

void cGtkmmView::OnPlayerAboutToFinish()
{
  // TODO: Should we just tell the player which track to play and not actually start playing it yet?

  // TODO: THIS NEEDS TO BE CALLED ON THE MAIN THREAD SO WE HAVE TO USE A GTKMM DISPATCHER
  ... call this via a dispatcher: mainWindow.OnActionPlayNextTrack();
}

void cGtkmmView::_Run()
{
  // Display our window
  Gtk::Main::run(*pMainWindow);
}
}
