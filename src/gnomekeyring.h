#ifndef GNOMEKEYRING_H
#define GNOMEKEYRING_H

// Spitfire headers
#include <spitfire/util/string.h>
#include <spitfire/util/signalobject.h>

namespace medusa
{
  // ** cGnomeKeyRing

  class cGnomeKeyRing
  {
  public:
    void LoadPassword(spitfire::string_t& sPassword);
    void SavePassword(const spitfire::string_t& sCaption, const spitfire::string_t& sPassword);
  };
}

#endif // GNOMEKEYRING_H
