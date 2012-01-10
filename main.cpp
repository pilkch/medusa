#include <gstreamermm.h>
#include <glibmm/main.h>
#include <iostream>

bool on_bus_callback(const Glib::RefPtr<Gst::Bus> &bus, const Glib::RefPtr<Gst::Message> &message, Glib::RefPtr<Glib::MainLoop> &loop)
{
  // I am not sure if it should be Glib::RefPtr<Glib::MainLoop> &loop
  // or Glib::RefPtr<Glib::MainLoop> loop. It works though
  switch(message->get_message_type())
  {
    case Gst::MESSAGE_EOS:
      loop->quit();
      break;
  }

  return true;
}


// The "Hello World" application in the GStreamer documentation shows how to play an Ogg/Vorbis file.
// To make this work with WAV files, you can simply replace "oggdemux" with "wavparse" and replace "vorbisdec" with "identity"

int main(int argc, char* argv[]) {
  std::cout<<"main\n";
  Gst::init(argc, argv);
  std::cout<<"main 1\n";
  Glib::RefPtr<Glib::MainLoop> loop = Glib::MainLoop::create();
  std::cout<<"main 2\n";
  Glib::RefPtr<Gst::PlayBin> playbin = Gst::PlayBin::create();
  std::cout<<"main 3\n";
  Glib::ustring sPath = argv[1];
  std::cout<<"main 4\n";
  Glib::ustring sURL = "file://" + sPath;
  std::cout<<"main 5\n";
  playbin->set_property("uri", sURL);
  std::cout<<"main 6\n";

  // get the bus
  Glib::RefPtr<Gst::Bus> bus = playbin->get_bus();
  // Add a bus watch. Bind to the slot the loop variable so you don't
  // have to declare it as global.
  bus->add_watch(sigc::bind(sigc::ptr_fun(&on_bus_callback), loop));
  playbin->set_state(Gst::STATE_PLAYING);
  loop->run(); // execution blocks here until loop->quit() is called
  // loop->quit() has been called.
  std::cout<<"done\n";
  playbin->set_state(Gst::STATE_NULL);
  return 0;
}
