#ifndef VIEW_H
#define VIEW_H

class view;
#include "system.h"

class view
{
  public:
  tsn_system& sys; //reference to a tsn_system instance

  view(tsn_system& s) : sys(s) {}; //constructor
  void print_start_menu();
  void print_help_menu();
  void print_edit_menu();
  void print_online(); //prints names of nodes on the network
  void show_user(); //prints user information and posts of a selected user
  void show_stats(); //shows stats about other known nodes and post content on the network
  void print_messages(); //prints all private messages

};
#endif