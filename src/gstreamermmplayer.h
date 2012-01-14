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

  void LoadFile(const std::string& sFilePath);
  void Play();
  void Pause();
  void Stop();

  bool IsPlaying() const { return (state == STATE::PLAYING); }
  bool IsStopped() const { return (state == STATE::STOPPED); }

private:
  bool _OnBusMessage(const Glib::RefPtr<Gst::Bus>& bus, const Glib::RefPtr<Gst::Message>& message);

  cGtkmmView* pView;

  enum class STATE {
    PLAYING,
    STOPPED
  };

  STATE state;

  Glib::RefPtr<Glib::MainLoop> loop;
  Glib::RefPtr<Gst::PlayBin> playbin;
  Glib::RefPtr<Gst::Bus> bus;
  guint uiWatchID;
};

#endif // gstreamermmplayer_h
