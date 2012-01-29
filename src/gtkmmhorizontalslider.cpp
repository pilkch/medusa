// Standard headers
#include <iostream>
#include <iomanip>

// Medusa headers
#include "gtkmmview.h"
#include "gtkmmhorizontalslider.h"

// ** cGtkmmHorizontalSlider

cGtkmmHorizontalSlider::cGtkmmHorizontalSlider(cGtkmmMainWindow& _mainWindow) :
  mainWindow(_mainWindow)
{
  set_draw_value(false);
  set_digits(0);
  set_increments(5, 5);

  #ifdef BUILD_SUPPORT_SLIDER_DONT_JUMP_HACK
  signal_button_press_event().connect(sigc::mem_fun(*this, &cGtkmmHorizontalSlider::OnButtonPressReleaseEvent), false);
  signal_button_release_event().connect(sigc::mem_fun(*this, &cGtkmmHorizontalSlider::OnButtonPressReleaseEvent), false);
  #endif

  signal_change_value().connect(sigc::mem_fun(*this, &cGtkmmHorizontalSlider::OnValueChanged));
}

#ifdef BUILD_SUPPORT_SLIDER_DONT_JUMP_HACK
bool bLeftClickedPressEventChanged = false;
// Handle clicking on the slider correctly
// 1) Left click on the slider will change the click into a middle click event (Jump to position)
bool cGtkmmHorizontalSlider::OnButtonPressReleaseEvent(GdkEventButton* event)
{
  std::cout<<"cGtkmmHorizontalSlider::OnButtonPressReleaseEvent\n";

  // Change our left click into a middle click
  if (!bLeftClickedPressEventChanged) {
    if ((event->type == GDK_BUTTON_PRESS) && (event->button == 1)) {
      std::cout<<"cGtkmmHorizontalSlider::OnButtonPressReleaseEvent Left click\n";
      event->button = 2;
      bLeftClickedPressEventChanged = true;
      HScale::on_button_press_event(event);
      return true;
    }
  } else {
    if (event->type == GDK_BUTTON_RELEASE) {
      event->button = 2;
      bLeftClickedPressEventChanged = false;
    }
  }

  return HScale::on_button_press_event(event);
}
#endif // BUILD_SUPPORT_SLIDER_DONT_JUMP_HACK

bool cGtkmmHorizontalSlider::OnValueChanged(Gtk::ScrollType scrollType, double value)
{
  std::cout<<"cGtkmmHorizontalSlider::OnValueChanged\n";

  mainWindow.OnActionSliderValueChanged(*this, value);

  return true;
}

uint64_t cGtkmmHorizontalSlider::GetValue() const
{
  return uint64_t(get_value());
}

void cGtkmmHorizontalSlider::SetValue(uint64_t uiValue)
{
  set_value(double(uiValue));
}

void cGtkmmHorizontalSlider::SetRange(uint64_t uiMin, uint64_t uiMax)
{
  set_range(double(uiMin), double(uiMax));
}
