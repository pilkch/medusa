#ifndef gstreamermmplayer_h
#define gstreamermmplayer_h

// Standard headers
#include <string>

// gstreamermm headers
#include <gstreamermm.h>

class cGtkmmView;

class cGStreamermmPlayer
{
public:
  explicit cGStreamermmPlayer(cGtkmmView& view);

  void Create(int argc, char** argv);
  void Destroy();

  void Play(const std::string& sFilePath);

private:
  cGtkmmView* pView;

  Glib::RefPtr<Glib::MainLoop> loop;
  Glib::RefPtr<Gst::PlayBin> playbin;
  Glib::RefPtr<Gst::Bus> bus;
};

#endif // gstreamermmplayer_h
