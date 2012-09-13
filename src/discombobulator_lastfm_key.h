// This header was automatically generate by discombobulator
// https://github.com/pilkch/tests/tree/master/discombobulator

#ifndef DISCOMBOBULATOR_LASTFM_KEY_H
#define DISCOMBOBULATOR_LASTFM_KEY_H

#include <string>
#include <sstream>

// The point of this function is to avoid storing any sensitive data as plain text in the executable
// This should be checked, a smart compiler may precompute these values and store them as plain text anyway
// Note that the function relies on the values wrapping at 255

namespace discombobulator
{
  inline std::string GetSecretLastfmKeyUTF8()
  {
    #error "Your discombobulated key would go here, otherwise you can just remove this line and return an empty string if you don't want last.fm support"
    return "";
  }
}

#endif // DISCOMBOBULATOR_LASTFM_KEY_H

