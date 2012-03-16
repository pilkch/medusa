// Standard headers
#include <iostream>

// Spitfire headers
#include <spitfire/util/thread.h>

// Medusa headers
#include "gtkmmdispatcher.h"

namespace medusa
{
  class cTest
  {
  public:
    void Test()
    {
      notify.Create(*this, &cTest::OnNotification);
      notify.Notify();
    }

  private:
    void OnNotification()
    {
      std::cout<<"cTest::OnNotification"<<std::endl;
      assert(spitfire::util::IsMainThread());
    }

    cGtkmmNotifyMainThread notify;
  };
}
