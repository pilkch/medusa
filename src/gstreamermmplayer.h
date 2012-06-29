#ifndef gstreamermmplayer_h
#define gstreamermmplayer_h

// Standard headers
#include <string>

// gstreamermm headers
#include <gstreamermm.h>

// Medusa headers
#include "medusa.h"
#include "track.h"

//#define BUILD_MEDUSA_PLAYBIN2

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

  string_t GetTrack() const { return sActiveTrackFilePath; }
  void SetTrack(const string_t& sFilePath, uint64_t uiDurationMilliseconds);

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
  #ifdef BUILD_MEDUSA_PLAYBIN2
  void OnAboutToFinish();
  #endif

  cGtkmmView* pView;

  enum class STATE {
    PLAYING,
    STOPPED
  };

  STATE state;

  #ifdef BUILD_MEDUSA_PLAYBIN2
  Glib::RefPtr<Gst::PlayBin2> playbin;
  #else
  Glib::RefPtr<Gst::PlayBin> playbin;
  #endif
  Glib::RefPtr<Gst::Bus> bus;
  guint uiWatchID;
  mutable uint64_t positionMS;

  string_t sActiveTrackFilePath;
  uint64_t uiActiveTrackDurationMilliseconds;

  sigc::connection timeoutConnection;
};
}

#endif // gstreamermmplayer_h
