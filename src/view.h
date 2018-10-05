#ifndef VIEW_H
#define VIEW_H

class view;
#include "system.h"

class view
{
  public:
  tsn_system& sys;

  view(tsn_system& s) : sys(s) {};
  void print_main_menu();
  void print_edit_menu(); //kind of main
  void print_online(); //from main
  void show_user();  //from main
  void show_stats();

  

};
#endif