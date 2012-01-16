#ifndef gstreamermmplayer_h
#define gstreamermmplayer_h

// Standard headers
#include <string>

// gstreamermm headers
#include <gstreamermm.h>

// Spitfire headers
#include <spitfire/util/cString.h>

typedef uint64_t timepositionms_t;

class cGtkmmView;

class cGStreamermmPlayer
{
public:
  explicit cGStreamermmPlayer(cGtkmmView& view);

  void Create(int argc, char** argv);
  void Destroy();

  void SetFile(const spitfire::string_t& sFilePath);

  void Play();
  void Pause();
  void Stop();
  void SeekMS(timepositionms_t timeMS);

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

  Glib::RefPtr<Gst::PlayBin> playbin;
  Glib::RefPtr<Gst::Bus> bus;
  guint uiWatchID;
};

#endif // gstreamermmplayer_h
