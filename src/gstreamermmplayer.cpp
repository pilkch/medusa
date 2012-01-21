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

  playbin = Gst::PlayBin::create();

  bus = playbin->get_bus();

  // Register our bus message handler
  uiWatchID = bus->add_watch(sigc::mem_fun(*this, &cGStreamermmPlayer::_OnBusMessage));
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

void cGStreamermmPlayer::SetTrack(const cTrack* pTrack)
{
  std::wcout<<"cGStreamermmPlayer::SetTrack"<<std::endl;

  if (pTrack != nullptr) {
    const Glib::ustring sPath = spitfire::string::ToUTF8(pTrack->sFilePath).c_str();
    const Glib::ustring sURL = "file://" + sPath;
    playbin->set_property("uri", sURL);
  } else playbin->set_property("uri", Glib::ustring(""));

  Stop();

  pActiveTrack = pTrack;
}

void cGStreamermmPlayer::Play()
{
  state = STATE::PLAYING;
  playbin->set_state(Gst::STATE_PLAYING);
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
