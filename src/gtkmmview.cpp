// Medusa headers
#include "gtkmmview.h"

cGtkmmView::cGtkmmView() :
  pMain(nullptr)
{
}

void cGtkmmView::Create(int argc, char** argv)
{
  //pMain = new Gtk::Main(argc, argv);
}

void cGtkmmView::_Run()
{
  // Run gtkmm main loop
  // NOTE: If you call run() without a window argument, hide() won't quit().
  //Gtk::Main::run();
  //Gtk::Main::run(mainWindow);

  //pMain->run(); // execution blocks here until pMain->quit() is called

  // pMain->quit() has now been called.
}
