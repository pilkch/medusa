// Standard headers
#include <iostream>

// Medusa headers
#include "gtkmmview.h"
#include "gtkmmmainwindow.h"

cGtkmmMainWindow::cGtkmmMainWindow(cGtkmmView& _view) :
  view(_view),
  button("Play")
{
  set_title("Medusa");

  SetPlaybackLengthMS(1000);

  positionSlider.signal_change_value().connect(sigc::mem_fun(*this, &cGtkmmMainWindow::OnPlaybackPositionChanged));

  button.signal_clicked().connect(sigc::mem_fun(*this, &cGtkmmMainWindow::OnPlayPauseClicked));

  vBox.pack_start(positionSlider, Gtk::PACK_SHRINK);
  vBox.pack_start(button, Gtk::PACK_SHRINK);

  // Add the box layout to the main window
  add(vBox);

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

void cGtkmmMainWindow::SetPlaybackPositionMS(uint64_t milliseconds)
{
  std::cout<<"cGtkmmMainWindow::SetPlaybackPositionMS "<<milliseconds<<"\n";
  positionSlider.set_value(double(milliseconds) / 1000.0f);
}

void cGtkmmMainWindow::SetPlaybackLengthMS(uint64_t milliseconds)
{
  std::cout<<"cGtkmmMainWindow::SetPlaybackLengthMS "<<milliseconds<<"\n";
  positionSlider.set_range(0, double(milliseconds) / 1000.0f);
  positionSlider.set_increments(5, 5);
}

void cGtkmmMainWindow::SetStatePlaying()
{
  button.set_label("Pause");

  // Call OnTimerPlaybackPosition function at a 200ms
  // interval to regularly update the position of the stream
  m_timeout_connection = Glib::signal_timeout().connect(sigc::mem_fun(*this, &cGtkmmMainWindow::OnTimerPlaybackPosition), 200);
}

void cGtkmmMainWindow::SetStatePaused()
{
  button.set_label("Play");

  // Disconnect the progress signal handler:
  m_timeout_connection.disconnect();
}
