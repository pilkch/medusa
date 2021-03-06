// Standard headers
#include <iostream>
#include <string>

// Gtkmm headers
#include <gtkmm/main.h>

// Spitfire headers
#include <spitfire/util/string.h>
#include <spitfire/util/thread.h>

// Medusa headers
#include "application.h"
#include "model.h"
#include "gtkmmview.h"
#include "controller.h"
#include "gstreamermmplayer.h"

namespace medusa
{
  cApplication::cApplication(int _argc, const char* const* _argv) :
    spitfire::cConsoleApplication(_argc, _argv),
    argc(_argc),
    argv(_argv)
  {
  }

  void cApplication::_PrintHelp() const
  {
    std::cout<<"Usage: "<<spitfire::string::ToUTF8(GetApplicationName())<<" [OPTIONS]"<<std::endl;
    std::cout<<std::endl;
    std::cout<<" -dark, --dark Use the dark version of our current GTK theme if available"<<std::endl;
    std::cout<<" -help, --help Display this help and exit"<<std::endl;
    std::cout<<" -version, --version Display version information and exit"<<std::endl;
  }

  string_t cApplication::_GetVersion() const
  {
    std::ostringstream o;
    o<<GetApplicationName();
    o<<" "<<BUILD_APPLICATION_VERSION_STRING;
    return o.str();
  }

  bool cApplication::_Run()
  {
    char** _argv = (char**)(argv);
    Gtk::Main kit(argc, _argv);

    const bool bIsLight = (IsArgumentPresent("-light") || IsArgumentPresent("--light"));
    if (!bIsLight) {
      // Set our preference for dark themes before we create any widgets
      // http://stackoverflow.com/questions/15997464/how-do-i-make-my-gtk-app-use-the-dark-theme
      Gtk::Settings::get_default()->property_gtk_application_prefer_dark_theme().set_value(true);
    }

    medusa::cModel model;
    medusa::cGtkmmView view(argc, _argv);
    medusa::cController controller(model, view);

    view.Run();

    return true;
  }
}
