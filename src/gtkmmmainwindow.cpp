// Medusa headers
#include "gtkmmview.h"
#include "gtkmmmainwindow.h"

cGtkmmMainWindow::cGtkmmMainWindow(cGtkmmView& _view) :
  view(_view),
  button("Play")
{
  set_title("Medusa");

  button.signal_clicked().connect(sigc::mem_fun(*this, &cGtkmmMainWindow::OnPlayPauseClicked));

  //add the button to the main window
  add(button);

  show_all_children();
}

void cGtkmmMainWindow::OnPlayPauseClicked()
{
  view.OnActionPlayPause();
}

void cGtkmmMainWindow::SetStatePlaying()
{
  button.set_label("Pause");
}

void cGtkmmMainWindow::SetStatePaused()
{
  button.set_label("Play");
}
