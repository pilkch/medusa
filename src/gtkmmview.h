#ifndef gtkmmview_h
#define gtkmmview_h

// glib headers
//#include <glibmm/main.h>

// gtkmm headers
#include <gtkmm.h>

#include "view.h"

class cGStreamermmPlayer;

class cGtkmmView : public cView
{
public:
  friend class cGStreamermmPlayer;

  cGtkmmView();

  void Create(int argc, char** argv);

protected:
  const Gtk::Main* GetMainLoop() const { return pMain; }
  Gtk::Main* GetMainLoop() { return pMain; }

private:
  void _Run();

  Gtk::Main* pMain;
};

#endif // gtkmmview_h
