#ifndef MEDUSA_UTIL_H
#define MEDUSA_UTIL_H

// Medusa headers
#include "medusa.h"

namespace medusa
{
  namespace util
  {
    string_t FormatNumber(uint64_t uiValue);
    string_t FormatTime(uint64_t milliseconds);

    void ClearPassword(std::string& sPassword);
    void ClearPassword(std::wstring& sPassword);
  }
}

#endif // !MEDUSA_UTIL_H
