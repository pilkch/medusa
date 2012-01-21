// Standard headers
#include <iostream>

// Medusa headers
#include "gtkmmview.h"
#include "gtkmmmainwindow.h"

cGtkmmView::cGtkmmView(int argc, char** argv) :
  kit(argc, argv),
  mainWindow(*this),
  player(*this)
{
  player.Create(argc, argv);
}

cGtkmmView::~cGtkmmView()
{
  player.Destroy();
}

void cGtkmmView::OnActionPlayTrack(const cTrack* pTrack)
{
  player.SetTrack(pTrack);

  OnActionPlay();
}

void cGtkmmView::OnActionPlaybackPositionChanged(uint64_t seconds)
{
  player.SeekMS(seconds * 1000);
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

void cGtkmmView::OnActionTimerUpdatePlaybackPosition()
{
  mainWindow.SetPlaybackPositionMS(player.GetPlaybackPositionMS());
}

void cGtkmmView::_Run()
{
  // Display our window
  Gtk::Main::run(mainWindow);
}
