// Standard headers
#include <iostream>
#include <iomanip>

// Medusa headers
#include "gtkmmview.h"
#include "gtkmmmainwindow.h"

cGtkmmMainWindow::cGtkmmMainWindow(cGtkmmView& _view) :
  view(_view),
  buttonPrevious("Previous"),
  buttonPlay("Play"),
  buttonNext("Next"),
  buttonVolume("Volume"),
  textPosition("0:00"),
  textLength("0:00"),
  dummyCategories("Categories"),
  dummyPlaylist("Playlist"),
  dummyStatusBar("StatusBar")
{
  set_title("Medusa");

  SetPlaybackLengthMS(1000);

  positionSlider.set_draw_value(false);

  positionSlider.signal_change_value().connect(sigc::mem_fun(*this, &cGtkmmMainWindow::OnPlaybackPositionChanged));

  buttonPlay.signal_clicked().connect(sigc::mem_fun(*this, &cGtkmmMainWindow::OnPlayPauseClicked));

  boxPlaybackButtons.pack_start(*Gtk::manage(new Gtk::Label()), Gtk::PACK_EXPAND_WIDGET);
  boxPlaybackButtons.pack_start(buttonPrevious, Gtk::PACK_SHRINK);
  boxPlaybackButtons.pack_start(buttonPlay, Gtk::PACK_SHRINK);
  boxPlaybackButtons.pack_start(buttonNext, Gtk::PACK_SHRINK);
  boxPlaybackButtons.pack_start(*Gtk::manage(new Gtk::Label()), Gtk::PACK_EXPAND_WIDGET);
  boxPlaybackButtons.pack_start(buttonVolume, Gtk::PACK_SHRINK);

  boxCategoriesAndPlaylist.pack_start(dummyCategories, Gtk::PACK_EXPAND_WIDGET);
  boxCategoriesAndPlaylist.pack_start(dummyPlaylist, Gtk::PACK_EXPAND_WIDGET);

  boxPositionSlider.pack_start(textPosition, Gtk::PACK_SHRINK);
  boxPositionSlider.pack_start(positionSlider, Gtk::PACK_EXPAND_WIDGET);
  boxPositionSlider.pack_start(textLength, Gtk::PACK_SHRINK);

  boxMainWindow.pack_start(boxPlaybackButtons, Gtk::PACK_SHRINK);
  boxMainWindow.pack_start(boxPositionSlider, Gtk::PACK_SHRINK);
  boxMainWindow.pack_start(boxCategoriesAndPlaylist, Gtk::PACK_EXPAND_WIDGET);
  boxMainWindow.pack_start(dummyStatusBar, Gtk::PACK_SHRINK);

  // Add the box layout to the main window
  add(boxMainWindow);

  show_all_children();
}

bool cGtkmmMainWindow::OnPlaybackPositionChanged(Gtk::ScrollType scrollType, double value)
{
  std::cout<<"cGtkmmMainWindow::OnPlaybackPositionChanged\n";

  view.OnActionPlaybackPositionChanged(value);

  return true;
}

void cGtkmmMainWindow::OnPlayPauseClicked()
{
  view.OnActionPlayPause();
}

bool cGtkmmMainWindow::OnTimerPlaybackPosition()
{
  view.OnActionTimerUpdatePlaybackPosition();

  return true;
}

std::string cGtkmmMainWindow::TimeToString(uint64_t milliseconds) const
{
  std::ostringstream o;

  o<<std::right<<std::setfill('0');

  uint64_t time = milliseconds / 1000;

  const uint64_t seconds = time % 60;
  time /= 60;

  const uint64_t minutes = time % 60;
  time /= 60;

  const uint64_t hours = time % 24;
  time /= 24;

  const uint64_t days = hours;

  if (days != 0) {
    o<<days<<":";
    o<<std::setw(2); // Set width of 2 for the next value
  }

  if (hours != 0)  {
    o<<hours<<":";
    o<<std::setw(2); // Set width of 2 for the next value
  }

  o<<minutes<<":";
  o<<std::setw(2)<<seconds;

  return o.str();
}

void cGtkmmMainWindow::SetPlaybackPositionMS(uint64_t milliseconds)
{
  std::cout<<"cGtkmmMainWindow::SetPlaybackPositionMS "<<milliseconds<<"\n";
  positionSlider.set_value(double(milliseconds) / 1000.0f);

  textPosition.set_text(TimeToString(milliseconds).c_str());
}

void cGtkmmMainWindow::SetPlaybackLengthMS(uint64_t milliseconds)
{
  std::cout<<"cGtkmmMainWindow::SetPlaybackLengthMS "<<milliseconds<<"\n";
  positionSlider.set_range(0, double(milliseconds) / 1000.0f);
  positionSlider.set_increments(5, 5);

  textLength.set_text(TimeToString(milliseconds).c_str());
}

void cGtkmmMainWindow::SetStatePlaying()
{
  buttonPlay.set_label("Pause");

  // Call OnTimerPlaybackPosition function at a 200ms
  // interval to regularly update the position of the stream
  m_timeout_connection = Glib::signal_timeout().connect(sigc::mem_fun(*this, &cGtkmmMainWindow::OnTimerPlaybackPosition), 200);
}

void cGtkmmMainWindow::SetStatePaused()
{
  buttonPlay.set_label("Play");

  // Disconnect the progress signal handler:
  m_timeout_connection.disconnect();
}
