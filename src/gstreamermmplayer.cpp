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
  uiWatchID(0),
  positionMS(0)
{
}

void cGStreamermmPlayer::Create(int argc, char** argv)
{
  std::cout<<"cGStreamermmPlayer::Create\n";

  Gst::init(argc, argv);

  playbin = Gst::PlayBin2::create("playbin");

  ASSERT(playbin);

  bus = playbin->get_bus();

  // Register our bus message handler
  uiWatchID = bus->add_watch(sigc::mem_fun(*this, &cGStreamermmPlayer::_OnBusMessage));

  // Handle about to finish (We want to tell the listener so that it can queue another track for example)
  playbin->signal_about_to_finish().connect(sigc::mem_fun(*this, &cGStreamermmPlayer::OnAboutToFinish));
}

void cGStreamermmPlayer::Destroy()
{
  std::cout<<"cGStreamermmPlayer::Destroy\n";

  bus->remove_watch(uiWatchID);
  Stop();
}

bool cGStreamermmPlayer::_OnBusMessage(const Glib::RefPtr<Gst::Bus>& bus, const Glib::RefPtr<Gst::Message>& message)
{
  switch (message->get_message_type()) {
    case Gst::MESSAGE_EOS: {
      Stop();
      break;
    }
  }

  return true;
}

bool cGStreamermmPlayer::OnTimerPlaybackPosition()
{
  pView->OnPlayerUpdatePlaybackPosition();

  return true;
}

void cGStreamermmPlayer::OnAboutToFinish()
{
  pView->OnPlayerAboutToFinish();
}

void cGStreamermmPlayer::SetTrack(const cTrack* pTrack)
{
  std::wcout<<"cGStreamermmPlayer::SetTrack \""<<pTrack->sFilePath<<"\""<<std::endl;

  ASSERT(playbin);

  Stop();

  if (pTrack != nullptr) playbin->property_uri() = Glib::filename_to_uri(spitfire::string::ToUTF8(pTrack->sFilePath).c_str());
  else playbin->set_property("uri", Glib::ustring(""));

  pActiveTrack = pTrack;
}

void cGStreamermmPlayer::SetVolume0To100(unsigned int uiVolume0To100)
{
  playbin->set_property("volume", static_cast<double>(uiVolume0To100) / 100.0);
}

void cGStreamermmPlayer::Play()
{
  state = STATE::PLAYING;
  playbin->set_state(Gst::STATE_PLAYING);

  // Call OnTimerPlaybackPosition function at a 200ms
  // interval to regularly update the position of the stream
  timeoutConnection = Glib::signal_timeout().connect(sigc::mem_fun(*this, &cGStreamermmPlayer::OnTimerPlaybackPosition), 100);
}

void cGStreamermmPlayer::Pause()
{
  state = STATE::STOPPED;
  playbin->set_state(Gst::STATE_PAUSED);

  // Disconnect the progress signal handler
  timeoutConnection.disconnect();
}

void cGStreamermmPlayer::Stop()
{
  state = STATE::STOPPED;
  playbin->set_state(Gst::STATE_NULL);

  // Disconnect the progress signal handler
  timeoutConnection.disconnect();
}

void cGStreamermmPlayer::SeekMS(timepositionms_t timeMS)
{
  const uint64_t timeNS = timeMS * 1000000; // Convert to nanoseconds
  playbin->seek(Gst::FORMAT_TIME, Gst::SEEK_FLAG_FLUSH | Gst::SEEK_FLAG_KEY_UNIT, timeNS);
}

uint64_t cGStreamermmPlayer::GetLengthMS() const
{
  return (pActiveTrack != nullptr) ? pActiveTrack->metaData.uiDurationMilliSeconds : 0;
}

uint64_t cGStreamermmPlayer::GetPlaybackPositionMS() const
{
  Gst::Format fmt = Gst::FORMAT_TIME;
  gint64 pos = 0;
  if (playbin->query_position(fmt, pos)) {
    positionMS = pos / 1000000; // Convert to milliseconds
  }

  return positionMS;
}
