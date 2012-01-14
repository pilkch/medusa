// Standard headers
#include <iostream>

// gtkmm headers
//#include <glibmm/main.h>

// Medusa headers
#include "gtkmmview.h"
#include "gstreamermmplayer.h"

// NOTE:
// The "Hello World" application in the GStreamer documentation shows how to play an Ogg/Vorbis file.
// To make this work with WAV files, you can simply replace "oggdemux" with "wavparse" and replace "vorbisdec" with "identity"

cGStreamermmPlayer::cGStreamermmPlayer(cGtkmmView& view) :
  pView(&view),
  state(STATE::STOPPED),
  uiWatchID(0)
{
}

void cGStreamermmPlayer::Create(int argc, char** argv)
{
  std::cout<<"cGStreamermmPlayer::Create\n";

  Gst::init(argc, argv);

  std::cout<<"cGStreamermmPlayer::Create 1\n";
  //loop = Glib::MainLoop::create();
  std::cout<<"cGStreamermmPlayer::Create 2\n";
  playbin = Gst::PlayBin::create();
}

void cGStreamermmPlayer::Destroy()
{
  std::cout<<"cGStreamermmPlayer::Destroy\n";

  bus->remove_watch(uiWatchID);
  Stop();
}

bool cGStreamermmPlayer::_OnBusMessage(const Glib::RefPtr<Gst::Bus>& bus, const Glib::RefPtr<Gst::Message>& message)
{
  // I am not sure if it should be Glib::RefPtr<Glib::MainLoop> &loop
  // or Glib::RefPtr<Glib::MainLoop> loop. It works though
  switch (message->get_message_type()) {
    case Gst::MESSAGE_EOS: {
      Stop();
      loop->quit();
      break;
    }
  }

  return true;
}

void cGStreamermmPlayer::LoadFile(const std::string& sFilePath)
{
  std::cout<<"cGStreamermmPlayer::LoadFile \""<<sFilePath<<"\"\n";

  Glib::ustring sPath = sFilePath.c_str();
  Glib::ustring sURL = "file://" + sPath;
  playbin->set_property("uri", sURL);

  // TODO: MOVE THIS INTO CREATE

  // get the bus
  bus = playbin->get_bus();
  // Add a bus watch. Bind to the slot the loop variable so you don't
  // have to declare it as global.
  //uiWatchID = bus->add_watch(sigc::bind(sigc::ptr_fun(&on_bus_callback), loop));
  //uiWatchID = bus->add_watch(sigc::ptr_fun(&on_bus_callback));
  uiWatchID = bus->add_watch(sigc::mem_fun(*this, &cGStreamermmPlayer::_OnBusMessage));

  std::cout<<"cGStreamermmPlayer::LoadFile returning\n";
}

void cGStreamermmPlayer::Play()
{
  state = STATE::PLAYING;
  playbin->set_state(Gst::STATE_PLAYING);

  // TODO: DO NOT BLOCK, use gtkmm to show a dialog and use the loop for the dialog instead
  //loop->run(); // execution blocks here until pMain->quit() is called
  // pMain->quit() has now been called.
}

void cGStreamermmPlayer::Pause()
{
  state = STATE::STOPPED;
  playbin->set_state(Gst::STATE_PAUSED);
}

void cGStreamermmPlayer::Stop()
{
  state = STATE::STOPPED;
  playbin->set_state(Gst::STATE_NULL);
}
