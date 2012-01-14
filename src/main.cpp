// Standard headers
#include <iostream>
#include <string>

// Medusa headers
#include "model.h"
#include "gtkmmview.h"
#include "controller.h"
#include "gstreamermmplayer.h"

int main(int argc, char* argv[])
{
  std::cout<<"main\n";

  cModel model;
  cGtkmmView view(argc, argv);
  cController controller(model, view);

  view.Run();

  return 0;
}
