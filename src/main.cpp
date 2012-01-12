// Standard headers
#include <iostream>
#include <string>

// TODO: INCLUDE SPITFIRE.H INSTEAD
#define nullptr NULL

// Medusa headers
#include "model.h"
#include "gtkmmview.h"
#include "controller.h"
#include "gstreamermmplayer.h"

int main(int argc, char* argv[])
{
  std::cout<<"main\n";

  cModel model;
  cGtkmmView view;
  cController controller(model, view);

  view.Run();

  cGStreamermmPlayer player;

  player.Create(argc, argv);
  player.Play("/home/chris/Music/collection/classic rock/Jefferson Airplane - White Rabbit.mp3");
  player.Destroy();

  return 0;
}
