#ifndef MEDUSA_H
#define MEDUSA_H

// Spitfire headers
#include <spitfire/util/string.h>

#define INVALID_TRACK nullptr

namespace medusa
{
  using spitfire::string_t;

  class cTrack;
  typedef const cTrack* trackid_t;
}

#endif // MEDUSA_H
