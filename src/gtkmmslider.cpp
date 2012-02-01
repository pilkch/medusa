// Standard headers
#include <iostream>
#include <iomanip>

// Medusa headers
#include "gtkmmview.h"
#include "gtkmmslider.h"

// ** cGtkmmSlider

cGtkmmSlider::cGtkmmSlider(cGtkmmMainWindow& _mainWindow, bool bVertical) :
  Scale(bVertical ? Gtk::Orientation::ORIENTATION_VERTICAL : Gtk::Orientation::ORIENTATION_HORIZONTAL),
  mainWindow(_mainWindow),
  bLeftClickedPressEventChanged(false)
{
  set_draw_value(false);
  set_digits(0);
  set_increments(5, 5);

  // Make vertical sliders increase upwards
  if (bVertical) set_inverted(true);

  #ifdef BUILD_GTKMM_SLIDER_DONT_JUMP_HACK
  signal_button_press_event().connect(sigc::mem_fun(*this, &cGtkmmSlider::OnButtonPressReleaseEvent), false);
  signal_button_release_event().connect(sigc::mem_fun(*this, &cGtkmmSlider::OnButtonPressReleaseEvent), false);
  #endif

  signal_change_value().connect(sigc::mem_fun(*this, &cGtkmmSlider::OnValueChanged));
}

#ifdef BUILD_GTKMM_SLIDER_DONT_JUMP_HACK
// Handle clicking on the slider correctly
// 1) Left click on the slider will change the click into a middle click event (Jump to position)
bool cGtkmmSlider::OnButtonPressReleaseEvent(GdkEventButton* event)
{
  std::cout<<"cGtkmmSlider::OnButtonPressReleaseEvent\n";

  // Change our left click into a middle click
  if (!bLeftClickedPressEventChanged) {
    if ((event->type == GDK_BUTTON_PRESS) && (event->button == 1)) {
      std::cout<<"cGtkmmSlider::OnButtonPressReleaseEvent Left click\n";
      event->button = 2;
      bLeftClickedPressEventChanged = true;
      Scale::on_button_press_event(event);
      return true;
    }
  } else {
    if (event->type == GDK_BUTTON_RELEASE) {
      event->button = 2;
      bLeftClickedPressEventChanged = false;
    }
  }

  return Scale::on_button_press_event(event);
}
#endif // BUILD_GTKMM_SLIDER_DONT_JUMP_HACK

bool cGtkmmSlider::OnValueChanged(Gtk::ScrollType scrollType, double value)
{
  std::cout<<"cGtkmmSlider::OnValueChanged"<<std::endl;

  mainWindow.OnActionSliderValueChanged(*this, value);

  return true;
}

uint64_t cGtkmmSlider::GetValue() const
{
  return uint64_t(get_value());
}

void cGtkmmSlider::SetValue(uint64_t uiValue)
{
  set_value(double(uiValue));
}

void cGtkmmSlider::SetRange(uint64_t uiMin, uint64_t uiMax)
{
  set_range(double(uiMin), double(uiMax));
}
