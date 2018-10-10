#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "view.h"

class controller
{
  public:
    tsn_system& sys; //reference to a tsn_system instance
    view& viewer; //reference to a view instance
    long last_request_time; //used to keep track of when the last request was made by the user

    //constructor
    controller(tsn_system& s, view& v) : sys(s), viewer(v), last_request_time{0} {};

    void execute_cmd(); //get command inputs from the user and executes it
    void background(); //initializes and runs threads for all necessary background functions
    

};
#endif