#ifndef MEDUSA_UTIL_H
#define MEDUSA_UTIL_H

// Spitfire headers
#include <spitfire/util/string.h>

namespace medusa
{
  namespace util
  {
    spitfire::string_t FormatNumber(uint64_t uiValue);
    spitfire::string_t FormatTime(uint64_t milliseconds);

    void ClearPassword(std::string& sPassword);
    void ClearPassword(std::wstring& sPassword);
  }
}

#endif // !MEDUSA_UTIL_H
