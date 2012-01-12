#ifndef gstreamermmplayer_h
#define gstreamermmplayer_h

// Standard headers
#include <string>

// gstreamermm headers
#include <gstreamermm.h>

class cGStreamermmPlayer
{
public:
  void Create(int argc, char** argv);
  void Destroy();

  void Play(const std::string& sFilePath);

private:
  Glib::RefPtr<Glib::MainLoop> loop;
  Glib::RefPtr<Gst::PlayBin> playbin;
  Glib::RefPtr<Gst::Bus> bus;
};

#endif // gstreamermmplayer_h
