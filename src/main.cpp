// Standard headers
#include <iostream>
#include <string>

// Spitfire headers
#include <spitfire/util/thread.h>

// Medusa headers
#include "model.h"
#include "gtkmmview.h"
#include "controller.h"
#include "gstreamermmplayer.h"

int main(int argc, char* argv[])
{
  std::cout<<"main\n";

  spitfire::util::SetMainThread();

  cModel model;
  cGtkmmView view(argc, argv);
  cController controller(model, view);

  view.Run();

  return 0;
}
