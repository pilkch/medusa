// Standard headers
#include <iostream>
#include <string>

// Medusa headers
#include "application.h"

int main(int argc, char* argv[])
{
  std::cout<<"main"<<std::endl;

  bool bIsSuccess = false;

  {
    medusa::cApplication application(argc, argv);
    bIsSuccess = application.Run();
  }

  return bIsSuccess ? EXIT_SUCCESS : EXIT_FAILURE;
}
