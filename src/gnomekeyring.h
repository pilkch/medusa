#ifndef GNOMEKEYRING_H
#define GNOMEKEYRING_H

// Spitfire headers
#include <spitfire/util/signalobject.h>

// Medusa headers
#include "medusa.h"

namespace medusa
{
  // ** cGnomeKeyRing

  class cGnomeKeyRing
  {
  public:
    void LoadPassword(string_t& sPassword);
    void SavePassword(const string_t& sCaption, const string_t& sPassword);
  };
}

#endif // GNOMEKEYRING_H
