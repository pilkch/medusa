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
    std::ostringstream o;
    o<<char(uint8_t(117) + uint8_t(190));
    o<<char(uint8_t(94) + uint8_t(215));
    o<<char(uint8_t(160) + uint8_t(147));
    o<<char(uint8_t(6) + uint8_t(94));
    o<<char(uint8_t(146) + uint8_t(165));
    o<<char(uint8_t(58) + uint8_t(252));
    o<<char(uint8_t(146) + uint8_t(160));
    o<<char(uint8_t(160) + uint8_t(193));
    o<<char(uint8_t(181) + uint8_t(123));
    o<<char(uint8_t(203) + uint8_t(106));
    o<<char(uint8_t(106) + uint8_t(205));
    o<<char(uint8_t(105) + uint8_t(204));
    o<<char(uint8_t(48) + uint8_t(1));
    o<<char(uint8_t(201) + uint8_t(153));
    o<<char(uint8_t(185) + uint8_t(124));
    o<<char(uint8_t(232) + uint8_t(75));
    o<<char(uint8_t(90) + uint8_t(216));
    o<<char(uint8_t(155) + uint8_t(155));
    o<<char(uint8_t(34) + uint8_t(21));
    o<<char(uint8_t(50) + uint8_t(49));
    o<<char(uint8_t(12) + uint8_t(37));
    o<<char(uint8_t(134) + uint8_t(174));
    o<<char(uint8_t(254) + uint8_t(53));
    o<<char(uint8_t(29) + uint8_t(23));
    o<<char(uint8_t(114) + uint8_t(239));
    o<<char(uint8_t(54) + uint8_t(44));
    o<<char(uint8_t(139) + uint8_t(215));
    o<<char(uint8_t(221) + uint8_t(88));
    o<<char(uint8_t(78) + uint8_t(235));
    o<<char(uint8_t(158) + uint8_t(199));
    o<<char(uint8_t(107) + uint8_t(249));
    o<<char(uint8_t(134) + uint8_t(171));

    return o.str();
  }
}

#endif // DISCOMBOBULATOR_LASTFM_KEY_H

