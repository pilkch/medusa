#ifndef MEDUSA_APPLICATION_H
#define MEDUSA_APPLICATION_H

// Spitfire headers
#include <spitfire/util/cConsoleApplication.h>

// Medusa headers
#include "medusa.h"

namespace medusa
{
  class cApplication : public spitfire::cConsoleApplication
  {
  public:
    cApplication(int argc, const char* const* argv);

  protected:
    int argc;
    const char* const* argv;

  private:
    virtual void _PrintHelp() const override;
    virtual string_t _GetVersion() const override;
    virtual bool _Run() override;
  };
}

#endif // MEDUSA_APPLICATION_H
