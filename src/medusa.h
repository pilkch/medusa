#ifndef MEDUSA_H
#define MEDUSA_H

// Spitfire headers
#include <spitfire/util/string.h>

#define MEDUSA_WEB_SERVER_PORT 38001

#define INVALID_TRACK nullptr

namespace medusa
{
  using spitfire::char_t;
  using spitfire::string_t;

  class cTrack;
  typedef const cTrack* trackid_t;
}

#endif // MEDUSA_H
