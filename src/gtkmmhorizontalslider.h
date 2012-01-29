#ifndef gtkmmhorizontalslider_h
#define gtkmmhorizontalslider_h

// gtkmm headers
#include <gtkmm.h>

// http://git.gnome.org/browse/rhythmbox/tree/widgets/rb-header.c
// HACK: we want the behaviour you get with the middle button, so we
// mangle the event.  clicking with other buttons moves the slider in
// step increments, clicking with the middle button moves the slider to
// the location of the click.
#define BUILD_SUPPORT_SLIDER_DONT_JUMP_HACK

class cGtkmmMainWindow;

class cGtkmmHorizontalSlider : public Gtk::HScale {
public:
  friend class cGtkmmMainWindow;

  explicit cGtkmmHorizontalSlider(cGtkmmMainWindow& mainWindow);
  virtual ~cGtkmmHorizontalSlider() {}

  void SetValue(uint64_t uiValue);
  uint64_t GetValue() const;
  void SetRange(uint64_t uiMin, uint64_t uiMax);

private:
  #ifdef BUILD_SUPPORT_SLIDER_DONT_JUMP_HACK
  bool OnButtonPressReleaseEvent(GdkEventButton* event);
  #endif
  bool OnValueChanged(Gtk::ScrollType scrollType, double value);

  cGtkmmMainWindow& mainWindow;
};

#endif // gtkmmhorizontalslider_h
