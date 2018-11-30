#include <thread>
#include <iostream>

#include "controller.h"

int main()
{
  user current_user; //user instance to construct a tsn_system
  tsn_system sys = tsn_system(current_user); //tsn_system instance for view constructor
  sys.load_user_data();

  view v = view(sys); //view instance for controller constructor
  v.print_start_menu();
  controller ctrl = controller(sys, v);
  std::thread bg (&controller::background, &ctrl); //starting the background threads

  while(1) //executing user commands
  {
    ctrl.execute_cmd();
  }

  return 0;
}