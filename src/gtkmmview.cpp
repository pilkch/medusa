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

void cGtkmmView::OnActionPlayPause()
{
  if (player.IsStopped()) player.Play();
  else player.Pause();

  // Now we want to query the player again in case it failed to start or stop playback
  if (player.IsStopped()) mainWindow.SetStatePaused();
  else mainWindow.SetStatePlaying();
}

void cGtkmmView::_Run()
{
  player.SetFile(TEXT("/home/chris/Music/collection/classic rock/Jefferson Airplane - White Rabbit.mp3"));

  OnActionPlayPause();

  // Display our window
  Gtk::Main::run(mainWindow);
}
