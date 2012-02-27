#ifndef gtkmmicontheme_h
#define gtkmmicontheme_h

// Standard headers
#include <string>

// gtkmm headers
#include <gtkmm.h>

namespace medusa
{
  class cGtkmmIconTheme
  {
  public:
    cGtkmmIconTheme();

    template <class T>
    void RegisterThemeChangedListener(T& listener);

    void LoadStockIcon(const std::string& sStockIconName, Gtk::Image& image);
    void LoadStockIconRotatedClockwise(const std::string& sStockIconName, Gtk::Image& image);

  private:
    Glib::RefPtr<Gtk::IconTheme> icon_theme;
  };

  template <class T>
  void cGtkmmIconTheme::RegisterThemeChangedListener(T& listener)
  {
    // Connect changed signal of IconTheme
    icon_theme->signal_changed().connect(sigc::mem_fun(listener, &T::OnThemeChanged));
  }
}

#endif // gtkmmicontheme_h
