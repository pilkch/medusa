#ifndef gstreamermmplayer_h
#define gstreamermmplayer_h

// Standard headers
#include <string>

// gstreamermm headers
#include <gstreamermm.h>

// Medusa headers
#include "medusa.h"
#include "track.h"

namespace medusa
{
  typedef uint64_t timepositionms_t;

  class cGtkmmView;

// ** cGStreamermmPlayer

class cGStreamermmPlayer
{
public:
  explicit cGStreamermmPlayer(cGtkmmView& view);

  void Create(int argc, char** argv);
  void Destroy();

  const cTrack* GetTrack() const { return pActiveTrack; }
  void SetTrack(const cTrack* pTrack);

  void SetVolume0To100(unsigned int uiVolume0To100);

  void Play();
  void Pause();
  void Stop();
  void SeekMS(timepositionms_t timeMS);

  bool IsPlaying() const { return (state == STATE::PLAYING); }
  bool IsStopped() const { return (state == STATE::STOPPED); }

  uint64_t GetLengthMS() const;
  uint64_t GetPlaybackPositionMS() const;

private:
  bool _OnBusMessage(const Glib::RefPtr<Gst::Bus>& bus, const Glib::RefPtr<Gst::Message>& message);
  bool OnTimerPlaybackPosition();
  void OnAboutToFinish();

  cGtkmmView* pView;

  enum class STATE {
    PLAYING,
    STOPPED
  };

  STATE state;

  Glib::RefPtr<Gst::PlayBin2> playbin;
  Glib::RefPtr<Gst::Bus> bus;
  guint uiWatchID;
  mutable uint64_t positionMS;

  const cTrack* pActiveTrack;

  sigc::connection timeoutConnection;
};
}

#endif // gstreamermmplayer_h
