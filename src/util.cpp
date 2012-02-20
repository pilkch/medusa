// Standard headers
#include <sstream>
#include <iomanip>

// Medusa headers
#include "util.h"

namespace medusa
{
  namespace util
  {
    spitfire::string_t FormatNumber(uint64_t uiValue)
    {
      if (uiValue == 0) return TEXT("");

      return spitfire::string::ToString(uiValue);
    }

    spitfire::string_t FormatTime(uint64_t milliseconds)
    {
      spitfire::ostringstream_t o;

      o<<std::right<<std::setfill(TEXT('0'));

      uint64_t time = milliseconds / 1000;

      const uint64_t seconds = time % 60;
      time /= 60;

      const uint64_t minutes = time % 60;
      time /= 60;

      const uint64_t hours = time % 24;
      time /= 24;

      const uint64_t days = hours;

      if (days != 0) {
        o<<days<<TEXT(":");
        o<<std::setw(2); // Set width of 2 for the next value
      }

      if (hours != 0)  {
        o<<hours<<TEXT(":");
        o<<std::setw(2); // Set width of 2 for the next value
      }

      o<<minutes<<":";
      o<<std::setw(2)<<seconds;

      return o.str();
    }

    void ClearPassword(std::string& sPassword)
    {
      const size_t n = sPassword.length();
      for (size_t i = 0; i < n; i++) sPassword[i] = 0;
    }

    void ClearPassword(std::wstring& sPassword)
    {
      const size_t n = sPassword.length();
      for (size_t i = 0; i < n; i++) sPassword[i] = 0;
    }
  }
}
