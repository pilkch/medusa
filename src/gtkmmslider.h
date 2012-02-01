#ifndef gtkmmslider_h
#define gtkmmslider_h

// gtkmm headers
#include <gtkmm.h>

// http://git.gnome.org/browse/rhythmbox/tree/widgets/rb-header.c
// HACK: We want the behaviour you get with the middle button, so we
// mangle the event.
// Clicking with other buttons moves the slider in step increments
// Clicking with the middle button moves the slider to the location of the click.
#define BUILD_GTKMM_SLIDER_DONT_JUMP_HACK

class cGtkmmSlider : public Gtk::Scale {
public:
  friend class cGtkmmMainWindow;

  cGtkmmSlider(cGtkmmMainWindow& mainWindow, bool bVertical);
  virtual ~cGtkmmSlider() {}

  void SetValue(uint64_t uiValue);
  uint64_t GetValue() const;
  void SetRange(uint64_t uiMin, uint64_t uiMax);

private:
  #ifdef BUILD_GTKMM_SLIDER_DONT_JUMP_HACK
  bool OnButtonPressReleaseEvent(GdkEventButton* event);
  #endif
  bool OnValueChanged(Gtk::ScrollType scrollType, double value);

  cGtkmmMainWindow& mainWindow;

  #ifdef BUILD_GTKMM_SLIDER_DONT_JUMP_HACK
  bool bLeftClickedPressEventChanged;
  #endif
};

#endif // gtkmmslider_h
