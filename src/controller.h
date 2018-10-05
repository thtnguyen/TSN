#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "view.h"

class controller
{
  public:
    tsn_system& sys;
    view& viewer;

    controller(tsn_system& s, view& v) : sys(s), viewer(v), last_request_time{0} {};
    void execute_cmd();
    void background();
    long last_request_time;

};
#endif