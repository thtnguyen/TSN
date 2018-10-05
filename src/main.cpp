#include <thread>
#include <sys/time.h>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <iomanip>
#include <cstdio>

//#include "controller.h"
#include "controller.h"

int main()
{
  std::cout << "Welcome to The Social Network." << std::endl;

  user current_user;
  tsn_system sys = tsn_system(current_user);
  sys.load_user_data();

  view v = view(sys);

  controller ctrl = controller(sys, v);
  std::thread bg (&controller::background, &ctrl);
  while(1)
  {
    ctrl.execute_cmd();
  }

  bg.join();

  return 0;
}