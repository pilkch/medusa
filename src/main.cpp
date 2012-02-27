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
  std::cout<<"main"<<std::endl;

  spitfire::util::SetMainThread();

  medusa::cModel model;
  medusa::cGtkmmView view(argc, argv);
  medusa::cController controller(model, view);

  view.Run();

  return 0;
}
